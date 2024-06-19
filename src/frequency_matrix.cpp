#include "frequency_matrix.hpp"

FrequencyMatrix::FrequencyMatrix(const std::size_t order) : m_order(order) {}

void FrequencyMatrix::increment(const sequence& sequence, const std::size_t& next) {
    // Find row corresponding to sequence. If it does not exist, add it and
    // add an entry for the next token index with frequency set to 1. Otherwise,
    // increment (or add with 1) the frequency of the next token index in that row
    auto row_frequencies = find(sequence);
    if (row_frequencies == end()) {
        (*this)[sequence][next] = 1;
    } else {
        auto& row = row_frequencies->second;
        auto column = row.find(next);
        if (column == row.end()) {
            row.insert({next, 1});
        } else {
            column->second += 1;
        }
    }
}