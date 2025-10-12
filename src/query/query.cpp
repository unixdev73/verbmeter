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
#include <verbmeter/query.hpp>
#include <algorithm>
#include <fstream>
#include <list>

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
  if (!db)
    return 1;

  if (auto error = countWordOccurrence(db, file); error)
    return 2;

  if (auto error = extractWordPositions(db, file); error)
    return 3;

  if (auto error = sortWordsByOccurrence(db); error)
    return 4;

  return 0;
}

int sortWordsByOccurrence(DatabaseT *const db) {
  if (!db)
    return 1;

  auto condition = [db](std::string const &a, std::string const &b) {
    auto const scoreA = db->wordInfo.at(a).count;
    auto const scoreB = db->wordInfo.at(b).count;
    return scoreA > scoreB;
  };

  std::sort(db->words.begin(), db->words.end(), condition);
  return 0;
}

int getWords(Database const db, std::vector<std::string> *const out,
             std::size_t count) {
  if (!db)
    return 1;
  if (count > db->words.size())
    return 2;
  if (!out)
    return 3;

  out->reserve(count);
  for (std::size_t i = 0; i < count; ++i)
    out->push_back(db->words[i]);
  return 0;
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

  std::list<std::string> wordList{};
  std::string currentWord{};

  while (stream >> currentWord) {
    if (db->wordInfo.contains(currentWord))
      ++db->wordInfo.at(currentWord).count;
    else {
      db->wordInfo.emplace(currentWord, WordInfoT{{}, 1});
      wordList.push_back(std::move(currentWord));
    }
  }

  db->words.resize(wordList.size());
  auto element = wordList.begin();
  for (std::size_t i = 0; i < wordList.size(); ++i) {
    db->words[i] = std::move(*element);
    element = std::next(element);
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

  std::size_t position{};
  std::string word{};

  while (stream >> word)
    db->wordInfo.at(word).positions.push_back(position++);
  return 0;
}
} // namespace qy
