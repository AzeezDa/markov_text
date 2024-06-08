#include <cassert>
#include <iostream>
#include "argparse/argparse.hpp"
#include "chain_constructor.h"

constexpr void check(const bool& to_check, const std::string& error_message) {
    if (!to_check) {
        // BRIGHT RED-vvvvvvvvvv      vvvvvvv-RESET
        std::cerr << "\033[1;31mERROR:\033[0m " << error_message << std::endl;
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("markov_text", "0.0.1");
    program.add_description("A text generator based on higher-order Markov chains. "
                            "The chain is constructed by reading a text passed into stdin. "
                            "The generated text will be outputted into stdout. ");

    // program.add_argument("order")
    //     .help("The order of the Markov Chain (i.e. the length of token sequences)")
    //     .scan<'i', size_t>();

    // program.add_argument("output_size")
    //     .help("The number of tokens generated")
    //     .scan<'i', size_t>();

    try {
        program.parse_args(argc, argv);

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << program;
        std::exit(1);
    }

    // size_t order = program.get<size_t>("order");
    // size_t output_size = program.get<size_t>("output_size");
    // check(order > 0, "Order must be a positive integer");
    // check(output_size > 0, "Output size must be a positive integer");

    ChainConstructor chain(2, std::cin);
    // chain.generate(output_size, std::cout);
    save_chain("out", chain);
}