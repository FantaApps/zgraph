#ifndef _ODPS_GRAPH_BNB_
#define _ODPS_GRAPH_BNB_

#include <memory>
#include <fstream>
#include <stack>
#include <set>
#include <unordered_set>
#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <iomanip>
#include <cstring>
#include "zstd_compress.h" 
#include "graph.h"

namespace apsara {
namespace odps {
namespace graph {
namespace query {

template <typename VType>
struct BiClique
{
    std::vector<VType> mFromSet;
    std::vector<VType> mToSet;

    void print()
    {
        std::cout<<"-----"<<std::endl;
        for(size_t i=0; i<mFromSet.size(); ++i)
            std::cout<<mFromSet[i]<<" ";
        std::cout<<std::endl;
        for(size_t i=0; i<mToSet.size(); ++i)
            std::cout<<mToSet[i]<<" ";
        std::cout<<std::endl;

    }
};

class ResCompare 
{
public:
    bool operator()(const std::pair<int, int> x,
                    const std::pair<int, int> y)
    {
        if(x.second < y.second)
            return true;
        else if(x.second > y.second)
            return false;
        else
        {
            return x.first < y.first;
        }
    }
};

template <typename VType>
class GraphBnB
{
public:

    GraphBnB(const std::shared_ptr<Graph<VType>> &g,
             const std::string                   &folder,
             bool                                 isRecover = false) :
        mFolder(folder),
        mStart(0), 
        mEnd(0),
        mTot(0),
        mCountLCheck(0),
        mLastRes(false),
        mCountRevLCheck(0),
        mReportInterval(100000),
        mCheckPtInterval(100000)
    { 
        mG = g; 

        if(isRecover)
            InitRecover();
        else
            InitRaw();
        
        mCnt = new VType[mG->GetNumV()];
        mCntTime = new uint32_t[mG->GetNumV()];
        for(size_t i=0; i<mG->GetNumV(); i++)
        {
            mCnt[i] = 0; 
            mCntTime[i] = 0; 
        }
        mTime = 0;
    }

    virtual ~GraphBnB()
    {
        delete [] mCnt;
        delete [] mCntTime;
    }

    void SetNumThreads(int numThreads) 
    { 
        mNumThreads = numThreads;
    }

    void EnumerateBiCliqueBasic() 
    { 
        mStart = omp_get_wtime();
        EnumerateBiCliqueRecursive(mP, mL, mR, mQ, 1); 
        std::cout<<"\n\n----Result:-----\n\n";
        for(auto iter = res.begin(); iter != res.end(); ++iter)
        {
            std::string key = "K"+std::to_string(iter->first.first)+","+std::to_string(iter->first.second);
            std::cout<<key<<"\t"<<iter->second<<std::endl;
        }

        std::cout<<"total biclique: "<<mTot<<std::endl;
    }

    
private:

    void EnumerateBiCliqueRecursive(std::set<VType>    &P,
                                    std::vector<VType> &L,
                                    std::set<VType>    &R,
                                    std::set<VType>    &Q,
                                    int                 lvl);

    void GetNeighbour(std::vector<VType> &Lp,
                      std::vector<VType> &neigh,
                      VType              &x);

    bool EarlyFalseCheck(std::set<VType> &Q,
                         std::vector<VType> &Lp,
                         std::set<VType> &Qp);

    bool EarlyTrueCheck(std::set<VType> &Q,
                        std::vector<VType> &Lp,
                        std::set<VType> &Qp);

    bool CheckMaximalNew(std::set<VType> &Q,
                         std::vector<VType> &Lp,
                         std::set<VType> &Qp);

    void MergeRNew(std::set<VType> &P,
                   std::vector<VType> &Lp,
                   std::set<VType> &Rp,
                   std::set<VType> &Pp);

    void InsertResult(const std::vector<VType> &Lp,
                      const std::set<VType> &Rp);

    void SaveStatus(std::set<VType>    &P,
                    std::vector<VType> &L,
                    std::set<VType>    &R,
                    std::set<VType>    &Q,
                    const VType        &x);


    void SaveBicliqueResult();

    void InitRaw();

    void InitRecover();

private:

    std::string GetKey(std::vector<VType> &Lp)
    {
        std::string ret="";
        for(size_t i=0; i<Lp.size(); i++)
            ret += "-"+std::to_string(Lp[i]);
        return ret;
    }

    static bool PairCompare(const std::pair<VType, size_t>& lhs,
                            const std::pair<VType, size_t>& rhs)
    {
        return lhs.second > rhs.second;
    }

    void Print(std::vector<VType> &v, std::string name)
    {
        std::cout<<name<<" ";
        for(auto i: v)
            std::cout<<i<<" ";
        std::cout<<std::endl;
    }
    void Print(std::set<VType> &v, std::string name)
    {
        std::cout<<name<<" ";
        for(auto i: v)
            std::cout<<i<<" ";
        std::cout<<std::endl;
    }

    int BinarySearch(std::shared_ptr<VType> &arr,
                     VType value, 
                     int l,
                     int r)
    {
        while (l <= r)
        {
            int m = l + (r-l)/2;
            if (arr.get()[m] == value)
            {
                return m;
            }
            if (arr.get()[m] < value)
                l = m + 1;
            else
                r = m - 1;
        }
        return -1;
    }

private:
    int                                            mNumThreads;
    std::shared_ptr<Graph<VType>>                  mG;
    std::string                                    mFolder;

    std::set<VType>                                mP;
    std::vector<VType>                             mL;
    std::set<VType>                                mR;
    std::set<VType>                                mQ;

    uint32_t                                       mTime;
    VType                                          *mCnt;
    uint32_t                                       *mCntTime;
    std::vector<VType>                             mLastLp;
    bool                                           mLastRes;
    std::unordered_set<std::string>                mHot;

    double                                         mStart; 
    double                                         mEnd;
    long                                           mTot;
    long                                           mCountLCheck;
    long                                           mCountRevLCheck;
    VType                                          mLastToExamine;
    VType                                          mExamineNow;
    long                                           mReportInterval;
    long                                           mCheckPtInterval;

    std::map<std::pair<int, int>, int, ResCompare> res;
    std::vector<BiClique<VType>>                   ret;
};

template<typename VType>
void GraphBnB<VType>::EnumerateBiCliqueRecursive(std::set<VType>    &P,
                                                 std::vector<VType> &L,
                                                 std::set<VType>    &R,
                                                 std::set<VType>    &Q,
                                                 int                 lvl)
{
    while(!P.empty())
    {
        auto it = P.end();
        --it; VType x = *it;      P.erase(it); 
        std::set<VType> Rp=R; Rp.insert(x);
        std::vector<VType> Lp;   GetNeighbour(L, Lp, x);
        std::set<VType> Pp;
        std::set<VType> Qp;

        bool earlyCheck = EarlyFalseCheck(Q,Lp,Qp);
        if(earlyCheck)
            earlyCheck = EarlyTrueCheck(Q,Lp,Qp);
        bool isMaximal=false;
        if(earlyCheck)
        {
            isMaximal = CheckMaximalNew(Q, Lp, Qp);
        }

        if(isMaximal)
        {

            MergeRNew(P,Lp,Rp,Pp);


            InsertResult(Lp,Rp);
            
            if(!Pp.empty())
            {
                EnumerateBiCliqueRecursive(Pp, Lp, Rp, Qp, lvl+1);
                if(lvl==1)
                {
                    if(Q.size() % mCheckPtInterval == 0)
                    {
                        SaveStatus(P, L, R, Q, x);
                    }
                    mExamineNow = x;
                }
            }
        }
        Q.insert(x);
    }
}

template<typename VType>
void GraphBnB<VType>::GetNeighbour(std::vector<VType> &L,
                                   std::vector<VType> &neigh,
                                   VType              &x)
{
    std::set<VType> st;
    size_t from, to;
    std::shared_ptr<VType> &edges = mG->GetRevEdges(x, from, to); 
    size_t range = (to-from); 
    size_t lsize = L.size(); 
    /*if(range> 10000 || (range>50 && L.size()<15)) // TODO both are sorted, optimize this
    {
        for(size_t i=0; i<L.size(); i++)
        {
            mCountLCheck++;
            if(BinarySearch(edges, L[i], (int)from, (int)to) != -1)
            {
                st.insert(L[i]);
            }
        }
    }
    else if(L.size() > 100 && range < 100)
    {
        for(size_t i=from; i<to; ++i)
        {
            mCountRevLCheck++;
            VType key = edges.get()[i];
            if(std::binary_search(L.begin(), L.end(), key))
            {
                st.insert(key);
            }
            if(neigh.size() == L.size())
                return;
        }
    }
    //else if(range*fls(lsize) > lsize*fls(range))
    //{
    //    for(size_t i=0; i<L.size(); i++)
    //    {
    //        mCountLCheck++;
    //        if(BinarySearch(edges, L[i], (int)from, (int)to) != -1)
    //        {
    //            st.insert(L[i]);
    //        }
    //    }
    //}
    else*/ //TODO reduce all these complexity to get correctness for nowß
    {
        for(size_t i=from; i<to; ++i)
        {
            mCountRevLCheck++;
            VType key = edges.get()[i];
            if(std::binary_search(L.begin(), L.end(), key))
            {
                st.insert(key);
            }
            if(neigh.size() == L.size())
                return; // TODO understand this 
        }
    }
    neigh.assign(st.begin(), st.end());
}

template<typename VType>
bool GraphBnB<VType>::EarlyTrueCheck(std::set<VType> &Q,
                                     std::vector<VType> &Lp,
                                     std::set<VType> &Qp)
{
    if(Lp.size() == 0)
        return false;
    std::vector<VType> common;
    if(Lp.size() >= 2)
    {
        size_t from1=0, to1=0;
        std::shared_ptr<VType> &edges1 = mG->GetEdges(Lp[0], from1, to1); 

        size_t from2=0, to2=0;
        std::shared_ptr<VType> &edges2 = mG->GetEdges(Lp[1], from2, to2); 

        size_t i=from1, j=from2;
        while(i<to1 && j < to2)
        {
            VType e1 = edges1.get()[i];
            VType e2 = edges2.get()[j];
            if(e1 == e2)
            {
                if(Q.find(e1) != Q.end())
                    common.push_back(e1);
                i++; j++;
            }
            else if(e1 < e2)
                i++;
            else
                j++;
        }
        
    }
    if(Lp.size()==2 && common.size()>0)
    {
        mHot.insert(GetKey(Lp));
        mLastLp  = Lp;
        mLastRes = false;
        return false;
    }
    else if(Lp.size()>2)
    {
        for(auto i : common)
        {
            int s = 2;
            if(Q.find(i) != Q.end())
            {
                for(size_t j=2; j<Lp.size(); j++)
                {
                    size_t from=0, to=0;
                    std::shared_ptr<VType> &edges = mG->GetEdges(Lp[j], from, to); 
                    int found = BinarySearch(edges, i, (int)from, (int)to);
                    if(found>=(int)from && found<(int)to)
                    {
                        s++;
                    }
                    else
                        break;
                }
            }
            if(s == (int)Lp.size())
            {
                mHot.insert(GetKey(Lp));
                mLastLp  = Lp;
                mLastRes = false;
                return false;
            }
        }
    }
    
    return true;
}

template<typename VType>
bool GraphBnB<VType>::EarlyFalseCheck(std::set<VType> &Q,
                                      std::vector<VType> &Lp,
                                      std::set<VType> &Qp)
{
    mTime++;
    if(mLastRes == false)
    {
        if(mLastLp == Lp)
        {
            mLastLp  = Lp;
            mLastRes = false;
            return false;
        }
        else if(mHot.find(GetKey(Lp)) != mHot.end())
        {
            mLastLp  = Lp;
            mLastRes = false;
            return false;
        }
        
    }
    if(Lp.size() == 1)
    {
        size_t from=0, to=0;
        std::shared_ptr<VType> &edges = mG->GetEdges(Lp[0], from, to); // TODO optimize this 
        for(size_t i=from; i<to; i++)
        {
            if(Q.find(edges.get()[i]) != Q.end()) 
            {
                mHot.insert(GetKey(Lp));
                mLastLp  = Lp;
                mLastRes = false;
                return false;
            }
        }

    }
    
    return true;
}

template<typename VType>
bool GraphBnB<VType>::CheckMaximalNew(std::set<VType> &Q,
                                      std::vector<VType> &Lp,
                                      std::set<VType> &Qp)
{
    mTime++;
    std::vector<VType> tmp;

    for(size_t i=0; i<Lp.size(); i++)
    {
        size_t from=0, to=0;
        std::shared_ptr<VType> &edges = mG->GetEdges(Lp[i], from, to); 
        for(size_t j=from; j<to; j++)
        {
            VType key = edges.get()[j];

            if(Q.size()> 1 && (key < *Q.begin() || key > *Q.rbegin())) continue;

            if(mCntTime[key] == mTime)
            {
                continue;
            }
            else
            {
                mCntTime[key] = mTime;
            }

            if(Q.find(key) != Q.end())
            {
                Qp.insert(key);
            }
        }
    }
    //if(!tmp.empty())
    //{
    //    sort(tmp.begin(), tmp.end());
    //    auto it = Qp.insert(Qp.begin(),tmp[0]);
    //    for(size_t i=1; i<tmp.size(); i++)
    //    {
    //        if(tmp[i] != tmp[i-1])
    //        {
    //            it = Qp.insert(it, tmp[i]); //TODO remove this
    //        }
    //    }
    //}

    mLastRes = true;
    return true;
}

template<typename VType>
void GraphBnB<VType>::MergeRNew(std::set<VType> &P,
                                std::vector<VType> &Lp,
                                std::set<VType> &Rp,
                                std::set<VType> &Pp)
{
    mTime++;
    for(size_t i=0; i<Lp.size(); i++)
    {
        size_t from, to;
        std::shared_ptr<VType> &edges = mG->GetEdges(Lp[i], from, to); 
        for(size_t j=from; j<to; j++)
        {
            VType key = edges.get()[j];
            
            if(P.size()> 1 && (key < *P.begin() || key > *P.rbegin())) continue;

            if(mCntTime[key] == mTime)
                mCnt[key]++;
            else
            { 
                mCntTime[key] = mTime; 
                mCnt[key] = 1;
            }

            if(P.find(key) != P.end())
            {
                if(mCnt[key] == Lp.size())
                {
                    Pp.erase(key);
                    Rp.insert(key);
                }
                else if(mCnt[key]==1)
                {
                    Pp.insert(key);
                }
            }
        }
    }
}

template<typename VType>
void GraphBnB<VType>::InsertResult(const std::vector<VType> &Lp,
                                   const std::set<VType>    &Rp)
{
    if(Lp.size()>0)
    {
        mHot.clear();
        BiClique<VType> bc;
        bc.mToSet = std::vector<VType>(Rp.begin(), Rp.end());
        bc.mFromSet = Lp;
        ret.push_back(bc);
        std::pair<int, int> key(Lp.size(), Rp.size());
        res[key]++;
        mTot++;
        if(ret.size() % mReportInterval ==0) 
        {
            mEnd = omp_get_wtime();
            std::cout<<mTot  << " Examine(Goal):"<<mExamineNow<<"("<<mLastToExamine<<")\t";
            std::cout<<std::fixed  << std::setprecision(2)<<
            " Time:"               << mEnd-mStart<<
            "\tLCheck: "           << mCountLCheck<<
            "\tRevLCheck: "        << mCountRevLCheck<<std::endl;
            mCountLCheck           = 0;
            mCountRevLCheck        = 0;
            mStart                 = omp_get_wtime();

            SaveBicliqueResult();
            
        }
    }
}

template<typename VType>
void GraphBnB<VType>::SaveStatus(std::set<VType>    &P,
                                 std::vector<VType> &L,
                                 std::set<VType>    &R,
                                 std::set<VType>    &Q,
                                 const VType        &x)
{
    // Save current status
    std::ofstream of(mFolder+"/status.txt");
    for(auto i : P)
        of << i << " ";
    of << std::endl;
    for(auto i : L)
        of << i << " ";
    of << std::endl;
    for(auto i : R)
        of << i << " ";
    of << std::endl;
    for(auto i : Q)
        of << i << " ";
    of << x <<std::endl;
    of.close();

    // Save ret info
    std::ofstream of1(mFolder+"/tmpret.txt");
    of1 << mTot << std::endl;
    for(auto r : ret)
    {
        for(size_t i=0; i<r.mFromSet.size(); ++i)
            of1<<r.mFromSet[i]<<" ";
        of1<<std::endl;
        for(size_t i=0; i<r.mToSet.size(); ++i)
            of1<<r.mToSet[i]<<" ";
        of1<<std::endl;
    }
    of1.close();

    // Save res info
    std::ofstream of2(mFolder+"/tmpres.txt");
    for(auto iter = res.begin(); iter != res.end(); ++iter)
    {
        std::string key = std::to_string(iter->first.first)+" "+std::to_string(iter->first.second);
        of2<<key<<" "<<iter->second<<std::endl;
    }
    of2.close();

    // Save assistance info
    std::ofstream of3(mFolder+"/assist.txt");
    of3<<(mLastRes ? "true":"false")<<std::endl;
    for(auto i : mLastLp)
        of3 << i << " ";
    of3 << std::endl;
    for(auto i : mHot)
        of3 << i << " ";
    of3 << std::endl;
}


template<typename VType>
void GraphBnB<VType>::SaveBicliqueResult()
{
    std::string inFilename(mFolder+"/out_"+std::to_string(mTot/mReportInterval)); 
    std::ofstream of(inFilename);
    for(auto r : ret)
    {
        of<<"-----"<<std::endl;
        for(size_t i=0; i<r.mFromSet.size(); ++i)
            of<<r.mFromSet[i]<<" ";
        of<<std::endl;
        for(size_t i=0; i<r.mToSet.size(); ++i)
            of<<r.mToSet[i]<<" ";
        of<<std::endl;
    }
    ret.clear();
    // compress the file
    char* const outFilename = createOutFilename_orDie(inFilename.c_str());
    compress_orDie(inFilename.c_str(), outFilename);
    free(outFilename);
    std::remove(inFilename.c_str());
}

template<typename VType>
void GraphBnB<VType>::InitRaw()
{
    std::vector<std::pair<VType, size_t>> deg;
    for(int i=mG->GetNumV()-1; i>=0; --i)
    {
        size_t from, to;
        mG->GetRevEdges(i, from, to); 
        if((to-from)> 0)
        {
            deg.push_back(std::pair<VType, size_t>(i, (to-from)));
        }
        else
            mL.push_back(i);
    }
    sort(mL.begin(), mL.end());

    for(size_t i=0; i<deg.size(); ++i)
    {
        mP.insert(deg[i].first);
    }
    mLastToExamine = *mP.begin();
}

template<typename VType>
void GraphBnB<VType>::InitRecover()
{
    // Recover current status
    std::ifstream ifs(mFolder+"/status.txt");

    std::string lineP;
    getline(ifs, lineP);
    std::istringstream ssP(lineP);
    while(ssP)
    {
        VType v;
        ssP>>v;
        mP.insert(v);
    }


    std::string lineL;
    getline(ifs, lineL);
    std::istringstream ssL(lineL);
    while(ssL)
    {
        VType v;
        ssL>>v;
        mL.push_back(v);
    }

    std::string lineR;
    getline(ifs, lineR);
    std::istringstream ssR(lineR);
    while(ssR)
    {
        VType v;
        ssR>>v;
        mR.insert(v);
    }

    std::string lineQ;
    getline(ifs, lineQ);
    std::istringstream ssQ(lineQ);
    while(ssQ)
    {
        VType v;
        ssQ>>v;
        mQ.insert(v);
    }

    // Recover ret
    std::ifstream if1(mFolder+"/tmpret.txt");
    std::string lineTot;
    getline(if1, lineTot);
    std::istringstream ssTot(lineTot);
    ssTot>>mTot;

    std::string lineRet;
    int count = 0;
    std::vector<VType> vecFrom;
    std::vector<VType> vecTo;
    while(getline(if1, lineRet))
    {
        std::istringstream ssRet(lineRet);
        while(ssRet)
        {
            VType v; 
            ssRet>>v;
            if(count%2==0)
                vecFrom.push_back(v);
            else
                vecTo.push_back(v);
        }
        if(count % 2 == 1)
        {
            BiClique<VType> bc;
            bc.mFromSet = vecFrom; 
            bc.mToSet = vecTo; 
            ret.push_back(bc);
            vecFrom.clear();
            vecTo.clear();
        }
        count++;
    }

    // Recover res
    std::ifstream if2(mFolder+"/tmpres.txt");
    std::string lineRes;
    while(getline(if2, lineRes))
    {
        std::istringstream ssRes(lineRes);
        std::string k1,k2,val;    
        ssRes>>k1>>k2>>val;
        std::pair<int, int> key(stoi(k1), stoi(k2));
        res[key] = stoi(val);
    }
    mLastToExamine = *mP.begin();

    // Recover assistant data structure
    std::ifstream if3(mFolder+"/assist.txt");
    std::string lineLastRes, lineLastLp, lineHot;
    getline(if3, lineLastRes);
    std::istringstream ssLastRes(lineLastRes);
    std::string trueOrFalse;
    ssLastRes>>trueOrFalse;
    if(trueOrFalse=="true") mLastRes = true;
    else mLastRes = false;

    getline(if3, lineLastLp);
    std::istringstream ssLastLp(lineLastLp);
    while(ssLastLp)
    {
        VType v;
        ssLastLp>>v;
        mLastLp.push_back(v);
    }
    
    getline(if3, lineHot);
    std::istringstream ssHot(lineHot);
    while(ssHot)
    {
        std::string s;
        ssHot>>s;
        mHot.insert(s);
    }
}

} // namespace query
} // namespace graph
} // namespace odps
} // namespace apsara

#endif
