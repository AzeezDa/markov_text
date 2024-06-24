#ifndef H_FREQUENCY_TABLE
#define H_FREQUENCY_TABLE

#include <unordered_map>
#include "sequence.hpp"

class FrequencyTable {
public:
    class row : public std::unordered_map<std::size_t, std::size_t> {
    public:
        void increment(const std::size_t next) {
            (*this)[next] += 1;
        }
    };

    row& try_insert_and_get(const Sequence& sequence) {
        auto [value, _] = m_table.insert({sequence, {}});
        return value->second;
    }

    const row& get_row(const Sequence& sequence) const {
        return m_table.at(sequence);
    }

    std::size_t size() const {
        return m_table.size();
    }

    auto begin() const {
        return m_table.begin();
    }

    auto end() const {
        return m_table.end();
    }

private:
    std::unordered_map<Sequence, row, Sequence::Hasher> m_table;
};

#endif  // H_FREQUENCY_TABLE