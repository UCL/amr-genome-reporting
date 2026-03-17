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
#ifndef WRITER_H
#define WRITER_H

#include "layout.h"
#include "create_pdf.h"

#include <pdfio.h>

typedef struct rect_coordinates_s {
    double x;
    double y;
    double w;
    double h;
} rect_coordinates_t;

void draw_banner(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    colour_t bgcol,
    char *text);

void draw_header_content(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    report_clinical_header_t *header,
    size_t header_size);

void draw_summary(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    colour_t bgcol,
    report_summary_segment_t *summary_segments,
    size_t summary_segments_size);

#endif /* WRITER_H */
