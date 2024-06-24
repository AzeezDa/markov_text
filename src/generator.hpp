#ifndef H_GENERATOR
#define H_GENERATOR

#include <array>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <random>
#include <utility>
#include "binary_io.hpp"
#include "chain.hpp"
#include "sequence.hpp"
#include "token.hpp"

namespace details {

struct TokenTableRow {
    std::size_t token_map_pointer;
    std::size_t sequence_map_pointer;
};

}  // namespace details

class TextGenerator {
public:
    TextGenerator() : m_gen(std::random_device()()) {}

    template <PrinterLike printer = DefaultPrinter>
    void generate(const std::filesystem::path& path, std::ostream& out, const std::size_t output_token_count);
    template <PrinterLike printer = DefaultPrinter>
    void generate(std::istream& token_table,
                  std::istream& token_map,
                  std::istream& sequence_map,
                  std::istream& frequency_table,
                  std::ostream& out,
                  std::size_t output_token_count);

private:
    struct TokenTableRow {
        std::size_t token_map_pointer;
        std::size_t sequence_map_pointer;
    };
    std::size_t m_order;
    std::size_t m_token_count;
    std::size_t m_sequence_count;
    std::vector<TokenTableRow> m_token_table_rows;
    std::mt19937 m_gen;

private:
    std::size_t byte_to_row(const std::size_t byte_pointer) {
        return (byte_pointer / (sizeof(std::size_t) * (m_order + 1)));
    }

    std::size_t row_to_byte(const std::size_t row_pointer) {
        return (row_pointer * (sizeof(std::size_t) * (m_order + 1)));
    }

    std::size_t get_random_sequence_row() {
        return std::uniform_int_distribution<std::size_t>(0, m_sequence_count - 1)(m_gen);
    }

    std::string get_token(std::size_t token_index, std::istream& token_map) {
        const auto map_byte_pointer = m_token_table_rows[token_index].token_map_pointer;
        token_map.seekg(map_byte_pointer);
        const auto token = binary_read<std::string>(token_map);
        return token;
    }

    std::pair<std::size_t, std::size_t> get_binary_search_bounds(const std::size_t index) {
        const std::size_t low = byte_to_row(m_token_table_rows[index].sequence_map_pointer);
        if (index == m_token_table_rows.size() - 1 ||
            m_token_table_rows[index + 1].sequence_map_pointer == NO_SEQUENCE) {
            return std::make_pair(low, m_sequence_count);
        } else {
            return std::make_pair(low, byte_to_row(m_token_table_rows[index + 1].sequence_map_pointer));
        }
    }

    void read_token_table(std::istream& token_table) {
        // Read Meta Data
        m_order = binary_read<std::size_t>(token_table);
        m_token_count = binary_read<std::size_t>(token_table);
        m_sequence_count = binary_read<std::size_t>(token_table);

        // Read Token Table
        m_token_table_rows.reserve(m_token_count);
        for (size_t i = 0; i < m_token_count; i++) {
            const auto row = binary_read<TokenTableRow>(token_table);
            m_token_table_rows.push_back(row);
        }
    }

    std::size_t get_frequency_table_pointer_for(const Sequence& current_sequence, std::istream& sequence_map) {
        const auto first_token_index = current_sequence.front();
        auto [low, high] = get_binary_search_bounds(first_token_index);

        // Binary Search for sequence in sequence map
        while (low < high) {
            const std::size_t mid = low + ((high - low) / 2);
            const std::size_t mid_as_pointer = row_to_byte(mid);

            sequence_map.seekg(mid_as_pointer);

            Sequence mid_sequence;
            for (std::size_t i = 0; i < m_order; i++) {
                const auto index = binary_read<std::size_t>(sequence_map);
                mid_sequence.push_back(index);
            }

            if (current_sequence < mid_sequence) {
                high = mid;
            } else if (current_sequence > mid_sequence) {
                low = mid + 1;
            } else {
                break;
            }
        }

        return binary_read<std::size_t>(sequence_map);
    }

    std::size_t sample_next_token(const std::size_t frequency_row_size, std::istream& frequency_table) {
        std::vector<double> weights;
        weights.reserve(frequency_row_size);
        std::vector<std::size_t> map;
        map.reserve(frequency_row_size);

        for (std::size_t i = 0; i < frequency_row_size; i++) {
            const auto token_index = binary_read<std::size_t>(frequency_table);
            map.push_back(token_index);

            const auto weight = binary_read<std::size_t>(frequency_table);
            weights.push_back(static_cast<double>(weight));
        }

        // Sample next token index
        std::discrete_distribution<std::size_t> distribution(weights.begin(), weights.end());
        return map[distribution(m_gen)];
    }
};

template <PrinterLike printer>
void TextGenerator::generate(const std::filesystem::path& path, std::ostream& out, const std::size_t output_token_count) {
    std::ifstream token_table(path.string() + FILE_EXTENSION_TOKEN_TABLE, std::ios::binary);
    std::ifstream token_map(path.string() + FILE_EXTENSION_TOKEN_MAP, std::ios::binary);
    std::ifstream sequence_map(path.string() + FILE_EXTENSION_SEQUENCE_MAP, std::ios::binary);
    std::ifstream frequency_table(path.string() + FILE_EXTENSION_FREQUENCY_TABLE, std::ios::binary);
    generate(token_table, token_map, sequence_map, frequency_table, out, output_token_count);
}

template <PrinterLike printer>
void TextGenerator::generate(std::istream& token_table,
                             std::istream& token_map,
                             std::istream& sequence_map,
                             std::istream& frequency_table,
                             std::ostream& out,
                             std::size_t output_token_count) {
    read_token_table(token_table);

    // Get a random sequence and write it
    const std::size_t random_sequence_byte_pointer = row_to_byte(get_random_sequence_row());
    sequence_map.seekg(random_sequence_byte_pointer);

    std::size_t current_output_token_count = 0;
    Sequence current_sequence;

    for (; current_output_token_count < std::min(m_order, output_token_count); current_output_token_count++) {
        const auto token_index = binary_read<std::size_t>(sequence_map);
        current_sequence.push_back(token_index);

        const std::string token = get_token(token_index, token_map);
        printer{}(out, token);
    }

    // Generate rest of tokens
    for (; current_output_token_count < output_token_count; current_output_token_count++) {
        const std::size_t frequency_table_pointer = get_frequency_table_pointer_for(current_sequence, sequence_map);

        frequency_table.seekg(frequency_table_pointer);
        const auto frequency_row_size = binary_read<std::size_t>(frequency_table);

        if (frequency_row_size == 0) {
            break;
        }

        const std::size_t next = sample_next_token(frequency_row_size, frequency_table);

        const auto token = get_token(next, token_map);
        printer{}(out, token);

        std::shift_left(current_sequence.begin(), current_sequence.end(), 1);
        current_sequence.back() = next;
    }
}

#endif  // H_GENERATOR