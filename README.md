# `markov_text` - A Text Generator Based on Higher-Order Markov Chains

## Installing and Building
1. Install CMake (at least version 3.11)
2. While in this directory run `cmake -B build`
3. After (2) finishes, run `cmake --build build`
4. Everything should be done now!

## Running
Write `./build/markov_text -h` for help.

An example usage is given below, where first the construction command is done:

```bash
./build/markov_text -c corpus -O 3 -o out
```
which will construct an order-`3` Markov chain based on the large text file `corpus` and save it as four files, starting with `out`. Note that `-O 3` (order `3`) and `-o out` (output file path `out`) are the default and can be omitted. Thus, calling `./build/markov_text -c corpus` will be equivalent to the command above.

Then to generate text, run:

```bash
./build/markov_text -g out -s 100
```
which will generate at most `100` tokens based on the chain that is stored in the files starting with `out`. Note that the value `-s 100` (generate at most `100` tokens) is the default value and can be omitted. Thus, calling `./build/markov_text -g out` is equivalent to the command above.

### Notes

1. It is undefined behaviour if the input file has fewer tokens than the order of the constructed chain
2. The reason for the generation of "at most" `N` tokens is that if the Markov Chain has no next state then the text generation process ends. This can happen when the current sequence of tokens is a unique sequence that appears at the end of the input text file. This can be produced be creating a file when `K` unique tokens then generating `N` < `K` tokens. In this case, at most `K` tokens will be produced.

## Contributions
Contributions and feedback are more than welcome!