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
#include "histogram.hpp"
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <numeric>

namespace vr {
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
    al::computeSinglePairDistances(&positionsA, &positionsB, totalWordCount,
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
                       std::string const &outputDir,
                       std::size_t const numOfMfw) {
  if (!hist)
    return 1;
  auto entry = hist->wordPairPtr.begin();

  for (std::size_t i = 0; i < numOfMfw && entry != hist->wordPairPtr.end();
       ++i) {
    std::ofstream outstream{std::filesystem::path(outputDir) /
                            std::filesystem::path(std::to_string(i) + ".txt")};
    if (!outstream.is_open())
      return 1;

    for (std::size_t i = 0; i < (*entry)->second.distances.size(); ++i) {
      outstream << (*entry)->second.distances[i];
      if (i < (*entry)->second.distances.size() - 1)
        outstream << "\n";
    }
                entry = std::next(entry);
  }

  return 0;
}
} // namespace vr
