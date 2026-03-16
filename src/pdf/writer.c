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
#include "layout.h"
#include "writer.h"
#include "create_pdf.h"

#include <pdfio.h>
#include <pdfio-content.h>

void draw_banner(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    colour_t bgcol,
    char *text)
{
    double rect_x = ctx->margin;
    double rect_y = ctx->cursor_y - 3 * ctx->line_height;
    double rect_w = PAGE_WIDTH - 2 * ctx->margin;
    double rect_h = 3 * ctx->line_height;

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, bgcol.r, bgcol.g, bgcol.b); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_BLACK); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, rect_x, rect_y, rect_w, rect_h);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    // Draw text
    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);
    pdfioContentTextBegin(stream);
    pdfioContentSetTextFont(stream, FONT_TAG_BOLD, ctx->font_size);

    // Inset text
    pdfioContentTextMoveTo(stream, rect_x + 12.0, rect_y + rect_h / 2.0 - 6.0);

    pdfioContentTextShow(stream, /*unicode*/false, text);
    pdfioContentTextEnd(stream);
    pdfioContentPathClose(stream);

    ctx->cursor_y = rect_y;
}

void draw_header_content(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    report_clinical_header_t *header,
    size_t header_size)
{
    char *label;
    char *value;
    char content[512];
    float line_pos = ctx->cursor_y - 10.0;

    if (!header) {
        fprintf(stderr, "Header is null\n");
        return;
    }
    
    for (size_t i = 0; i < header_size; i++)
    {
        label = header[i].label;
        value = header[i].value;
        snprintf(content, sizeof(content), "%s: %s", label, value);

        pdfioContentTextBegin(stream);
        pdfioContentSetTextFont(stream, FONT_TAG_NORM, 10.0);
        // Test if index is even/odd
        if ((i & 1) == 0) // first column
        {
            line_pos = line_pos - 12.0;
            pdfioContentTextMoveTo(stream, MARGIN + 12.0, line_pos);
        } else // second column
        {
            pdfioContentTextMoveTo(stream, PAGE_WIDTH / 2.0 + 12.0, line_pos);
        }
        pdfioContentTextShow(stream, false, content);
        pdfioContentTextEnd(stream);
    }

    ctx->cursor_y = line_pos - 18.0;

    (void)label;
    (void)value;
    (void)content;
}
