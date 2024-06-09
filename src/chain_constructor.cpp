#include "chain_constructor.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include "util.h"

ChainConstructor::ChainConstructor(const size_t order, std::istream& in) : m_matrix(order), m_order(order) {
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

void save_chain(const std::string& path, const ChainConstructor& chain) {
    std::ofstream token_index(path + ".tix", std::ios::binary);
    std::ofstream sequence_index(path + ".six", std::ios::binary);
    std::ofstream token_map(path + ".map", std::ios::binary);
    std::ofstream frequency(path + ".frq", std::ios::binary);

    binary_write(token_index, chain.m_order);
    binary_write(token_index, chain.m_map.m_current_index);

    std::vector<const sequence*> sequences;
    sequences.reserve(chain.m_matrix.size());

    for (const auto& [key, _] : chain.m_matrix) {
        sequences.push_back(&key);
    }

    std::sort(sequences.begin(), sequences.end(), [](auto x, auto y){
       return (*x) < (*y);
    });

    size_t current_token_index = 0;
    size_t map_byte_index = 0;
    size_t sequence_byte_index = 0;
    size_t frequency_byte_index = 0;

    binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
    binary_write(token_index, map_byte_index);

    map_byte_index += binary_write(token_map, chain.m_map.m_inverse_map[current_token_index]);

    for (const auto& sequence : sequences) {
        const size_t first_in_sequence = (*sequence)[0];
        if (first_in_sequence != current_token_index) {
            current_token_index = first_in_sequence;
            binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
            binary_write(token_index, map_byte_index);
            map_byte_index += binary_write(token_map, chain.m_map.m_inverse_map[first_in_sequence]);
        }

        const auto& frequencies = chain.m_matrix.at(*sequence);

        for (const auto& token_index : *sequence) {
            sequence_byte_index += binary_write(sequence_index, token_index);
        }
        sequence_byte_index += binary_write(sequence_index, frequency_byte_index);

        frequency_byte_index += binary_write(frequency, frequencies.size());
        for (const auto& [token_index, token_frequency] : frequencies) {
            frequency_byte_index += binary_write(frequency, token_index);
            frequency_byte_index += binary_write(frequency, token_frequency);
        }
    }
    binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
    binary_write(token_index, map_byte_index);
}