#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include <iostream>
#include "../third_party/json.hpp" 
#include "../lib/graph.h"
#include "../lib/graph_algo.h"
#include "../lib/graph_tool.h"


using json = nlohmann::json;
using namespace apsara::odps::graph::query;

using namespace std;

char uri_root[512];

static void syntax(void)
{
    fprintf(stdout, "Syntax: queryServer <docroot>\n");
}

template <typename T>
void RunQuery(const json &payload, shared_ptr<Graph<T>> &g)
{
    GraphAlgo<T> algo(g);
    algo.SetNumThreads(payload["threadnum"]);
    double start = omp_get_wtime();
    cout<<"Query: "<<payload["query"]<<" result:"<<algo.BFS(payload["query"])<<endl;
    double end = omp_get_wtime();
    cout<<" time taken:"<<end-start<<endl;
}

void Process(const json &payload)
{
    static shared_ptr<Graph<uint32_t>> gCSRUint32 = 
        shared_ptr<Graph<uint32_t>>(new GraphCSR<uint32_t>());
    static shared_ptr<Graph<uint64_t>> gCSRUint64 = 
        shared_ptr<Graph<uint64_t>>(new GraphCSR<uint64_t>());
    static shared_ptr<Graph<uint32_t>> gAdjUint32 = 
        shared_ptr<Graph<uint32_t>>(new GraphAdj<uint32_t>());
    static shared_ptr<Graph<uint64_t>> gAdjUint64 = 
        shared_ptr<Graph<uint64_t>>(new GraphAdj<uint64_t>());

    string rootDir = "/apsarapangu/disk8/twitter-2010/bfs_query/test/";
    string format = payload["format"];
    string encode = payload["encode"];
    string dir = rootDir+format+"/"+encode+"/";

    if(payload["format"] == "CSR")
    {
        if(payload["encode"] == "uint32")
        {
            gCSRUint32->Init(dir);
            RunQuery<uint32_t>(payload, gCSRUint32);
        }
        else if(payload["encode"] == "uint64")
        {
            gCSRUint64->Init(dir);
            RunQuery<uint64_t>(payload, gCSRUint64);
        }
    }

    if(payload["format"] == "Adj")
    {
        if(payload["encode"] == "uint32")
        {
            gAdjUint32->Init(dir);
            RunQuery<uint32_t>(payload, gCSRUint32);
        }
        else if(payload["encode"] == "uint64")
        {
            gAdjUint64->Init(dir);
            RunQuery<uint64_t>(payload, gAdjUint64);
        }
    }
}

/* Callback used for the /dump URI, and for every non-GET request:
 * dumps all information to stdout and gives back a trivial 200 ok */
    static void
graph_query_cb(struct evhttp_request *req, void *arg)
{
    const char *cmdtype;
    struct evbuffer *buf;

    switch (evhttp_request_get_command(req)) {
        case EVHTTP_REQ_GET:    cmdtype = "GET";     break;
        case EVHTTP_REQ_POST:   cmdtype = "POST";    break;
        case EVHTTP_REQ_PUT:    cmdtype = "PUT";     break;
        case EVHTTP_REQ_DELETE: cmdtype = "DELETE";  break;
        case EVHTTP_REQ_PATCH:  cmdtype = "PATCH";   break;
        default:                cmdtype = "unknown"; break;
    }

    printf("Received a %s request for %s\n",
            cmdtype, evhttp_request_get_uri(req));

    buf = evhttp_request_get_input_buffer(req);
    string cmd;
    while (evbuffer_get_length(buf)) {
        int n;
        char cbuf[128];
        n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
        cbuf[n] = '\0';
        cmd += cbuf;
    }
    cout<<"\ncmd: "<<cmd<<endl;

    auto payload = json::parse(cmd);
    Process(payload);

    evhttp_send_reply(req, 200, "OK", NULL);
}

static void
send_document_cb(struct evhttp_request *req, void *arg)
{
}

int main(int argc, char **argv)
{
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;

    ev_uint16_t port = 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return (1);

    if (argc < 2) {
        syntax();
        return 1;
    }

    base = event_base_new();
    if (!base) 
    {
        fprintf(stderr, "Couldn't create an event_base: exiting\n");
        return 1;
    }

    http = evhttp_new(base);
    if (!http) {
        fprintf(stderr, "couldn't create evhttp. Exiting.\n");
        return 1;
    }

    /* The /dump URI will dump all requests to stdout and say 200 ok. */
    evhttp_set_cb(http, "/graphQuery", graph_query_cb, NULL);

    /* We want to accept arbitrary requests, so we need to set a "generic"
     * cb.  We can also add callbacks for specific paths. */
    evhttp_set_gencb(http, send_document_cb, argv[1]);

    /* Now we tell the evhttp what port to listen on */
    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", 8414);
    if (!handle) {
        fprintf(stderr, "couldn't bind to port %d. Exiting.\n",
                (int)port);
        return 1;
    }

    {
        /* Extract and display the address we're listening on. */
        struct sockaddr_storage ss;
        evutil_socket_t fd;
        ev_socklen_t socklen = sizeof(ss);
        char addrbuf[128];
        void *inaddr;
        const char *addr;
        int got_port = -1;
        fd = evhttp_bound_socket_get_fd(handle);
        memset(&ss, 0, sizeof(ss));
        if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
            perror("getsockname() failed");
            return 1;
        }
        if (ss.ss_family == AF_INET) {
            got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
            inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
        } else if (ss.ss_family == AF_INET6) {
            got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
            inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
        } else {
            fprintf(stderr, "Weird address family %d\n",
                    ss.ss_family);
            return 1;
        }
        addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
                sizeof(addrbuf));
        if (addr) {
            printf("Listening on %s:%d\n", addr, got_port);
            evutil_snprintf(uri_root, sizeof(uri_root),
                    "http://%s:%d",addr,got_port);
        } else {
            fprintf(stderr, "evutil_inet_ntop failed\n");
            return 1;
        }
    }

    event_base_dispatch(base);

    return 0;

}
