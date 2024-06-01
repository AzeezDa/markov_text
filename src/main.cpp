#include <iostream>
#include "chain.h"

int main() {
    Chain c(3, std::cin);
    c.generate(1000, std::cout);
}