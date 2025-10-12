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

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace qy {
struct WordInfoT {
  std::vector<std::size_t> positions{};
  std::size_t count{};
};

struct DatabaseT {
  std::unordered_map<std::string, WordInfoT> wordInfo{};
  std::vector<std::string> words{};
};
} // namespace qy

namespace qy {
/* DESCRIPTION:
 *
 * Sorts the collected words, most common first.
 *
 * EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'file' argument does not point to a valid file.
 */
int sortWordsByOccurrence(DatabaseT *const db);

/* DESCRIPTION:
 *
 * Iterates over each word in a file and
 * stores each position at which it occurs.
 * The time complexity is O(n).
 * It requires that the countWordOccurrence() method be executed first.
 *
 * EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'file' argument does not point to a valid file.
 *
 * 3 - The database has not been initialized.
 */
int extractWordPositions(DatabaseT *const db, std::string const &file);

/* DESCRIPTION:
 *
 * Iterates over each word in a file and counts how many times it occurs.
 * The time complexity is O(n).
 *
 * EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'file' argument does not point to a valid file.
 */
int countWordOccurrence(DatabaseT *const db, std::string const &file);
} // namespace qy
