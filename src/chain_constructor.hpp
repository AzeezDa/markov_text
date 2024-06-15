#ifndef H_CHAIN_CONSTRUCTOR
#define H_CHAIN_CONSTRUCTOR

#include <istream>
#include <filesystem>
#include "frequency_matrix.hpp"
#include "token_map.hpp"

class ChainConstructor {
public:
    ChainConstructor(const std::size_t, std::istream&);

    friend void save_chain(const std::filesystem::path&, const ChainConstructor&);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
    std::size_t m_order;
};

void save_chain(const std::filesystem::path&, const ChainConstructor&);

#endif  // H_CHAIN_CONSTRUCTOR