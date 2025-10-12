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

UniqueDatabase createUniqueDatabase() {
  Database db{nullptr};
  createDatabase(&db);
  return {db, destroyDatabase};
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

int getWords(Database const db, std::vector<std::string> *const out,
             std::size_t count) {
  if (!db)
    return 1;
  if (count > db->sortedUniqueWords.size())
    return 2;
  if (!out)
    return 3;

  out->reserve(count);
  for (std::size_t i = 0; i < count; ++i)
    out->push_back(db->sortedUniqueWords[i]);
  return 0;
}

int getWordPositions(Database const db, std::string const &word,
                     std::vector<std::size_t> *const pos) {
  if (!db)
    return 1;
  if (!db->wordInfo.contains(word))
    return 2;
  if (!pos)
    return 3;

  auto const &positions = db->wordInfo.at(word).positions;
  pos->resize(positions.size());
  for (std::size_t i = 0; i < positions.size(); ++i)
    pos->at(i) = positions.at(i);
  return 0;
}

int getTotalWordCount(Database const db, std::size_t *const count) {
  if (!db)
    return 1;
  if (!count)
    return 2;
  *count = db->totalWordCount;
  return 0;
}
} // namespace qy

// PRIVATE API IMPLEMENTATION
namespace qy {
int sortWordsByOccurrence(DatabaseT *const db) {
  if (!db)
    return 1;

  auto condition = [db](std::string const &a, std::string const &b) {
    auto const scoreA = db->wordInfo.at(a).count;
    auto const scoreB = db->wordInfo.at(b).count;
    return scoreA > scoreB;
  };

  std::sort(db->sortedUniqueWords.begin(), db->sortedUniqueWords.end(),
            condition);
  return 0;
}

int countWordOccurrence(DatabaseT *const db, std::string const &file) {
  if (!db)
    return 1;
  if (file.empty())
    return 2;

  std::ifstream stream{file};
  if (!stream.is_open())
    return 2;
  db->totalWordCount = 0;

  std::list<std::string> wordList{};
  std::string currentWord{};

  while (stream >> currentWord) {
    ++db->totalWordCount;
    if (db->wordInfo.contains(currentWord))
      ++db->wordInfo.at(currentWord).count;
    else {
      db->wordInfo.emplace(currentWord, WordInfoT{{}, 1});
      wordList.push_back(std::move(currentWord));
    }
  }

  db->sortedUniqueWords.resize(wordList.size());
  auto element = wordList.begin();
  for (std::size_t i = 0; i < wordList.size(); ++i) {
    db->sortedUniqueWords[i] = std::move(*element);
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
