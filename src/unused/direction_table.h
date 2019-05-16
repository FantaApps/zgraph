#ifndef _ODPS_DIRECTION_TABLE_
#define _ODPS_DIRECTION_TABLE_

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <cstddef>
#include <iomanip>
#include <cmath>
#include "bitset.h"

namespace apsara {
namespace odps {
namespace graph {
namespace query {

class DirectionTable 
{
public: 
    DirectionTable() {}
    DirectionTable(long one, long two, const std::string &file)
    {
        for(long i=0; i<two; i++)
        {
            BitSet t(one);
            for(long j=0; j<one; j++)
            {
                double dir = j * log2(i);
                double rev = i * log2(j);
                if(dir > rev && j>1)
                {
                    t.Set(j);
                }
            }
            mTable.emplace_back(t);
        }
    }

    bool UseForward(const size_t &range, const size_t &lsize)
    {
        return mTable[lsize].IsSet(range);
        //if(range> 10000 || (range>10 && lsize<=5))
        //    return true;
        //return false;
    }
    DirectionTable(const std::string &file) {}
    virtual ~DirectionTable() {}

private:
    std::vector<BitSet> mTable; 
};

} // namespace query
} // namespace graph
} // namespace odps
} // namespace apsara

#endif
