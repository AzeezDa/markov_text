#ifndef H_SEQUENCE
#define H_SEQUENCE

#include <algorithm>
#include <vector>

class Sequence : public std::vector<std::size_t> {
public:
    Sequence(const std::size_t size) : std::vector<std::size_t>(size) {}
    Sequence() {}
    void shift_push(const std::size_t value) {
        std::shift_left(begin(), end(), 1);
        back() = value;
    }

    // Based on the answer by see in: https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
    struct Hasher {
        std::size_t operator()(const Sequence& sequence) const noexcept {
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
};

#endif  // H_SEQUENCE