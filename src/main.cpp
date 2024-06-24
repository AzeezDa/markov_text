#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "argparse/argparse.hpp"
#include "chain.hpp"
#include "generator.hpp"
#include "token.hpp"

const std::size_t DEFAULT_OUTPUT_TOKEN_COUNT = 100;
const std::size_t DEFAULT_CHAIN_ORDER = 3;
const std::string DEFAULT_CHAIN_FILE_NAME = "out";

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
        .default_value(DEFAULT_CHAIN_ORDER)
        .scan<'i', std::size_t>()
        .help("The order of the chain to be constructed");

    program.add_argument("-o")
        .default_value(DEFAULT_CHAIN_FILE_NAME)
        .help("The Name of the constructed chain files");

    program.add_argument("-s")
        .default_value(DEFAULT_OUTPUT_TOKEN_COUNT)
        .scan<'i', std::size_t>()
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
        const auto order = program.get<std::size_t>("-O");
        check(order > 0, "Order must be a positive integer");

        const auto input_file_path = program.get<std::string>("-c");
        check(std::filesystem::exists(input_file_path), "Input file does not exist!");
        const auto output_file_path = program.get<std::string>("-o");

        std::ifstream input(input_file_path);

        std::clog << "Constructing..." << std::endl;
        Chain chain(order);
        chain.construct(input);
        std::clog << "Saving..." << std::endl;
        chain.save(output_file_path);
        std::clog << "Done! Finishing up..." << std::endl;
    }

    // Generate text... Yes it is possible to construct and generate in the same command
    if (program.is_used("-g")) {
        const auto chain_file_path = program.get<std::string>("-g");
        const auto token_count = program.get<std::size_t>("-s");
        check(std::filesystem::exists(chain_file_path + FILE_EXTENSION_TOKEN_TABLE), "Token Table file does not exist!");
        check(std::filesystem::exists(chain_file_path + FILE_EXTENSION_TOKEN_MAP), "Token Map file does not exist!");
        check(std::filesystem::exists(chain_file_path + FILE_EXTENSION_SEQUENCE_MAP), "Sequence Map file does not exist!");
        check(std::filesystem::exists(chain_file_path + FILE_EXTENSION_FREQUENCY_TABLE), "Frequency Table file does not exist!");

        TextGenerator().generate(chain_file_path, std::cout, token_count);
    }
}