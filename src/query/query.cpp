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

#include "private/query.hpp"
#include <fstream>
#include <verbmeter/query.hpp>

// PUBLIC API IMPLEMENTATION
namespace qy {
int createDatabase(Database *const db) {
  *db = new DatabaseT{};
  if (*db)
    return 1;
  return 0;
}

void destroyDatabase(Database const db) { delete db; }

int queryFile(Database const db, std::string const &file) {
  if (auto error = countWordOccurrence(db, file); error)
    return error;

  if (auto error = extractWordPositions(db, file); error)
    return error;

  return 0;
}
} // namespace qy

// PRIVATE API IMPLEMENTATION
namespace qy {
int countWordOccurrence(DatabaseT *const db, std::string const &file) {
  if (!db)
    return 1;
  if (file.empty())
    return 2;

  std::ifstream stream{file};
  if (!stream.is_open())
    return 2;

  std::string word{};
  while (stream >> word) {
    if (db->wordInfo.contains(word))
      ++db->wordInfo.at(word).count;
    else
      db->wordInfo.emplace(word, WordInfoT{{}, 1});
  }
  return 0;
}

int extractWordPositions(DatabaseT *const db, std::string const &file) {
  if (!db)
    return 1;
  if (file.empty())
    return 2;

  std::ifstream stream{file};
  if (!stream.is_open())
    return 2;

  for (auto &record : db->wordInfo) {
    const auto size = record.second.count;
    record.second.positions.reserve(size);
  }

  std::string word{};
  SizeT position{};

  while (stream >> word)
    db->wordInfo.at(word).positions.push_back(position++);
  return 0;
}
} // namespace qy
