#pragma once

#include <unordered_map>
#include <vector>

struct ChainConstructor;

struct TokenMap : private std::unordered_map<std::string, int> {
    TokenMap();

    using std::unordered_map<std::string, int>::operator[];

    size_t try_insert(std::string&& token);

    friend void save_chain(const std::string&, const ChainConstructor&);
private:
    size_t m_current_index;
    std::vector<std::string> m_inverse_map;
};
