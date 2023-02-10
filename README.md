# Fuzzy C/C++ Antlr Grammar

Fuzzyc, an island grammar-based fuzzy C/C++ parser, was originally introduced
in a paper ["Chucky: Exposing Missing Checks in Source Code
for Vulnerability Discovery"](https://intellisec.de/pubs/2013-ccs.pdf) with
its source code provided at https://github.com/fabsx00/joern.

Fuzzyc parser is based on an
[island grammar](https://ieeexplore.ieee.org/document/957806)
that allows the parser to analyze code without requiring code to be compiled or
a build environment to be configured.

The code was migrated to a new repository (https://github.com/joernio/joern)
and integrated with other projects, and this directory pulls only the Fuzzyc
ANTLR grammar files from the repo.

Note: the Fuzzy C/C++ Antlr grammar files were removed from the original source as of Jun 27, 2022 (https://github.com/joernio/joern/commit/61ed5b010d77b7705bd0e99144fcbc1acaf4d26c).