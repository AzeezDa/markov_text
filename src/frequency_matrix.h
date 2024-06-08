#pragma once

#include <unordered_map>
#include <vector>

using sequence = std::vector<size_t>;
using frequency_row = std::unordered_map<size_t, size_t>;
using frequency_matrix = std::unordered_map<sequence, frequency_row>;

template <>
struct std::hash<sequence> {
    size_t operator()(const sequence& sequence) const noexcept {
        size_t hash = 0;
        for (const auto& n : sequence) {
            hash ^= std::hash<size_t>{}(n);
            hash <<= 1;
        }
        return hash;
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