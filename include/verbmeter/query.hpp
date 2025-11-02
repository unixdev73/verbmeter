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

#include <memory>
#include <string>
#include <vector>

namespace qy {
struct DatabaseT;
using Database = DatabaseT *;

/* EXIT STATUS:
 * 0 - The operation was completed successfuly.
 *
 * 1 - The operation failed.
 */
int createDatabase(Database *const);

void destroyDatabase(Database const);

using UniqueDatabase = std::unique_ptr<DatabaseT, void (*)(Database const)>;
UniqueDatabase createUniqueDatabase();

/* EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - Counting the word occurrence failed.
 *
 * 3 - Gathering word positions failed.
 *
 * 4 - Sorting the words, most common first, failed.
 */
int queryFile(Database const db, std::string const &file);

/* DESCRIPTION:
 *
 * Returns a sequence of words that occur most commonly in the database.
 * Requires the queryFile function to be run first.
 * If 'count' is equal to 0, all the words are returned.
 *
 * EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'count' argument is greater than the available number of words.
 *
 * 3 - The 'out' argument is a nullptr.
 */
int getWords(Database const db, std::vector<std::string> *const out,
             std::size_t count = 0);

/* EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'word' argument is not present within the database.
 *
 * 3 - The 'pos' argument is a nullptr.
 */
int getWordPositions(Database const db, std::string const &word,
                     std::vector<std::size_t> *const pos);

/* EXIT STATUS:
 *
 * 0 - The operation was successful.
 *
 * 1 - The 'db' argument is a nullptr.
 *
 * 2 - The 'count' argument is a nullptr.
 */
int getTotalWordCount(Database const db, std::size_t *const count);
} // namespace qy
