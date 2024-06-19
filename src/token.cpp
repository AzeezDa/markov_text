#include "token.hpp"

void ASCIITokenizer::operator()(std::istream& in, const push_function& push) {
    // Read char by char and tokenise according to some rules. I chose the ones
    // below just because they gave good enough (but not perfect) results for
    // English texts
    char ch;
    std::string current;
    while (!in.eof()) {
        in >> std::noskipws >> ch;

        const bool token_empty = current.empty();

        if (((std::isspace(ch) != 0) || (std::isprint(ch) == 0)) && !token_empty) {
            push(current);
        } else if ((std::isalnum(ch) != 0) || !token_empty && (std::isalnum(current.back()) != 0)) {
            current.push_back(ch);
        } else if (std::ispunct(ch) != 0) {
            if (!current.empty()) {
                push(current);
            }

            current.push_back(ch);
            if (ch != '$') {
                push(current);
            }
        }
    }
}

void ASCIITokenPrinter::operator()(std::ostream& out, const std::string& token, char previous) {
    // Printing out by following English rules for spacing before and after punctuations
    // It is not perfect but works good enough
    if (((std::ispunct(token[0]) == 0) || token[0] == '$' || token[0] == '(' || token[0] == '&') && previous != '(') {
        out << ' ';
    }

    out << token;
}