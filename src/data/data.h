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
#ifndef DATA_H
#define DATA_H

#include <stddef.h>

typedef struct report_clinical_header_s {
    const char *label;
    const char *value;
} report_clinical_header_t;

typedef enum report_summary_font_e {
    normal,
    bold,
    italic
} report_summary_font_t;

typedef struct report_summary_segment_s {
    report_summary_font_t font;
    char *text;
} report_summary_segment_t;

typedef struct report_bacterial_typing_profile_s {
    char *gene;
    char *allele;
} report_bacterial_typing_profile_t;

typedef struct report_bacterial_typing_s {
    char *description;
    char *url;
    char *typing;
    char *sequence_type;
    report_bacterial_typing_profile_t *profile;
    size_t profile_size;
    size_t profile_capacity;
} report_bacterial_typing_t;

typedef struct report_determinants_s {
    char *drug;
    char *acquired_gen;
} report_determinants_t;

typedef struct report_clinical_microbiology_s {
    report_summary_segment_t *summary_segments;
    size_t summary_segments_size;
    size_t summary_segments_capacity;
    char *organism;
    report_bacterial_typing_t bacterial_typing;
    report_determinants_t *determinants;
    size_t determinants_size;
    size_t determinants_capacity;
} report_clinical_microbiology_t;

typedef struct report_data_s {
    char *title;
    report_clinical_header_t *clinical_header;
    size_t clinical_header_size;
    size_t clinical_header_capacity;
    report_clinical_microbiology_t clinical_microbiology;
} report_data_t;

void report_data_init(report_data_t *data);

void report_data_free(report_data_t *data);

int report_data_append_header(
    report_data_t *data, const char *label, const char *value);

void report_data_append_clinical_microbiology_summary(
    report_data_t *data, const char *font, const char *text);

void report_data_append_bacterial_typing_profile(
    report_data_t *data, const char *gene, const char *allele);

#endif /* DATA_H */
