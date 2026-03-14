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

void report_data_init(report_data_t *data)
{
    data->title = NULL;
    data->clinical_header = NULL;
    data->clinical_header_size = 0;
    data->clinical_header_capacity = 0;
}

void report_data_free(report_data_t *data)
{
    free(data->clinical_header);
    data->clinical_header = NULL;
    data->clinical_header_size = 0;
    data->clinical_header_capacity = 0;
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
