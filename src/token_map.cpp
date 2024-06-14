#include "token_map.hpp"
#include <string>

TokenMap::TokenMap() : std::unordered_map<std::string, int>({}), m_current_index(0) {}

size_t TokenMap::try_insert(std::string&& token) {
    // Try to insert the new token into the token map, and return its token index.
    // If the token already exists return the stored index immediately.

    if (find(token) != this->end()) {
        return (*this)[token];
    }

    m_inverse_map.push_back(token);
    (*this)[std::move(token)] = m_current_index;
    const size_t inserted_index = m_current_index;
    m_current_index += 1;

    return inserted_index;
}