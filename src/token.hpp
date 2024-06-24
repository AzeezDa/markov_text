#ifndef H_TOKEN
#define H_TOKEN

#include <concepts>
#include <istream>
#include <ostream>
#include <string>

template <class T>
concept TokenizerLike = std::is_invocable_r_v<std::string, T, std::istream&>;

template <class T>
concept PrinterLike = std::is_invocable_r_v<void, T, std::ostream&, const std::string&>;

class DefaultTokenizer {
public:
    std::string operator()(std::istream& in) const {
        std::string output;
        in >> output;
        return output;
    }
};

class DefaultPrinter {
public:
    void operator()(std::ostream& out, const std::string& token) const {
        out << token << ' ';
    }
};

#endif  // H_TOKEN