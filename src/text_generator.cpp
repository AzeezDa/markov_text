#include "text_generator.hpp"
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include "binary_io.hpp"
#include "frequency_matrix.hpp"

TextGenerator::TextGenerator(const std::filesystem::path& path)
    : m_sequence_index(path.string() + ".six", std::ios::binary),
      m_token_map(path.string() + ".map", std::ios::binary),
      m_frequency(path.string() + ".frq", std::ios::binary) {
    std::ifstream token_index(path.string() + ".tix", std::ios::binary);

    binary_read(token_index, m_order);
    std::size_t token_count;
    binary_read(token_index, token_count);
    m_token_byte_pointers.reserve(token_count);

    for (std::size_t i = 0; i < token_count + 1; i++) {
        TokenBytePointer token_byte_pointer;
        binary_read(token_index, token_byte_pointer.sequence_index);
        binary_read(token_index, token_byte_pointer.map_index);
        m_token_byte_pointers.push_back(token_byte_pointer);
    }
}

template <TokenPrinterLike print>
void TextGenerator::generate(const std::size_t output_token_count) {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Get first random sequence at random by first getting a random token from the token_index
    // Then pick a sequence that starts with that token by randomly indexing into the .six file
    const std::size_t token_count = m_token_byte_pointers.size();
    const std::size_t random_token_index = std::uniform_int_distribution<std::size_t>(0, token_count)(gen);
    const std::size_t sequence_index_start = m_token_byte_pointers[random_token_index].sequence_index;
    const std::size_t sequence_index_end = m_token_byte_pointers[random_token_index + 1].sequence_index;
    const std::size_t random_sequence_index = std::uniform_int_distribution<std::size_t>(sequence_index_start, sequence_index_end - 1)(gen);

    sequence current_sequence;

    char previous = ' ';

    // Write tokens in the random sequence
    m_sequence_index.seekg(random_sequence_index * 8 * (m_order + 1));
    for (std::size_t i = 0; i < m_order; i++) {
        std::size_t token_index;
        binary_read(m_sequence_index, token_index);
        current_sequence.push_back(token_index);

        const std::size_t map_index = m_token_byte_pointers[token_index].map_index;
        m_token_map.seekg(map_index);

        std::string token;
        binary_read(m_token_map, token);

        print{}(std::cout, token, previous);

        previous = token[0];
    }

    // Generate the next token in each iteration by sampling a random next token based
    // on the frequencies for each sequence. Then shift the sequence once to the left,
    // adding the next token in last position of the sequence
    for (std::size_t i = 0; i < output_token_count - m_order; i++) {
        // Get indices of first and last sequences that start with the first token index
        // of the sequence
        const std::size_t token_index = current_sequence[0];
        std::size_t sequence_index_start = m_token_byte_pointers[token_index].sequence_index;
        std::size_t sequence_index_end = m_token_byte_pointers[token_index + 1].sequence_index;

        // Binary search to find the sequence row in the .six file
        while (sequence_index_start < sequence_index_end) {
            const std::size_t mid = sequence_index_start + (sequence_index_end - sequence_index_start) / 2;

            m_sequence_index.seekg(mid * 8 * (m_order + 1));

            sequence mid_sequence;

            for (std::size_t i = 0; i < m_order; i++) {
                std::size_t token_index;
                binary_read(m_sequence_index, token_index);
                mid_sequence.push_back(token_index);
            }

            if (current_sequence < mid_sequence) {
                sequence_index_end = mid;
            } else if (current_sequence > mid_sequence) {
                sequence_index_start = mid + 1;
            } else {
                sequence_index_start = mid;
                break;
            }
        }

        // Read the frequency byte pointer and go to the frequency row
        // in the .frq file
        std::size_t frequency_index;
        binary_read(m_sequence_index, frequency_index);
        m_frequency.seekg(frequency_index);

        // Read the number of entries in that frequency row
        std::size_t frequency_count;
        binary_read(m_frequency, frequency_count);

        std::vector<double> weights;
        weights.reserve(frequency_count);
        std::vector<std::size_t> map;
        map.reserve(frequency_count);

        // Read the indices and frequencies
        for (std::size_t i = 0; i < frequency_count; i++) {
            std::size_t token_index;
            binary_read(m_frequency, token_index);
            map.push_back(token_index);

            std::size_t weight;
            binary_read(m_frequency, weight);
            weights.push_back(weight);
        }

        // Sample next token index
        std::discrete_distribution<std::size_t> distribution(weights.begin(), weights.end());
        const std::size_t next = map[distribution(gen)];

        // Read the token as a string corresponding to it by indexing into
        // the .map file
        const std::size_t map_index = m_token_byte_pointers[next].map_index;
        m_token_map.seekg(map_index);
        std::string token;
        binary_read(m_token_map, token);

        print{}(std::cout, token, previous);

        previous = token[0];

        // Shift the sequence to the left and add the next token index
        // as the last element in the sequence
        for (std::size_t i = 0; i < m_order - 1; i++) {
            current_sequence[i] = current_sequence[i + 1];
        }
        current_sequence[m_order - 1] = next;
    }
}