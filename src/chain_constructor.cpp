#include "chain_constructor.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include "util.h"

ChainConstructor::ChainConstructor(const size_t order, std::istream& in) : m_matrix(order), m_order(order) {
    std::vector<size_t> sequence(order, 0);

    std::string current;

    // Insert the token into the map and shift the sequence once to the left,
    // making the last element of the sequence be next token index
    auto push = [&]() {
        const size_t next = m_map.try_insert(std::move(current));
        m_matrix.increment(sequence, next);
        for (size_t i = 0; i < order - 1; i++) {
            sequence[i] = sequence[i + 1];
        }

        sequence[order - 1] = next;

        current.clear();
    };

    // Read char by char and tokenise according to some rules. I chose the once
    // below just because they gave good enough (but not perfect) results for
    // English texts
    char ch;
    while (!in.eof()) {
        in >> std::noskipws >> ch;

        const bool token_empty = current.empty();

        if ((std::isspace(ch) || !std::isprint(ch)) && !token_empty) {
            push();
        } else if (std::isalnum(ch)) {
            current.push_back(ch);
        } else if (!token_empty && std::isalnum(current.back())) {
            current.push_back(ch);
        } else if (std::ispunct(ch)) {
            if (!current.empty()) {
                push();
            }

            current.push_back(ch);
            if (ch != '$') {
                push();
            }
        }
    }
}

void save_chain(const std::string& path, const ChainConstructor& chain) {
    // Maps the first index in a sequence to the first occurrence of a sequence
    // in the .six file that starts with that index in lexicographical order.
    // It also maps that token into the .map file to get the token corresponding
    // to the that index
    std::ofstream token_index(path + ".tix", std::ios::binary);

    // Maps each sequence into the row of frequencies in the .frq file
    std::ofstream sequence_index(path + ".six", std::ios::binary);

    // Contains the translations of indices to words
    std::ofstream token_map(path + ".map", std::ios::binary);

    // Each row starts with a number of entries n in that row then followed by
    // n pairs of values, where the first is the token index and the second is
    // the frequency of that token index
    std::ofstream frequency(path + ".frq", std::ios::binary);

    binary_write(token_index, chain.m_order);
    binary_write(token_index, chain.m_map.m_current_index);

    // Store pointer to all sequences of tokens
    std::vector<const sequence*> sequences;
    sequences.reserve(chain.m_matrix.size());

    for (const auto& [key, _] : chain.m_matrix) {
        sequences.push_back(&key);
    }

    // Sort the sequences in lexicographical order
    std::sort(sequences.begin(), sequences.end(), [](auto x, auto y) {
        return (*x) < (*y);
    });

    // Byte pointers to file. When the return value of binary_write is added
    // to them then it means the pointer is moved forward
    size_t current_token_index = 0;
    size_t map_byte_index = 0;
    size_t sequence_byte_index = 0;
    size_t frequency_byte_index = 0;

    // Here the sequence_byte_index is divided by 8*(order + 1) since every row in the .six file is the same size:
    // length of the sequence which is order + 1 for the byte pointer.

    // Write first token
    binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
    binary_write(token_index, map_byte_index);

    map_byte_index += binary_write(token_map, chain.m_map.m_inverse_map[current_token_index]);

    for (const auto& sequence : sequences) {
        // Switch to next token_index if the sequence has a different token index than the previous
        const size_t first_in_sequence = (*sequence)[0];
        if (first_in_sequence != current_token_index) {
            current_token_index = first_in_sequence;
            binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
            binary_write(token_index, map_byte_index);
            map_byte_index += binary_write(token_map, chain.m_map.m_inverse_map[first_in_sequence]);
        }

        const auto& frequencies = chain.m_matrix.at(*sequence);

        // Write token indices of sequence followed by byte pointer to frequency matrix
        for (const auto& token_index : *sequence) {
            sequence_byte_index += binary_write(sequence_index, token_index);
        }
        sequence_byte_index += binary_write(sequence_index, frequency_byte_index);

        // Write number of entries n followed by n pairs of entries corresponding to
        // index followed by frequency
        frequency_byte_index += binary_write(frequency, frequencies.size());
        for (const auto& [token_index, token_frequency] : frequencies) {
            frequency_byte_index += binary_write(frequency, token_index);
            frequency_byte_index += binary_write(frequency, token_frequency);
        }
    }
    // Write upper boundary indices into token_index file to allow for binary search
    // during text generation
    binary_write(token_index, sequence_byte_index / (8 * (chain.m_order + 1)));
    binary_write(token_index, map_byte_index);
}