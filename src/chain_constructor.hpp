#ifndef H_CHAIN_CONSTRUCTOR
#define H_CHAIN_CONSTRUCTOR

#include <algorithm>
#include <filesystem>
#include <istream>
#include <ostream>
#include <string>
#include "binary_io.hpp"
#include "frequency_matrix.hpp"
#include "token.hpp"
#include "token_map.hpp"

const std::string TOKEN_INDEX_FILE_EXTENSION = ".tix";
const std::string SEQUENCE_INDEX_FILE_EXTENSION = ".six";
const std::string TOKEN_MAP_FILE_EXTENSION = ".map";
const std::string FREQUENCY_FILE_EXTENSION = ".frq";

class ChainConstructor {
public:
    ChainConstructor(const std::size_t);

    template <TokenizerLike tokenizer = ASCIITokenizer>
    void construct(std::istream&);

    void save_chain(const std::filesystem::path&);
    void save_chain(std::ostream&, std::ostream&, std::ostream&, std::ostream&);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
    std::size_t m_order;
};

template <TokenizerLike tokenizer>
void ChainConstructor::construct(std::istream& in) {
    std::vector<std::size_t> sequence(m_order, 0);

    // Insert the token into the map and shift the sequence once to the left,
    // making the last element of the sequence be next token index
    auto push = [&](std::string& current) {
        const std::size_t next = m_map.try_insert(std::move(current));
        m_matrix.increment(sequence, next);

        std::shift_left(sequence.begin(), sequence.end(), 1);
        sequence.back() = next;

        current.clear();
    };

    tokenizer{}(in, push);
}
#endif  // H_CHAIN_CONSTRUCTOR