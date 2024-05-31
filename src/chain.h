#pragma once

#include "frequency_matrix.h"
#include "token_map.h"

struct Chain {
    Chain(const size_t, std::istream&);

    friend std::ostream& operator<<(std::ostream& os, const Chain& chain);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
};

std::ostream& operator<<(std::ostream& os, const Chain& chain);