#pragma once

#include "frequency_matrix.h"
#include "token_map.h"

struct ChainConstructor {
    ChainConstructor(const size_t, std::istream&);

    friend void save_chain(const std::string&, const ChainConstructor&);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
    size_t m_order;
};

void save_chain(const std::string&, const ChainConstructor&);