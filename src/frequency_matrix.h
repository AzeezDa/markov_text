#pragma once

#include <unordered_map>
#include <vector>

using sequence = std::vector<size_t>;
using frequency_row = std::unordered_map<size_t, size_t>;
using frequency_matrix = std::unordered_map<sequence, frequency_row>;

// Based on the answer by see in: https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
template <>
struct std::hash<sequence> {
    size_t operator()(const sequence& sequence) const noexcept {
        size_t seed = sequence.size();
        for (auto n : sequence) {
            n = ((n >> 16) ^ n) * 0x45d9f3b;
            n = ((n >> 16) ^ n) * 0x45d9f3b;
            n = (n >> 16) ^ n;
            seed ^= n + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

struct ChainConstructor;

struct FrequencyMatrix : private frequency_matrix {
    FrequencyMatrix(const size_t order);

    using frequency_matrix::operator[];
    using frequency_matrix::at;

    void increment(const sequence& sequence, const size_t& next);
    sequence get_random_sequence() const;

    friend void save_chain(const std::string&, const ChainConstructor&);

private:
    const size_t m_order;
};