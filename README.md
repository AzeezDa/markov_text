# `markov_text` - A Text Generator Based on Higher-Order Markov Chains

**This project is currently work in progress**

## Installing and Building
1. Install CMake (at least version 3.11)
2. While in this directory run `cmake -B build`
3. After (2) finishes, run `cmake --build build`
4. Everything should be done now!

## Running (Linux-based only)
Write `./build/markov_text -h` for help.

An example usage is:

```bash
./build/markov_text 3 1000 < corpus > output
```
which will generate `1000` tokens using a Markov chain of order `3` that is constructed based on the large text file, `corpus`. The generated text will be written to the `output` file.


## Goals
### High Priority
- [ ] Saving the constructed Markov chain as a file for faster reusage or training with more tokens
- [ ] Optimisation of the chain construction process

### Later
- [ ] Support for other encodings other than ASCII
- [ ] Prompts