#pragma once

#include <fstream>
#include <iostream>
#include <vector>

struct TokenBytePointer
{
    size_t sequence_index;
    size_t map_index;
};


struct TextGenerator {
    TextGenerator(const std::string&);
    void generate(const size_t);

private:
    std::ifstream m_sequence_index;
    std::ifstream m_token_map;
    std::ifstream m_frequency;
    size_t m_order;
    std::vector<TokenBytePointer> m_token_byte_pointers;
};