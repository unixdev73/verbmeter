/* Copyright (c) 2025 unixdev73@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <verbmeter/algo.hpp>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: <element> <element> [<element>]...\n";
    return 1;
  }

  std::vector<std::string> elements{std::size_t(argc - 1), ""};
  for (int i = 1; i < argc; ++i)
    elements[i - 1] = argv[i];

  std::vector<std::pair<std::string const *, std::string const *>> combo{};
  if (auto error = al::gen2ElementCombinations(elements, &combo); error) {
    std::cerr << "Failed to generate combinations with error code: " << error;
    std::cerr << std::endl;
    return 2;
  }

  for (std::size_t i = 0; i < combo.size(); ++i)
    std::cout << *combo[i].first << " " << *combo[i].second << "\n";
  return 0;
}
