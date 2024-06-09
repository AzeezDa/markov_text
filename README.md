# `markov_text` - A Text Generator Based on Higher-Order Markov Chains

**This project is currently work in progress**

## Installing and Building
1. Install CMake (at least version 3.11)
2. While in this directory run `cmake -B build`
3. After (2) finishes, run `cmake --build build`
4. Everything should be done now!

## Running (Linux-based only)
Write `./build/markov_text -h` for help.

An example usage is given below, where first the construction command is done:

```bash
./build/markov_text -c corpus -O 3 -o out
```
which will construct an order-`3` Markov chain based on the large text file `corpus` and save it as four files, starting with `out`.

Then to generate text, run

```bash
./build/markov_text -g out -s 1000
```
which will generate `1000` tokens based on the chain that is stored in the files starting with `out`.

## Goals
### High Priority
- [x] Saving the constructed Markov chain as a file for faster reusage ~~or training with more tokens~~
- [ ] Optimisation of the chain construction process

### Later
- [ ] Support for other encodings other than ASCII
- [ ] Prompts