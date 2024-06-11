#pragma once
#include <iostream>
#include <type_traits>

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
