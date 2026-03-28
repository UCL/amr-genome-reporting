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
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "io/read_text.h"
#include "json/parse_json.h"
#include "mustach/mustach-cjson.h"

int main(int argc, char **argv)
{
    if (argc < 3) {
      fprintf(stderr, "Usage: %s [JSON file] [template]\n", argv[0]);
      return 1;
    }

  const char *filename = argv[1];

  FILE *fp = fopen(filename, "r");
  if (!fp) {
      // fprintf(stderr, "File %s could not be opened", filename);
      perror(filename);
    return 2;
  }

  const char *template = argv[2];
  FILE *tf = fopen(template, "rb");
  if (!tf) {
      perror(template);
      return 2;
  }

  size_t json_len = 0;
  char *json_text = read_text_file(fp, &json_len);
  fclose(fp);
  if (!json_text) {
      fprintf(stderr, "Failed to read input file\n");
      return 3;
  }

  char *template_text = read_template(tf);
  fclose(tf);
  if (!template_text) {
      fprintf(stderr, "Failed to read template\n");
      return 3;
  }

  cJSON *json = cJSON_Parse(json_text);
  if (!json) {
    const char *errp = cJSON_GetErrorPtr();
    if (errp)
      fprintf(stderr, "Error before %s\n", errp);
    free(json_text);
    return 4; // TODO replace with proper error code enum value
  }

  json_status_t status = validate_json_structure(json);
  if (status != OK) {
      fprintf(stderr, "JSON file '%s' is invalid\n", filename);
      return 5; // TODO replace with proper error code enum value
  }

  char *output;
  size_t out_len = 0;

  int res = mustach_cJSON_mem(
      template_text,
      0,
      json,
      Mustach_With_AllExtensions,
      &output,
      &out_len);

  if (res == MUSTACH_OK)
      fprintf(stdout, "%s\n", output);

  //cJSON_Delete(json);
  
  free(json_text);
  free(template_text);
    
  // 1 = true, 0 = false
  return 0;
}
