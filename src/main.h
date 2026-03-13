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
#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

typedef struct report_clinical_header_s {
    char *label;
    char *value;
} report_clinical_header_t;

typedef struct report_data_s {
    char *title;
    report_clinical_header_t *clinical_header;
    size_t clinical_header_size;
} report_data_t;

#endif /* MAIN_H */
