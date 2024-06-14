#ifndef H_BINARY_IO
#define H_BINARY_IO

#include <iostream>
#include <type_traits>

// Write a value in binary to the output stream. Strings are written byte by byte,
// and are null-terminated. Other values are written as they are.
template <typename T>
constexpr size_t binary_write(std::ostream& out, const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        out << value << '\0';
        return value.size() + 1;
    } else {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
        return sizeof(T);
    }
}

// Read a value as binary from the input stream. Strings are read byte by byte,
// and must be null-terminated. Other values are read as they are.
template <typename T>
constexpr size_t binary_read(std::istream& in, T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        std::getline(in, value, '\0');
        return value.size() + 1;
    } else {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        return sizeof(T);
    }
}

#endif // H_BINARY_IO
