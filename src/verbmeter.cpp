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

#include <filesystem>
#include <verbmeter/algo.hpp>
#include "histogram.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

namespace vr {
int writeMappingFile(DistanceHistogramT const *const hist, std::ostream &out);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr
        << "Usage: <input file> <output dir path> <number of most freq words>";
    return 1;
  }

  std::string const inputFile{argv[1]};
  std::string const outputDir{argv[2]};
  std::size_t numOfMfw{};

  try {
    numOfMfw = std::stoull(argv[3]);
  } catch (...) {
    std::cerr << "Failed to convert: '" << argv[3] << "' to a number\n";
    return 1;
  }
  if (numOfMfw < 2) {
    std::cerr << "The number of most frequent words must be at least 2\n";
    return 1;
  }

  if (!std::filesystem::exists(inputFile)) {
    std::cerr << "The input file: '" << inputFile << "' does not exist\n";
    return 1;
  }

  if (!std::filesystem::exists(outputDir)) {
    std::cerr << "The input dir: '" << outputDir << "' does not exist\n";
    return 1;
  }

  auto dbPtr = qy::createUniqueDatabase();
  qy::Database const db = dbPtr.get();

  if (auto error = qy::queryFile(db, inputFile); error) {
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

  std::sort(histogram.wordPairPtr.begin(), histogram.wordPairPtr.end(),
            [](auto const &a, auto const &b) {
              return a->second.distanceAvg < b->second.distanceAvg;
            });

  std::ofstream mapping{std::filesystem::path(outputDir) /
                        std::filesystem::path("mapping.txt")};
  if (auto error = writeMappingFile(&histogram, mapping); error) {
    std::cerr << "Writing mapping file failed!" << std::endl;
    return 1;
  }

  for (std::size_t i = 0; i < numOfMfw; ++i) {
    std::ofstream outstream{std::filesystem::path(outputDir) /
                            std::filesystem::path(std::to_string(i) + ".txt")};
    if (!outstream.is_open())
      return 1;
    vr::writeHistogramData(&histogram, outstream);
  }
  return 0;
}

namespace vr {
int writeMappingFile(DistanceHistogramT const *const hist, std::ostream &out) {
  std::size_t index = 0;

  for (auto const &[pair, info] : hist->wordPairInfo) {
    out << index << "\t" << (*pair->first) << " " << (*pair->second) << "\n";
    ++index;
  }

  return 0;
}
} // namespace vr
