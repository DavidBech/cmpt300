# Assignment 4 branch

All Code Written By David Bechert

## Usage 

`./UnixLs [OPTION] ... [FILES] ...`

## Assignment Details

- Implematation of `ls` command
  - Supports -R (Recursion), -i (iNode Value), and -l (long listing)
  - All CLI options must be placed prior to files

## Makefile Targets

- `make`, `make all`, or `make UnixLs` will all build the default UnixLs binary file
- `make set_test` will build a binary file to test the set implementation
- `make clean` to remove any object files and binaries created
