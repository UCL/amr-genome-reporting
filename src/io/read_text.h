/*
  AMR Genome Reporting CLI
  Copyright (C) 2026  University College London.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef READ_TEXT_H
#define READ_TEXT_H

#include <stdio.h>
#include <stddef.h>

#define INITIAL_PAGE_SIZE 4096;

/**
 * @brief Read contents of a text file stream.
 *
 * Reads data from a given FILE stream and returns a dynamically
 * allocated, \0 terminated string.
 *
 * Caller must be responsible to call free() on the returned string.
 *
 * @param fp Open FILE pointer positioned at the start of the input.
 * @param out_len Output parameter receiving the length in bytes
 * (excluding \0)
 * @return Pointer to the heap-allocated buffer. NULL on error.
 */
char *read_text_file(FILE *fp, size_t *out_len);

#endif /* READ_TEXT_H */
