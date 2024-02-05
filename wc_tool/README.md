
# **wc tool (ccwc)**

## Description
A tool that mimics the built in Unix wc tool (slightly stripped).\
Prints line, word, and byte count for each file, and a total if more than one file was specified.

The coding Challenge prompt [Write Your Own wc Tool][].

[Write Your Own wc Tool]: https://codingchallenges.fyi/challenges/challenge-wc

## Quick Start
Download the ccwc.c file from the repository.

Compile
```c
// compiling using gcc
gcc -g -Wall ccwc.c -o ccwc
```
## Usage
The tool takes 2 types of arguments, flags and files.\
The order does not matter.

Example
```c
// from the directory that contains the ccwc file
./ccwc [flags]...[file/directory]..
```

Default
```c
./ccwc [file/directory]
```
By default running the program without any flags will print the line, word, and byte count respectively. When there is more than one file in the arguments the tool will add the name of the file associated with the count on each line and print an additional line with the totals. When there is no file in the arguments it will read from stdin.

The flags that can be used to select which counts to print. The order is always: line, word, character, byte.
* `-c` - to print the byte count
* `-m` - to print the character count
* `-l` - to print the line count
* `-w` - to print the word count

Flags can be cascaded, `-lw` will print the line count and word count. This can be done for all flags.

> **_NOTE:_** Currently the character count is the same as the byte count, this is because my Locale does not recognize multi-byte characters.

## For Future Updates?
- Fix the character count
- Add the remaining features from wc
    - Files specified in a file
    - The maximum display width
- Add the secondary descriptive flag.