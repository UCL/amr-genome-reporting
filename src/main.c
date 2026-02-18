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
#include <stddef.h>
#include <stdio.h>
#include <cJSON.h>

#include "io/read_text.h"
#include "json/parse_json.h"

int main(int argc, char **argv)
{
  if (argc < 2)
    fprintf(stderr, "Usage: amr_gen_reporting [JSON file]\n");

  FILE *fp;
  char *filename = argv[1];
  fp = fopen(filename, "r");

  if (fp == NULL)
    fprintf(stderr, "File %s could not be opened", filename);

  size_t *out_len = 0;

  const char *json_text = read_text_file(fp, out_len);

  const cJSON *json = cJSON_Parse(json_text);
  if (json == NULL) {
    const char *errp = cJSON_GetErrorPtr();
    if (errp != NULL)
      fprintf(stderr, "Error before %s\n", errp);
    return -1; // TODO replace with proper error code enum value
  }

  json_status_t status = validate_json_structure(json);
  if (status != OK)
    return -2; // TODO replace with proper error code enum value

  return 0;
}
