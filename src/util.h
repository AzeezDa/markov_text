#pragma once
#include <iostream>

template <typename T>
constexpr size_t binary_write(std::ostream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));

    return sizeof(T);
}

template <typename T>
constexpr size_t binary_read(std::istream& in, T& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(T));

    return sizeof(T);
}

template<>
size_t binary_write(std::ostream& out, const std::string& value) {
    out << value << '\0';

    return value.size() + 1;
}

template<>
size_t binary_read(std::istream& in, std::string& value) {
    std::getline(in, value, '\0');

    return value.size() + 1;
}