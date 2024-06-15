#ifndef H_TEXT_GENERATOR
#define H_TEXT_GENERATOR

#include <fstream>
#include <filesystem>
#include <vector>

class TextGenerator {
public:
    TextGenerator(const std::filesystem::path&);
    void generate(const std::size_t);

private:
    struct TokenBytePointer {
        std::size_t sequence_index;
        std::size_t map_index;
    };
    std::ifstream m_sequence_index;
    std::ifstream m_token_map;
    std::ifstream m_frequency;
    std::size_t m_order;
    std::vector<TokenBytePointer> m_token_byte_pointers;
};

#endif  // H_TEXT_GENERATOR