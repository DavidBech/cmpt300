# Assignment 4 branch

All Code Written By David Bechert

## Usage

`./UnixLs [OPTION] ... [FILES] ...`

## Options

- `-i` display the inode number prior to the file name
- `-l` display long listing info about the file
- `-R` recursivly list directory entries

## Assignment Details

- Implematation of `ls` command
  - All CLI options must be placed prior to files
  - File outputs assume `-U1` options of standard `ls` command

## Makefile Targets

- `make`, `make all`, or `make UnixLs` will all build the default UnixLs binary file
- `make set_test` will build a binary file to test the set implementation
- `make clean` to remove any object files and binaries created
