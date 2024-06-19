#ifndef H_BINARY_IO
#define H_BINARY_IO

#include <istream>
#include <ostream>
#include <type_traits>
#include <string>

template <typename T>
struct dependable_false : std::false_type {};

template <typename T>
inline constexpr bool is_binary_streamable = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

// Write a value in binary to the output stream. Strings are written byte by byte,
// and are null-terminated. Other values (trivial + standard layout) are written as they are.
template <typename T>
constexpr std::size_t binary_write(std::ostream& out, const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        out << value << '\0';
        return value.size() + 1;
    } else if constexpr (is_binary_streamable<T>) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
        return sizeof(T);
    } else {
        static_assert(dependable_false<T>::value, "Not binary writeable");
        return 0;
    }
}

// Read a value as binary from the input stream. Strings are read byte by byte,
// and must be null-terminated. Other (trivial + standard layout) values are read as they are.
template <typename T>
constexpr T binary_read(std::istream& in) {
    T t;
    if constexpr (std::is_same_v<T, std::string>) {
        std::getline(in, t, '\0');
        return t;
    } else if constexpr (is_binary_streamable<T>) {
        in.read(reinterpret_cast<char*>(&t), sizeof(T));
        return t;
    } else {
        static_assert(dependable_false<T>::value, "Not binary readable");
        return 0;
    }
}

#endif  // H_BINARY_IO
