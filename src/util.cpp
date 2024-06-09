#include "util.h"

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