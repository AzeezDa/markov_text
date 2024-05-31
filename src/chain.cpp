#include "chain.h"
#include <iostream>

Chain::Chain(const size_t order, std::istream& in) : m_matrix(order) {
    std::vector<size_t> sequence(order, 0);

    std::string current;

    auto push = [&]() {
        const size_t next = m_map.try_insert(current);
        m_matrix.increment(sequence, next);
        for (size_t i = 0; i < order - 1; i++) {
            sequence[i] = sequence[i + 1];
        }

        sequence[order - 1] = next;

        current.clear();
    };

    char ch;
    while (!in.eof()) {
        in >> std::noskipws >> ch;

        if (std::isalpha(ch)) {
            current.push_back(ch);
        } else {
            if (!current.empty()) {
                push();
            }

            if (!std::isspace(ch)) {
                current.push_back(ch);
                push();
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Chain& chain) {
    os << chain.m_map << '\n'
       << chain.m_matrix;
    return os;
}