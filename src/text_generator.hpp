#ifndef H_TEXT_GENERATOR
#define H_TEXT_GENERATOR

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <random>
#include <vector>
#include <string>
#include "chain_constructor.hpp"
#include "token.hpp"

template <TokenPrinterLike T>
void generate(const std::size_t, std::istream&, std::istream&, std::istream&, std::istream&, std::ostream&);

namespace details {

std::random_device rd;
std::mt19937 gen(rd());

struct TokenIndexPointer {
    std::size_t sequence_index;
    std::size_t map_index;
};

struct TokenIndexOutput {
    std::size_t chain_order;
    std::vector<TokenIndexPointer> token_pointers;
};

TokenIndexOutput read_token_index(std::istream& token_index) {
    TokenIndexOutput output;

    output.chain_order = binary_read<std::size_t>(token_index);
    const auto token_count = binary_read<std::size_t>(token_index);

    output.token_pointers.reserve(token_count);

    for (std::size_t i = 0; i < token_count + 1; i++) {
        const auto pointer = binary_read<TokenIndexPointer>(token_index);
        output.token_pointers.push_back(pointer);
    }

    return output;
}

std::size_t get_random_sequence_index(const std::vector<TokenIndexPointer>& pointers) {
    const std::size_t token_count = pointers.size();
    const std::size_t random_token_index = std::uniform_int_distribution<std::size_t>(0, token_count)(gen);
    const std::size_t sequence_index_start = pointers[random_token_index].sequence_index;
    const std::size_t sequence_index_end = pointers[random_token_index + 1].sequence_index;
    const std::size_t random_sequence_index = std::uniform_int_distribution<std::size_t>(sequence_index_start, sequence_index_end - 1)(gen);

    return random_sequence_index;
}

std::size_t from_row_index(const std::size_t index, const std::size_t order) {
    const std::size_t BITS_PER_BYTES = 8;
    return index * BITS_PER_BYTES * (order + 1);
}

std::size_t sample_next_token(const sequence& current_sequence,
                              const std::vector<TokenIndexPointer>& token_byte_pointers,
                              std::istream& sequence_index,
                              std::istream& frequency,
                              std::size_t order) {
    const std::size_t first_token_in_sequence = current_sequence.front();

    std::size_t sequence_index_start = token_byte_pointers[first_token_in_sequence].sequence_index;
    std::size_t sequence_index_end = token_byte_pointers[first_token_in_sequence + 1].sequence_index;

    while (sequence_index_start < sequence_index_end) {
        const std::size_t mid = sequence_index_start + (sequence_index_end - sequence_index_start) / 2;

        sequence_index.seekg(details::from_row_index(mid, order));

        sequence mid_sequence;

        for (std::size_t i = 0; i < order; i++) {
            const auto token_index = binary_read<std::size_t>(sequence_index);
            mid_sequence.push_back(token_index);
        }

        if (current_sequence < mid_sequence) {
            sequence_index_end = mid;
        } else if (current_sequence > mid_sequence) {
            sequence_index_start = mid + 1;
        } else {
            break;
        }
    }

    // Read the frequency byte pointer and go to the frequency row
    // in the .frq file
    const auto frequency_index = binary_read<std::size_t>(sequence_index);
    frequency.seekg(frequency_index);

    // Read the number of entries in that frequency row
    const auto frequency_count = binary_read<std::size_t>(frequency);

    // Read the row
    std::vector<double> weights;
    weights.reserve(frequency_count);
    std::vector<std::size_t> map;
    map.reserve(frequency_count);

    for (std::size_t i = 0; i < frequency_count; i++) {
        const auto token_index = binary_read<std::size_t>(frequency);
        map.push_back(token_index);

        const auto weight = binary_read<std::size_t>(frequency);
        weights.push_back(weight);
    }

    // Sample next token index
    std::discrete_distribution<std::size_t> distribution(weights.begin(), weights.end());
    const std::size_t next = map[distribution(gen)];

    return next;
}

std::string index_to_string(const std::size_t index, const std::vector<TokenIndexPointer>& token_byte_pointers, std::istream& token_map) {
    // Read the token as a string corresponding to it by indexing into
    // the .map file
    const std::size_t map_index = token_byte_pointers[index].map_index;
    token_map.seekg(map_index);
    const auto token = binary_read<std::string>(token_map);

    return token;
}

}  // namespace details

template <TokenPrinterLike T = ASCIITokenPrinter>
void generate(const std::size_t output_token_count, const std::filesystem::path& path, std::ostream& out) {
    std::ifstream token_index(path.string() + TOKEN_INDEX_FILE_EXTENSION, std::ios::binary);
    std::ifstream sequence_index(path.string() + SEQUENCE_INDEX_FILE_EXTENSION, std::ios::binary);
    std::ifstream token_map(path.string() + TOKEN_MAP_FILE_EXTENSION, std::ios::binary);
    std::ifstream frequency(path.string() + FREQUENCY_FILE_EXTENSION, std::ios::binary);

    generate<T>(output_token_count, token_index, sequence_index, token_map, frequency, out);
}

template <TokenPrinterLike printer>
void generate(const std::size_t output_token_count,
              std::istream& token_index,
              std::istream& sequence_index,
              std::istream& token_map,
              std::istream& frequency,
              std::ostream& out) {
    // Generate token_index
    const auto [order, token_byte_pointers] = details::read_token_index(token_index);

    // Get random sequence
    const auto random_sequence_index = details::get_random_sequence_index(token_byte_pointers);

    // Write random sequence
    sequence current_sequence;
    char previous = ' ';
    std::size_t current_token = 0;

    sequence_index.seekg(details::from_row_index(random_sequence_index, order));
    for (; current_token < order; current_token++) {
        const auto token_index = binary_read<std::size_t>(sequence_index);
        current_sequence.push_back(token_index);

        const std::size_t map_index = token_byte_pointers[token_index].map_index;
        token_map.seekg(map_index);

        const auto token = binary_read<std::string>(token_map);

        printer{}(out, token, previous);
        previous = token[0];
    }

    // Write rest of sequence
    for (; current_token < output_token_count; current_token++) {
        const auto next = details::sample_next_token(current_sequence, token_byte_pointers, sequence_index, frequency, order);
        const auto token = details::index_to_string(next, token_byte_pointers, token_map);

        printer{}(out, token, previous);
        previous = token[0];

        std::shift_left(current_sequence.begin(), current_sequence.end(), 1);
        current_sequence.back() = next;
    }
}

#endif  // H_TEXT_GENERATOR