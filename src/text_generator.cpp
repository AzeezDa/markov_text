#include "text_generator.hpp"
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include "binary_io.hpp"
#include "frequency_matrix.hpp"
#include "token.hpp"

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
