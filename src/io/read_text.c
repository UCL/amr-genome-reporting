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
#include "read_text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *read_text_file(FILE *fp, size_t *out_len)
{
  // Dynamically sized output buffer
  char *buffer = NULL;

  // Allocated capacity (bytes)
  size_t bufcap = 0;

  // Number of bytes written (excluding '\0')
  size_t length = 0;

  // Arguments required by getline()
  char *line = NULL;
  size_t linecap = 0;

  while(1) {
    ssize_t n = getline(&line, &linecap, fp);
    if (n < 0) {
      if (feof(fp))
        break;
      goto error;
    }

    // Add one (+1) to reserve space for final '\0'
    if (length + (size_t)n + 1 > bufcap) {
      size_t cap = bufcap ? bufcap * 2 : INITIAL_PAGE_SIZE;

      while (cap < length + (size_t)n + 1)
        cap *= 2;

      char *tmp = realloc(buffer, cap);
      if (!tmp)
        goto error;

      buffer = tmp;
      bufcap = cap;
    }

    memcpy(buffer + length, line, (size_t)n);
    length += (size_t)n;
  }

  if (!buffer) {
    buffer = calloc(1, 1);
    if (!buffer)
      goto error;
  } else {
    buffer[length] = '\0';
  }

  free(line);

  if (out_len)
    *out_len = length;

  return buffer;

 error:
  free(line);
  free(buffer);
  return NULL;
}

char *read_template(FILE *fp)
{
    if (!fp)
        return NULL;

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = malloc(file_size + 1);
    fread(data, file_size, 1, fp);

    data[file_size] = '\0';

    return data;
}
