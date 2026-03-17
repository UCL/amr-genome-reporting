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
#include <string.h>

rect_coordinates_t calculate_coordinates(layout_ctx_t *ctx)
{
    rect_coordinates_t coordinates = {
        .x = ctx->margin,
        .y = ctx->cursor_y - 3 * ctx->line_height,
        .w = PAGE_WIDTH - 2 * ctx->margin,
        .h = 3 * ctx->line_height
    };
    return coordinates;
}

static char *next_word(char **text)
{
    if (**text == '\0')
        return NULL;

    char *start = *text;

    while (**text && **text != ' ')
        (*text)++;

    if (**text)
        *(*text)++ = '\0';

    return start;
}

size_t wrap_text(
    pdfio_stream_t *stream,
    pdfio_dict_t *dict,
    const char *font,
    double font_size,
    double max_width,
    char *text,
    char lines[][8],
    size_t max_lines)
{
    size_t line_count = 0;
    double line_width = 0;

    pdfioContentSetTextFont(stream, font, font_size);

    char current_line[512] = {0};

    char *cursor = text;
    char *word;

    pdfio_obj_t *font_obj = pdfioDictGetObj(dict, font);

    while ((word = next_word(&cursor)) != NULL)
    {
        double word_width = pdfioContentTextMeasure(font_obj, word, font_size);

        if (line_width + word_width > max_width)
        {
            strcpy(lines[line_count++], current_line);

            if (line_count >= max_lines)
                break;

            current_line[0] = '\0';
            line_width = 0;
        }

        if (current_line[0] != '\0')
        {
            strcat(current_line, " ");
            line_width += pdfioContentTextMeasure(font_obj, " ", font_size);
        }

        strcat(current_line, word);
        line_width += word_width;
    }

    if (current_line[0] != '\0' && line_count < max_lines)
        strcpy(lines[line_count++], current_line);

    return line_count;
}

void draw_banner(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    colour_t bgcol,
    char *text)
{
    /*
    double rect_x = ctx->margin;
    double rect_y = ctx->cursor_y - 3 * ctx->line_height;
    double rect_w = PAGE_WIDTH - 2 * ctx->margin;
    double rect_h = 3 * ctx->line_height;
    */
    rect_coordinates_t coord = calculate_coordinates(ctx);

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, bgcol.r, bgcol.g, bgcol.b); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_BLACK); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, coord.x, coord.y, coord.w, coord.h);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    // Draw text
    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);
    pdfioContentTextBegin(stream);
    pdfioContentSetTextFont(stream, FONT_TAG_BOLD, ctx->font_size);

    // Inset text
    pdfioContentTextMoveTo(stream, coord.x + 12.0, coord.y + coord.h / 2.0 - 6.0);

    pdfioContentTextShow(stream, /*unicode*/false, text);
    pdfioContentTextEnd(stream);
    pdfioContentPathClose(stream);

    ctx->cursor_y = coord.y;
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

void draw_summary(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    colour_t bgcol,
    report_summary_segment_t *summary_segments,
    size_t summary_segments_size)
{
    cursor_to_newline(ctx);
    rect_coordinates_t coord = calculate_coordinates(ctx);

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, bgcol.r, bgcol.g, bgcol.b); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_BLACK); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, coord.x, coord.y, coord.w, coord.h);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    report_summary_font_t *font;
    char *text;
    char *font_tag;
    report_summary_font_t font_type;

    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);    
    pdfioContentTextBegin(stream);
    cursor_to_newline(ctx);
    for (size_t i = 0; i < summary_segments_size; i++)
    {
        font = summary_segments[i].font;
        text = summary_segments[i].text;

        if (font == normal) {
            pdfioContentSetTextFont(stream, FONT_TAG_NORM, ctx->font_size);
        } else if (font == italic) {
            pdfioContentSetTextFont(stream, FONT_TAG_ITAL, ctx->font_size);
        } else if (font == bold) {
            pdfioContentSetTextFont(stream, FONT_TAG_BOLD, ctx->font_size);
        }
            
        pdfioContentTextMoveTo(stream, ctx->cursor_x, ctx->cursor_y);
        pdfioContentTextShow(stream, false, text);
        cursor_to_newline(ctx);
    }
    pdfioContentTextEnd(stream);
}
