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

#include <verbmeter/query.hpp>
#include <verbmeter/algo.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include <list>

namespace {
inline void log(std::string const &msg, std::ostream &out = std::cout) {
  out << "INF: " << msg << "\n";
}
} // namespace

namespace vr {
struct WordPairInfoT {
  std::vector<std::size_t> distances;
  double distanceAvg{};
};

struct DistanceHistogramT {
  using WordPair = std::pair<std::string const *, std::string const *> *;
  std::unordered_map<WordPair, WordPairInfoT> wordPairInfo;
};

int computeSinglePairDistances(std::vector<std::size_t> const *const posA,
                               std::vector<std::size_t> const *const posB,
                               std::size_t const totalWordCount,
                               bool const aEqualsB,
                               std::vector<std::size_t> *const out);

int computeWordDistances(
    qy::Database const db,
    std::vector<std::pair<std::string const *, std::string const *>>
        *const variations,
    DistanceHistogramT *const hist);

int writeHistogramData(DistanceHistogramT *const hist, std::ostream &out);
void printHistogramData(DistanceHistogramT *const hist);
} // namespace vr

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "Usage: <filepath> <number of most freq words> <output>\n";
    return 1;
  }

  auto dbPtr = qy::createUniqueDatabase();
  qy::Database const db = dbPtr.get();
  std::string const filepath{argv[1]};
  std::string const output{argv[3]};
  std::size_t numOfMfw{};

  try {
    numOfMfw = std::stoull(argv[2]);
  } catch (...) {
    std::cerr << "Failed to convert: '" << argv[2] << "' to a number\n";
    return 1;
  }
  if (numOfMfw < 2) {
    std::cerr << "The number of most frequent words must be at least 2\n";
    return 1;
  }

  if (auto error = qy::queryFile(db, filepath); error) {
    std::cerr << "Failed to query file with error code: " << error << std::endl;
    return 1;
  }

  std::vector<std::string> mostFrequentWords{};
  if (auto error = qy::getWords(db, &mostFrequentWords, numOfMfw); error) {
    std::cerr << "Failed to get words with error code: " << error << std::endl;
    return 1;
  }

  std::vector<std::pair<std::string const *, std::string const *>> variations{};
  if (auto error = al::gen2ElementVariations(mostFrequentWords, &variations);
      error) {
    std::cerr << "Failed to get variations with error code: " << error
              << std::endl;
    return 1;
  }

  vr::DistanceHistogramT histogram{};
  if (auto error = vr::computeWordDistances(db, &variations, &histogram);
      error) {
    std::cerr << "Failed to compute distances with error code: " << error
              << std::endl;
    return 1;
  }

  vr::printHistogramData(&histogram);
  std::cout << "\n";

  std::ofstream outstream{output};
  if (!outstream.is_open())
    return 1;
  vr::writeHistogramData(&histogram, outstream);

  return 0;
}

namespace vr {
int computeSinglePairDistances(std::vector<std::size_t> const *const posA,
                               std::vector<std::size_t> const *const posB,
                               std::size_t const totalWordCount,
                               bool const aEqualsB,
                               std::vector<std::size_t> *const out) {
  if (!out)
    return 1;

  out->clear();
  out->reserve(std::max(posA->size(), posB->size()));

  const auto ascending = [](auto const &a, auto const &b) { return a < b; };
  std::list<std::size_t> distances{};

  for (std::size_t i = 0; i < posA->size(); ++i) {
    for (std::size_t j = 0; j < posB->size(); ++j) {
      auto const endOffset = posB->at(j);
      auto const begin = posA->at(i);
      if (endOffset < posA->at(i))
        distances.push_back(totalWordCount - posA->at(i) + endOffset);
      else
        distances.push_back(endOffset - begin);
    }

    distances.sort(ascending);
    if (aEqualsB)
      distances.pop_front();
    if (distances.size())
      out->push_back(distances.front());
    distances.clear();
  }
  return 0;
}

int computeWordDistances(
    qy::Database const db,
    std::vector<std::pair<std::string const *, std::string const *>>
        *const variations,
    DistanceHistogramT *const hist) {
  if (!db)
    return 1;
  if (!variations)
    return 2;
  if (!hist)
    return 3;

  std::size_t totalWordCount{};
  qy::getTotalWordCount(db, &totalWordCount);

  for (auto &pair : *variations) {
    std::vector<std::size_t> positionsA{}, positionsB{};
    qy::getWordPositions(db, *pair.first, &positionsA);
    qy::getWordPositions(db, *pair.second, &positionsB);
    bool const aEqualsB = (*pair.first == *pair.second);
    WordPairInfoT info{};
    computeSinglePairDistances(&positionsA, &positionsB, totalWordCount,
                               aEqualsB, &info.distances);
    info.distanceAvg =
        std::accumulate(info.distances.begin(), info.distances.end(), 0);
    info.distanceAvg = double(info.distanceAvg) / double(info.distances.size());
    hist->wordPairInfo.emplace(&pair, std::move(info));
  }
  return 0;
}

int writeHistogramData(DistanceHistogramT *const hist, std::ostream &out) {
  if (!hist)
    return 1;
  for (auto const &[pair, info] : hist->wordPairInfo) {
    std::string const wordA = *pair->first;
    std::string const wordB = *pair->second;
    out << wordA << " " << wordB << " " << info.distanceAvg << " ";
    for (auto const dist : info.distances)
      out << dist << " ";
    out << std::endl;
  }
  return 0;
}

void printHistogramData(DistanceHistogramT *const hist) {
  writeHistogramData(hist, std::cout);
}
} // namespace vr
