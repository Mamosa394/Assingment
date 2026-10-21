#ifndef HASHSTRATEGY_H
#define HASHSTRATEGY_H

#include <string>
#include <cmath>
#include <climits>

// Pluggable hash function that takes a key and a capacity (M) to produce an index.
enum class HashFunction {
    Division,       // h = key % M
    Multiplicative, // h = floor(M * frac(key * 0.6180339887))
    MidSquare,      // square the key, take middle digits, mod M
    Universal,      // h = ((a*key + b) mod p) mod M
    CustomUniversal // same as Universal but A, B, P come from the user
};

struct HashStrategy {
    HashFunction fn = HashFunction::Division;
    int a = 3;
    int b = 5;
    int p = 1000003; // prime > any realistic key

    // Core: turn a key into [0, capacity)
    int indexFor(int key, int capacity) const {
        long long k = key;
        if (capacity <= 0) return 0;
        long long m = capacity;

        switch (fn) {
        case HashFunction::Division: {
            long long r = k % m;
            if (r < 0) r += m;
            return static_cast<int>(r);
        }
        case HashFunction::Multiplicative: {
            const double A = 0.6180339887498949; // 1/phi
            double frac = std::fmod(std::fabs(static_cast<double>(k)) * A, 1.0);
            int idx = static_cast<int>(std::floor(m * frac));
            if (idx < 0) idx += static_cast<int>(m);
            return idx;
        }
        case HashFunction::MidSquare: {
            long long sq = k * k;
            if (sq < 0) sq = -sq;
            int digits = 1;
            long long tmp = m;
            while (tmp >= 10) { tmp /= 10; ++digits; }
            std::string s = std::to_string(sq);
            if (static_cast<int>(s.size()) < digits) {
                return static_cast<int>(sq % m);
            }
            size_t start = (s.size() - digits) / 2;
            std::string mid = s.substr(start, digits);
            return static_cast<int>(std::stoll(mid) % m);
        }
        case HashFunction::Universal:
        case HashFunction::CustomUniversal: {
            int pp = (p > 0) ? p : 1000003;
            long long v = (static_cast<long long>(a) * k + b) % pp;
            if (v < 0) v += pp;
            return static_cast<int>(v % m);
        }
        }
        return 0;
    }
};

#endif