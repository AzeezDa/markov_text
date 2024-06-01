#include "chain.h"
#include <iostream>
#include <random>

Chain::Chain(const size_t order, std::istream& in) : m_matrix(order), m_order(order) {
    std::vector<size_t> sequence(order, 0);

    std::string current;

    auto push = [&]() {
        const size_t next = m_map.try_insert(std::move(current));
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

void Chain::generate(const size_t word_count, std::ostream& out) {
    std::random_device rd;
    std::mt19937 gen(rd());

    sequence sequence = m_matrix.get_random_sequence();

    out << m_map.get_token_at(sequence[0]);
    for (size_t i = 1; i < m_order; ++i) {
        const auto& next_token = m_map.get_token_at(sequence[i]);

        if (!std::ispunct(next_token[0])) {
            out << ' ';
        }
        out << next_token;
    }

    for (size_t i = 0; i < word_count - m_order; ++i) {
        const auto& nexts = m_matrix[sequence];

        std::vector<double> weights;
        std::vector<size_t> map;

        for (const auto& [next, weight] : nexts) {
            weights.push_back(weight);
            map.push_back(next);
        }

        std::discrete_distribution<size_t> distribution(weights.begin(), weights.end());
        const size_t next = map[distribution(gen)];
        const auto& next_token = m_map.get_token_at(next);

        if (!std::ispunct(next_token[0])) {
            out << ' ';
        }
        out << next_token;

        for (size_t i = 0; i < m_order - 1; i++) {
            sequence[i] = sequence[i + 1];
        }
        sequence[m_order - 1] = next;
    }
}

std::ostream& operator<<(std::ostream& os, const Chain& chain) {
    os << chain.m_map << '\n'
       << chain.m_matrix;
    return os;
}