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

#include "../private/query.hpp"
#include <iostream>
#include <verbmeter/query.hpp>

void printStats(qy::DatabaseT const *const db);

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: <filepath>" << std::endl;
    return 1;
  }

  std::string const filepath = argv[1];
  qy::DatabaseT db{};

  auto err = qy::queryFile(&db, filepath);
  if (err) {
    std::cerr << "Failed to query file with error code: " << err << std::endl;
    return 2;
  }

  printStats(&db);
  return 0;
}

void printStats(qy::DatabaseT const *const db) {
  for (auto const &info : db->wordInfo) {
    std::cout << "WORD: " << info.first << ", COUNT: " << info.second.count;
    std::cout << " POSITIONS: ";
    for (std::size_t i = 0; i < info.second.positions.size(); ++i) {
      std::cout << info.second.positions[i];
      std::cout << (i < info.second.positions.size() - 1 ? ", " : "");
    }
    std::cout << std::endl;
  }
}
