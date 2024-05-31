#pragma once

#include <unordered_map>

struct TokenMap : private std::unordered_map<std::string, int> {
    TokenMap(); 

    using std::unordered_map<std::string, int>::operator[];

    size_t try_insert(const std::string& token);

    friend std::ostream& operator<<(std::ostream& os, const TokenMap& map);

private:
    size_t m_current_index;
};

std::ostream& operator<<(std::ostream& os, const TokenMap& map);