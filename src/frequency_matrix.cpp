#include "frequency_matrix.h"
#include <iostream>

FrequencyMatrix::FrequencyMatrix(const size_t order) : m_order(order), frequency_matrix({}) {}

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

std::ostream& operator<<(std::ostream& os, const FrequencyMatrix& matrix) {
    for (const auto& [sequence, row] : matrix) {
        os << '{' << sequence[0];
        for (size_t i = 1; i < matrix.m_order; i++) {
            os << ' ' << sequence[i];
        }
        os << '}';
        for (const auto& [next, frequency] : row) {
            os << " (" << next << ' ' << frequency << ')';
        }
        os << '\n';
    }
    return os;
}
