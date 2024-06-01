#pragma once

#include "frequency_matrix.h"
#include "token_map.h"

struct Chain {
    Chain(const size_t, std::istream&);

    void generate(const size_t, std::ostream&);

    friend std::ostream& operator<<(std::ostream&, const Chain&);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
    size_t m_order;
};

std::ostream& operator<<(std::ostream&, const Chain&);