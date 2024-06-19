#ifndef H_TOKEN
#define H_TOKEN

#include <concepts>
#include <functional>
#include <istream>
#include <ostream>
#include <string>

using push_function = std::function<void(std::string&)>;

template <class T>
concept TokenizerLike = std::is_invocable_r_v<void, T, std::istream&, const push_function&>;

template <class T>
concept TokenPrinterLike = std::is_invocable_r_v<void, T, std::ostream&, const std::string&, unsigned char>;

class ASCIITokenizer {
public:
    void operator()(std::istream& in, const push_function& push);
};

class ASCIITokenPrinter {
public:
    void operator()(std::ostream& out, const std::string& token, unsigned char previous);
};

#endif  // H_TOKEN