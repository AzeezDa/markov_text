#include "chain.hpp"
#include <algorithm>
#include <fstream>
#include <istream>
#include <vector>
#include "binary_io.hpp"
#include "sequence.hpp"
#include "token.hpp"

void Chain::save(const std::filesystem::path& path) const {
    std::ofstream token_table(path.string() + FILE_EXTENSION_TOKEN_TABLE, std::ios::binary);
    std::ofstream token_map(path.string() + FILE_EXTENSION_TOKEN_MAP, std::ios::binary);
    std::ofstream sequence_map(path.string() + FILE_EXTENSION_SEQUENCE_MAP, std::ios::binary);
    std::ofstream frequency_table(path.string() + FILE_EXTENSION_FREQUENCY_TABLE, std::ios::binary);
    save(token_table, token_map, sequence_map, frequency_table);
}

void Chain::save(std::ostream& token_table, std::ostream& token_map, std::ostream& sequence_map, std::ostream& frequency_table) const {
    binary_write(token_table, m_order, m_map.size(), m_table.size());

    // Set up sequences
    std::vector<const Sequence*> sequences;
    sequences.reserve(m_table.size());
    for (const auto& [sequence, _] : m_table) {
        sequences.push_back(&sequence);
    }
    std::sort(sequences.begin(), sequences.end(), [](auto x, auto y) {
        return *x < *y;
    });

    std::size_t token_index = -1;  // Defined behaviour according to standard
    std::size_t token_map_pointer = 0;
    std::size_t sequence_map_pointer = 0;
    std::size_t frequency_table_pointer = 0;

    for (const auto& sequence : sequences) {
        // Update token_table and token_map when a new token start is encountered
        if (sequence->front() != token_index) {
            token_index += 1;
            binary_write(token_table, token_map_pointer, sequence_map_pointer);
            token_map_pointer += binary_write(token_map, m_map.get_token(token_index));
        }

        // Write sequence
        for (const auto& index : *sequence) {
            sequence_map_pointer += binary_write(sequence_map, index);
        }
        sequence_map_pointer += binary_write(sequence_map, frequency_table_pointer);

        // Write frequencies for that sequence
        const auto& row = m_table.get_row(*sequence);
        frequency_table_pointer += binary_write(frequency_table, row.size());
        for (const auto& [next_token_index, frequency] : row) {
            frequency_table_pointer += binary_write(frequency_table, next_token_index, frequency);
        }
    }

    // Write last token that do not start a sequence (if any)
    for (token_index += 1; token_index < m_map.size(); token_index++) {
        binary_write(token_table, token_map_pointer, NO_SEQUENCE);
        token_map_pointer += binary_write(token_map, m_map.get_token(token_index));
    }
}