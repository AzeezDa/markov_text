#include "chain_constructor.hpp"
#include <algorithm>
#include <fstream>
#include <ostream>
#include <ranges>
#include <string>
#include "binary_io.hpp"

namespace details {

std::size_t as_row_index(const std::size_t index, const std::size_t order) {
    const std::size_t BITS_PER_BYTES = 8;
    return index / (BITS_PER_BYTES * (order + 1));
};

}  // namespace details

ChainConstructor::ChainConstructor(const std::size_t order) : m_matrix(order), m_order(order) {}

void ChainConstructor::save_chain(const std::filesystem::path& path) {
    std::ofstream token_index(path.string() + TOKEN_INDEX_FILE_EXTENSION, std::ios::binary);
    std::ofstream sequence_index(path.string() + SEQUENCE_INDEX_FILE_EXTENSION, std::ios::binary);
    std::ofstream token_map(path.string() + TOKEN_MAP_FILE_EXTENSION, std::ios::binary);
    std::ofstream frequency(path.string() + FREQUENCY_FILE_EXTENSION, std::ios::binary);

    save_chain(token_index, sequence_index, token_map, frequency);
}

void ChainConstructor::save_chain(std::ostream& token_index,
                                  std::ostream& sequence_index,
                                  std::ostream& token_map,
                                  std::ostream& frequency) {
    // Setup sequences
    std::vector<const sequence*> sequences;
    sequences.reserve(m_matrix.size());

    for (const auto& [key, _] : m_matrix) {
        sequences.push_back(&key);
    }

    std::ranges::sort(sequences, [](auto x, auto y) {
        return *x < *y;
    });

    // Byte pointers to files. When the return value of binary_write is added
    // to them then it means the pointer is moved forward
    std::size_t current_token_index = 0;
    std::size_t map_byte_index = 0;
    std::size_t sequence_byte_index = 0;
    std::size_t frequency_byte_index = 0;

    binary_write(token_index, m_order);
    binary_write(token_index, m_map.m_current_index);

    binary_write(token_index, details::as_row_index(sequence_byte_index, m_order));
    binary_write(token_index, map_byte_index);

    map_byte_index += binary_write(token_map, m_map.m_inverse_map[current_token_index]);

    for (const auto& sequence : sequences) {
        // Write to Token Index
        const std::size_t first_in_sequence = sequence->front();
        if (first_in_sequence != current_token_index) {
            current_token_index = first_in_sequence;
            binary_write(token_index, details::as_row_index(sequence_byte_index, m_order));
            binary_write(token_index, map_byte_index);
            map_byte_index += binary_write(token_map, m_map.m_inverse_map[first_in_sequence]);
        }

        // Write to Sequence Index
        for (const auto& token_index : *sequence) {
            sequence_byte_index += binary_write(sequence_index, token_index);
        }
        sequence_byte_index += binary_write(sequence_index, frequency_byte_index);

        // Write to Frequencies
        const auto& frequencies = m_matrix.at(*sequence);
        frequency_byte_index += binary_write(frequency, frequencies.size());
        for (const auto& [token_index, token_frequency] : frequencies) {
            frequency_byte_index += binary_write(frequency, token_index);
            frequency_byte_index += binary_write(frequency, token_frequency);
        }
    }

    // Write upper boundary indices into token_index file to allow for binary search
    // during text generation
    binary_write(token_index, details::as_row_index(sequence_byte_index, m_order));
    binary_write(token_index, map_byte_index);
}