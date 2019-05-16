/*
 * Copyright (c) 2016 drali. All rights reserved.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
 *  2. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  3. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 */
#pragma once

#include <cstring>  // for memcpy
#include <ostream>
#include <omp.h>

class BitSet {
    public:
        // Empty bit set
        BitSet() = default;

        void SetNumThreads(int num) {numthreads = num;}

        // Allocated, but no bits set
        BitSet(const size_t size) {
            numthreads = 20;
            allocated_ = (size >> 3) + 1;
            bits_ = new unsigned char[allocated_];
            size_ = size;
            size_t i;
#pragma omp parallel for num_threads(numthreads) private(i)
            for(i=0; i<allocated_; i++)
                bits_[i] = 0;
        }

        unsigned char * Get()
        {
            return bits_;
        }

        ~BitSet() {
            if (bits_ != nullptr) {
                delete [] bits_;
                bits_ = nullptr;
            }
        }

        BitSet(const BitSet& rhs) : allocated_(rhs.allocated_), size_(rhs.size_) {
            if (allocated_ != 0) {
                bits_ = new unsigned char[allocated_];
                memcpy(bits_, rhs.bits_, (size_ >> 3) + 1);
            }
        }

        BitSet& operator=(const BitSet& rhs) {
            if (this != &rhs) {
                allocated_ = rhs.allocated_;
                size_ = rhs.size_;
                bits_ = new unsigned char[allocated_];
                memcpy(bits_, rhs.bits_, (size_ >> 3) + 1);
            }
            return *this;
        }

        BitSet(BitSet&& rhs) {
            if (this == &rhs) { return; }
            allocated_ = rhs.allocated_;
            bits_ = rhs.bits_;
            size_ = rhs.size_;
            rhs.allocated_ = 0;
            rhs.bits_ = nullptr;
            rhs.size_ = 0;
        }

        BitSet& operator=(BitSet&& rhs) {
            if (this == &rhs) { return  *this; }
            allocated_ = rhs.allocated_;
            bits_ = rhs.bits_;
            size_ = rhs.size_;
            rhs.allocated_ = 0;
            rhs.bits_ = nullptr;
            rhs.size_ = 0;
            return *this;
        }

        void Set(const size_t position) {
            #pragma omp atomic
            bits_[position >> 3] |= (1 << (position & 0x7));
        }

        void UnSet(const size_t position) {
            //if(bits_[position >> 3])
            {
#pragma omp atomic
                bits_[position >> 3] &= (0 << (position & 0x7));
            }
        }

        void ReSet() {
            size_t i;
#pragma omp parallel for num_threads(numthreads) private(i)
            for(i=0; i<allocated_; i++)
                bits_[i] = 0;
        }

        void Reset() {
            size_t i;
            for(i=0; i<allocated_; i++)
                bits_[i] = 0;
        }

        bool Test(const size_t i)
        {
            return bits_[i] != 0;
        }

        bool Any() {
            bool ret = false;
            size_t i;
#pragma omp parallel for num_threads(numthreads) private(i)
            for(i=0; i<allocated_; i++)
                if (bits_[i]) ret = true;
            return ret;
        }

        // Returns number of bits
        size_t GetSize() const {
            return size_;
        }

        size_t GetBuckets() const {
            return allocated_;
        }

        size_t Count() const {
            size_t ret = 0;
            size_t i;
#pragma omp parallel for num_threads(numthreads) private(i) reduction(+:ret)
            for(i=0; i<size_; i++)
                ret += (IsSet(i) ? 1 : 0);
            return ret;
        }

        bool IsSet(const size_t position) const {
            return (bits_[position >> 3] & (1 << (position & 0x7))) != 0;
        }

        friend std::ostream& operator<<(std::ostream& out, const BitSet& bit_set) {
            for (size_t i = 0; i < bit_set.size_; ++i) {
                out << bit_set.IsSet(i);
            }
            out << std::endl;
            return out;
        }

    private:
        unsigned char* bits_ = nullptr;

        // Bytes allocated
        size_t allocated_ = 0;

        // Number of bits
        size_t size_ = 0;
        int numthreads;
};
