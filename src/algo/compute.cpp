#include <verbmeter/algo.hpp>

namespace al {
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
} // namespace al
