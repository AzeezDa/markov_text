#include "token_map.h"
#include <iostream>

TokenMap::TokenMap() : m_current_index(0), std::unordered_map<std::string, int>({}) {}

size_t TokenMap::try_insert(const std::string& token) {
    if (find(token) != this->end()) {
        return (*this)[token];
    }

    (*this)[token] = m_current_index;
    const size_t inserted_index = m_current_index;
    m_current_index += 1;

    return inserted_index;
}

std::ostream& operator<<(std::ostream& os, const TokenMap& map) {
    for (const auto& [key, value] : map) {
        os << key << ':' << value << ' ';
    }
    return os;
}