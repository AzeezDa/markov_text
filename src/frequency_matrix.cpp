#include "frequency_matrix.h"
#include <iostream>
#include <random>

FrequencyMatrix::FrequencyMatrix(const size_t order) : frequency_matrix({}), m_order(order) {}

void FrequencyMatrix::increment(const sequence& sequence, const size_t& next) {
    auto row_kvp = find(sequence);
    if (row_kvp == end()) {
        (*this)[sequence][next] = 1;
    } else {
        auto& row = row_kvp->second;
        auto column = row.find(next);
        if (column == row.end()) {
            row.insert({next, 1});
        } else {
            column->second += 1;
        }
    }
}

sequence FrequencyMatrix::get_random_sequence() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand(0, size() - 1);

    auto start = begin();
    start = std::next(start, rand(gen));
    return start->first;
}