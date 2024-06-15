#ifndef H_FREQUENCY_MATRIX
#define H_FREQUENCY_MATRIX

#include <unordered_map>
#include <filesystem>
#include <vector>

using sequence = std::vector<std::size_t>;

// Based on the answer by see in: https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
struct sequence_hasher {
    std::size_t operator()(const sequence& sequence) const noexcept {
        std::size_t seed = sequence.size();
        for (auto n : sequence) {
            n = ((n >> 16) ^ n) * 0x45d9f3b;
            n = ((n >> 16) ^ n) * 0x45d9f3b;
            n = (n >> 16) ^ n;
            seed ^= n + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

using frequency_row = std::unordered_map<std::size_t, std::size_t>;
using frequency_matrix = std::unordered_map<sequence, frequency_row, sequence_hasher>;

class ChainConstructor;

class FrequencyMatrix : frequency_matrix {
public:
    FrequencyMatrix(const std::size_t order);

    using frequency_matrix::operator[];
    using frequency_matrix::at;

    void increment(const sequence& sequence, const std::size_t& next);

    friend void save_chain(const std::filesystem::path&, const ChainConstructor&);

private:
    const std::size_t m_order;
};

#endif  // H_FREQUENCY_MATRIX