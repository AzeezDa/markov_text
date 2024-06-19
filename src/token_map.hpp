#ifndef H_TOKEN_MAP
#define H_TOKEN_MAP

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class ChainConstructor;

class TokenMap : std::unordered_map<std::string, std::size_t> {
public:
    TokenMap();

    using std::unordered_map<std::string, std::size_t>::operator[];

    std::size_t try_insert(std::string&& token);

    friend class ChainConstructor;

private:
    std::size_t m_current_index;
    std::vector<std::string> m_inverse_map;
};

#endif  // H_TOKEN_MAP