# Introduction

This is a linguistic utility that analyzes text files.

# How to build

```bash
cmake -B build
cmake --build ./build
```

# wordcount

The wordcount utility counts how many words there are in a file.
It also counts the occurrences of each word in the file.
And it lists the positions of each occurrence of each word in the file.

It can be run like so:

```bash
./build/src/query/test/wordcount /path/to/file
```

# verbmhist

The verbmhist utility generates the data required to construct
a histogram of the frequencies of occurrence of words and the distance
between them.

It can be run like so:

```bash
./build/src/verbmhist /path/to/file <numberOfMostCommonWords> /path/to/output/file
```

# mkhists

The mkhists script runs the verbmhist binary on all files in a directory

```bash
./script/mkhists ./path/to/source/dir <numOfMostCommonWords> <sortByAvg>
```

The script generates an output file for each input file with a similar name,
but with the prefix HIST_.
