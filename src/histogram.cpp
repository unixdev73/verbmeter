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
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>

namespace vr {
struct WordPairInfoT {
  std::vector<std::size_t> distances;
  double distanceAvg{};
};

struct DistanceHistogramT {
  using WordPair = std::pair<std::string const *, std::string const *> *;

  std::unordered_map<WordPair, WordPairInfoT> wordPairInfo;
  std::vector<typename decltype(wordPairInfo)::iterator> wordPairPtr;
};

int computeSinglePairDistances(std::vector<std::size_t> const *const posA,
                               std::vector<std::size_t> const *const posB,
                               std::size_t const totalWordCount,
                               std::vector<std::size_t> *const out);

int computeWordDistances(
    qy::Database const db,
    std::vector<std::pair<std::string const *, std::string const *>> *const
        variations,
    DistanceHistogramT *const hist);

int writeHistogramData(DistanceHistogramT const *const hist, std::ostream &out);
void printHistogramData(DistanceHistogramT const *const hist);
} // namespace vr

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "Usage: <filepath> <number of most freq words> <output> "
                 "[sortByAvg=no]\n";
    return 1;
  }

  auto dbPtr = qy::createUniqueDatabase();
  qy::Database const db = dbPtr.get();
  std::string const filepath{argv[1]};
  std::string const output{argv[3]};
  bool sortByAvg{false};
  if (argc > 4)
    if (std::string{argv[4]} == "yes")
      sortByAvg = true;
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

  if (sortByAvg) {
    std::sort(histogram.wordPairPtr.begin(), histogram.wordPairPtr.end(),
              [](auto const &a, auto const &b) {
                return a->second.distanceAvg > b->second.distanceAvg;
              });
  }

  // vr::printHistogramData(&histogram);

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
                               std::vector<std::size_t> *const out) {
  if (!posA)
    return 1;
  if (!posB)
    return 2;
  if (!out)
    return 3;

  out->clear();
  out->reserve(posA->size());

  for (std::size_t i = 0; i < posA->size(); ++i) {
    bool loopAround = false;
    auto nearestB = std::upper_bound(posB->begin(), posB->end(), posA->at(i));
    if (nearestB == posB->end()) {
      nearestB = posB->begin();
      loopAround = true;
    }

    auto nearestA = posA->begin();
    if (loopAround)
      nearestA = --(posA->end());
    else
      nearestA =
          std::prev(std::lower_bound(posA->begin(), posA->end(), *nearestB));

    i = nearestA - posA->begin();

    if (*nearestA == *nearestB)
      continue;

    if (loopAround)
      out->push_back(totalWordCount - *nearestA + *nearestB);
    else
      out->push_back(*nearestB - *nearestA);
  }

  return 0;
}

int computeWordDistances(
    qy::Database const db,
    std::vector<std::pair<std::string const *, std::string const *>>
        *variations,
    DistanceHistogramT *hist) {
  if (!db)
    return 1;
  if (!variations)
    return 2;
  if (!hist)
    return 3;

  std::size_t totalWordCount{};
  qy::getTotalWordCount(db, &totalWordCount);
  hist->wordPairPtr.clear();
  hist->wordPairPtr.reserve(variations->size());

  for (auto &pair : *variations) {
    std::vector<std::size_t> positionsA{}, positionsB{};
    qy::getWordPositions(db, *pair.first, &positionsA);
    qy::getWordPositions(db, *pair.second, &positionsB);
    WordPairInfoT info{};
    computeSinglePairDistances(&positionsA, &positionsB, totalWordCount,
                               &info.distances);
    info.distanceAvg =
        std::accumulate(info.distances.begin(), info.distances.end(), 0);
    info.distanceAvg = double(info.distanceAvg) / double(info.distances.size());

    auto pairIt = hist->wordPairInfo.emplace(&pair, std::move(info)).first;
    hist->wordPairPtr.push_back(pairIt);
  }

  return 0;
}

int writeHistogramData(DistanceHistogramT const *const hist,
                       std::ostream &out) {
  if (!hist)
    return 1;
  for (auto const &rec : hist->wordPairPtr) {
    auto const &[pair, info] = *rec;
    std::string const wordA = *pair->first;
    std::string const wordB = *pair->second;
    std::string const part1 = "WORDS: " + wordA + ", " + wordB;
    std::string const part2 = "AVG: " + std::to_string(info.distanceAvg);

    out << part1;
    for (std::size_t i = 0; i < 30 - part1.size(); ++i)
      out << " ";
    out << part2;
    for (std::size_t i = 0; i < 20 - part2.size(); ++i)
      out << " ";
    out << "\tDISTANCES: ";
    for (auto const dist : info.distances)
      out << dist << " ";
    out << std::endl;
  }
  return 0;
}

void printHistogramData(DistanceHistogramT const *const hist) {
  writeHistogramData(hist, std::cout);
}
} // namespace vr
