#include <cassert>
#include <iostream>
#include "argparse/argparse.hpp"
#include "chain_constructor.h"
#include "text_generator.h"

constexpr void check(const bool& to_check, const std::string& error_message) {
    if (!to_check) {
        // BRIGHT RED-vvvvvvvvvv      vvvvvvv-RESET
        std::cerr << "\033[1;31mERROR:\033[0m " << error_message << std::endl;
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    // Set up argparse
    argparse::ArgumentParser program("markov_text", "0.0.1");
    program.add_description("A text generator based on higher-order Markov chains.");

    program.add_argument("-c")
        .help("Construct the chain using a given path to a text file");

    program.add_argument("-g")
        .help("Generate text based on given chain file");

    program.add_argument("-O")
        .default_value<size_t>(3)
        .scan<'i', size_t>()
        .help("The order of the chain to be constructed");

    program.add_argument("-o")
        .default_value(std::string{"out"})
        .help("The Name of the constructed chain files");

    program.add_argument("-s")
        .default_value<size_t>(100)
        .scan<'i', size_t>()
        .help("The number of tokens to generate");

    try {
        program.parse_args(argc, argv);

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << program;
        std::exit(1);
    }

    // Construct the Chain
    if (program.is_used("-c")) {
        const size_t order = program.get<size_t>("-O");
        check(order > 0, "Order must be a positive integer");

        const std::string input_file_path = program.get<std::string>("-c");
        const std::string output_file_path = program.get<std::string>("-o");

        std::ifstream input(input_file_path);

        std::clog << "Constructing..." << std::endl;
        const ChainConstructor chain(order, input);
        std::clog << "Saving..." << std::endl;
        save_chain(output_file_path, chain);
        std::clog << "Done!" << std::endl;
    }

    // Generate text... Yes it is possible to construct and generate in the same command
    if (program.is_used("-g")) {
        const std::string chain_file_path = program.get<std::string>("-g");
        const size_t token_count = program.get<size_t>("-s");

        TextGenerator text_generator(chain_file_path);
        text_generator.generate(token_count);
    }
}