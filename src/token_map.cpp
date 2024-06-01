#include "token_map.h"
#include <iostream>

TokenMap::TokenMap() : std::unordered_map<std::string, int>({}), m_current_index(0) {}

size_t TokenMap::try_insert(std::string&& token) {
    if (find(token) != this->end()) {
        return (*this)[token];
    }

    m_inverse_map.push_back(token);
    (*this)[std::move(token)] = m_current_index;
    const size_t inserted_index = m_current_index;
    m_current_index += 1;

    return inserted_index;
}

const std::string& TokenMap::get_token_at(size_t index) const {
    return m_inverse_map.at(index);
}

std::ostream& operator<<(std::ostream& os, const TokenMap& map) {
    for (const auto& [key, value] : map) {
        os << key << ':' << value << ' ';
    }
    return os;
}