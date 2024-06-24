#ifndef H_TOKEN_MAP
#define H_TOKEN_MAP

#include <string>
#include <unordered_map>
#include "sequence.hpp"

class TokenMap {
public:
    TokenMap() : m_current_index(0) {}

    std::size_t try_insert(const std::string& token) {
        const auto [value, inserted] = m_map.insert({token, m_current_index});
        if (inserted) {
            m_inverse_map.push_back(token);
            m_current_index += 1;
        }
        return value->second;  // second is value of key-value pair
    }

    std::size_t get_index(const std::string& token) const {
        return m_map.at(token);
    }

    const std::string& get_token(const std::size_t index) const {
        return m_inverse_map[index];
    }

    std::size_t size() const {
        return m_current_index;
    }

private:
    std::size_t m_current_index;
    std::unordered_map<std::string, std::size_t> m_map;
    std::vector<std::string> m_inverse_map;
};

#endif  // H_TOKEN_MAP