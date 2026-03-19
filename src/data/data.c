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
#include "data.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void report_data_init(report_data_t *data)
{
    data->title = NULL;
    data->clinical_header = NULL;
    data->clinical_header_size = 0;
    data->clinical_header_capacity = 0;
    data->clinical_microbiology.organism = NULL;
    data->clinical_microbiology.summary_segments = NULL;
    data->clinical_microbiology.summary_segments_size = 0;
    data->clinical_microbiology.summary_segments_capacity = 0;
    data->clinical_microbiology.bacterial_typing.profile = NULL;
    data->clinical_microbiology.bacterial_typing.profile_size = 0;
    data->clinical_microbiology.bacterial_typing.profile_capacity = 0;
    data->clinical_microbiology.bacterial_typing.description = NULL;
    data->clinical_microbiology.bacterial_typing.url = NULL;
    data->clinical_microbiology.bacterial_typing.typing = NULL;
    data->clinical_microbiology.bacterial_typing.sequence_type = NULL;
    data->clinical_microbiology.determinants = NULL;
    data->clinical_microbiology.determinants_size = 0;
    data->clinical_microbiology.determinants_capacity = 0;
}

void report_data_free(report_data_t *data)
{
    free(data->clinical_header);
    data->clinical_header = NULL;
    data->clinical_header_size = 0;
    data->clinical_header_capacity = 0;
    free(data->clinical_microbiology.bacterial_typing.profile);
    data->clinical_microbiology.bacterial_typing.profile = NULL;
    data->clinical_microbiology.bacterial_typing.profile_size = 0;
    data->clinical_microbiology.bacterial_typing.profile_capacity = 0;
    free(data->clinical_microbiology.determinants);
    data->clinical_microbiology.determinants = NULL;
    data->clinical_microbiology.determinants_size = 0;
    data->clinical_microbiology.determinants_capacity = 0;
}

static report_summary_font_t parse_font(const char *font)
{
    if (strcmp(font, "normal") == 0)
        return normal;
    if (strcmp(font, "bold") == 0)
        return bold;
    if (strcmp(font, "italic") == 0)
        return italic;

    return normal; // fallback
}

int report_data_append_header(
    report_data_t *data,
    const char *label,
    const char *value)
{
    if (data->clinical_header_size == data->clinical_header_capacity)
    {
        size_t capacity = data->clinical_header_capacity == 0
            ? 8
            : data->clinical_header_capacity * 2;

        report_clinical_header_t *tmp = realloc(
            data->clinical_header,
            capacity * sizeof(report_clinical_header_t));
        if (!tmp)
            return 0;

        data->clinical_header = tmp;
        data->clinical_header_capacity = capacity;
    }

    report_clinical_header_t *item = &data->clinical_header[
        data->clinical_header_size++];
    item->label = label;
    item->value = value;

    return 1;
}

void report_data_append_clinical_microbiology_summary(
    report_data_t *data,
    const char *font,
    const char *text)
{
    if (data->clinical_microbiology.summary_segments_size == data->clinical_microbiology.summary_segments_capacity)
    {
        size_t capacity = data->clinical_microbiology.summary_segments_capacity == 0
            ? 8
            : data->clinical_microbiology.summary_segments_capacity * 2;

        report_summary_segment_t *tmp = realloc(
            data->clinical_microbiology.summary_segments,
            capacity * sizeof(report_summary_segment_t));
        if (!tmp)
            return;

        data->clinical_microbiology.summary_segments = tmp;
        data->clinical_microbiology.summary_segments_capacity = capacity;
    }

    report_summary_segment_t *segment = &data->clinical_microbiology.summary_segments[
        data->clinical_microbiology.summary_segments_size++];

    segment->font = parse_font(font);
    segment->text = strdup(text);
}

void report_data_append_bacterial_typing_profile(
    report_data_t *data,
    const char *gene,
    const char *allele)
{
    if (data->clinical_microbiology.bacterial_typing.profile_size == data->clinical_microbiology.bacterial_typing.profile_capacity)
    {
        size_t capacity = data->clinical_microbiology.bacterial_typing.profile_capacity == 0
            ? 8
            : data->clinical_microbiology.bacterial_typing.profile_capacity * 2;

        report_bacterial_typing_profile_t *tmp = realloc(
            data->clinical_microbiology.bacterial_typing.profile,
            capacity * sizeof(report_bacterial_typing_profile_t));
        if (!tmp)
            return;

        data->clinical_microbiology.bacterial_typing.profile = tmp;
        data->clinical_microbiology.bacterial_typing.profile_capacity = capacity;
    }

    report_bacterial_typing_profile_t *profile = &data->clinical_microbiology.bacterial_typing.profile[
        data->clinical_microbiology.bacterial_typing.profile_size++];

    profile->gene = strdup(gene);
    profile->allele = strdup(allele);
}

void report_data_append_determinants(
    report_data_t *data,
    const char *drug,
    const char *acquired_gen)
{
    if (data->clinical_microbiology.determinants_size == data->clinical_microbiology.determinants_capacity)
    {
        size_t capacity = data->clinical_microbiology.determinants_capacity == 0
            ? 8
            : data->clinical_microbiology.determinants_capacity * 2;

        report_determinants_t *tmp = realloc(
            data->clinical_microbiology.determinants,
            capacity * sizeof(report_determinants_t));
        if (!tmp)
            return;

        data->clinical_microbiology.determinants = tmp;
        data->clinical_microbiology.determinants_capacity = capacity;
    }

    report_determinants_t *determinants = &data->clinical_microbiology.determinants[
        data->clinical_microbiology.determinants_size++];

    determinants->drug = drug;
    determinants->acquired_gen = acquired_gen;
}
