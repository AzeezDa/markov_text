#ifndef H_CHAIN
#define H_CHAIN

#include <filesystem>
#include <istream>
#include <limits>
#include "frequency_table.hpp"
#include "token.hpp"
#include "token_map.hpp"

const std::string FILE_EXTENSION_TOKEN_TABLE = ".tt";
const std::string FILE_EXTENSION_TOKEN_MAP = ".tm";
const std::string FILE_EXTENSION_SEQUENCE_MAP = ".st";
const std::string FILE_EXTENSION_FREQUENCY_TABLE = ".ft";
const std::size_t NO_SEQUENCE = std::numeric_limits<std::size_t>::max();

class Chain {
public:
    Chain(const std::size_t order) : m_order(order) {}

    template <TokenizerLike tokenizer = DefaultTokenizer>
    void construct(std::istream& in);

    void save(const std::filesystem::path& path) const;
    void save(std::ostream& token_table, std::ostream& token_map, std::ostream& sequence_map, std::ostream& frequency_table) const;

private:
    const std::size_t m_order;
    TokenMap m_map;
    FrequencyTable m_table;
};

template <TokenizerLike tokenizer>
void Chain::construct(std::istream& in) {
    Sequence current_sequence(m_order);

    // Sequence of first <order> tokens
    for (std::size_t i = 0; i < m_order; i++) {
        const std::string token = tokenizer{}(in);
        const std::size_t index = m_map.try_insert(token);
        current_sequence.shift_push(index);
    }

    // Rest of tokens
    while (!in.eof()) {
        const std::string token = tokenizer{}(in);
        if (token.empty()) {
            continue;
        }
        const std::size_t index = m_map.try_insert(token);
        m_table
            .try_insert_and_get(current_sequence)
            .increment(index);
        current_sequence.shift_push(index);
    }

    // Insert last sequence (which can have 0 frequency in all tokens)
    m_table.try_insert_and_get(current_sequence);
}

#endif  // H_CHAIN