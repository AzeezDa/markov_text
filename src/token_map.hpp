#ifndef H_TOKEN_MAP
#define H_TOKEN_MAP

#include <string>
#include <unordered_map>
#include <vector>

class ChainConstructor;

class TokenMap : std::unordered_map<std::string, int> {
public:
    TokenMap();

    using std::unordered_map<std::string, int>::operator[];

    std::size_t try_insert(std::string&& token);

    friend void save_chain(const std::string&, const ChainConstructor&);

private:
    std::size_t m_current_index;
    std::vector<std::string> m_inverse_map;
};

#endif  // H_TOKEN_MAP