#ifndef H_CHAIN_CONSTRUCTOR
#define H_CHAIN_CONSTRUCTOR

#include <filesystem>
#include <istream>
#include <algorithm>
#include "frequency_matrix.hpp"
#include "binary_io.hpp"
#include "token.hpp"
#include "token_map.hpp"

class ChainConstructor {
public:
    ChainConstructor(const std::size_t);

    template <TokenizerLike tokenizer = ASCIITokenizer>
    void construct(std::istream& in) {
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

    friend void save_chain(const std::filesystem::path&, const ChainConstructor&);

private:
    FrequencyMatrix m_matrix;
    TokenMap m_map;
    std::size_t m_order;
};

void save_chain(const std::filesystem::path&, const ChainConstructor&);

#endif  // H_CHAIN_CONSTRUCTOR