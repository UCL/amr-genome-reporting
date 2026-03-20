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
#include <ctype.h>

#define MAX_LINE_LENGTH 512

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

static const char *next_word_const(const char **text, size_t *len)
{
    if (**text == '\0')
        return NULL;

    while (isspace((unsigned char)**text))
        (*text)++;

    const char *start = *text;

    while (**text && !isspace((unsigned char)**text))
        (*text)++;

    *len = *text - start;

    return start;    
}

size_t wrap_text(
    pdfio_stream_t *stream,
    pdfio_dict_t *dict,
    const char *font,
    double font_size,
    double max_width,
    char *text,
    char lines[][MAX_LINE_LENGTH],
    size_t max_lines)
{
    size_t line_count = 0;
    double line_width = 0;

    pdfio_obj_t *font_obj = pdfioDictGetObj(dict, font);
    if (!font_obj)
    {
        fprintf(stderr, "Font %s not found\n", font);
        return 0;
    }

    pdfioContentSetTextFont(stream, font, font_size);

    char current_line[MAX_LINE_LENGTH] = {0};

    char *buffer = strdup(text);
    char *cursor = buffer;
    char *word;

    double space_width = pdfioContentTextMeasure(font_obj, " ", font_size);

    while ((word = next_word(&cursor)) != NULL)
    {
        double word_width = pdfioContentTextMeasure(font_obj, word, font_size);

        if (word_width > max_width)
        {
            // TODO: Truncate
        }

        if (line_width + word_width > max_width)
        {
            strncpy(lines[line_count++], current_line, MAX_LINE_LENGTH);

            if (line_count >= max_lines)
                break;

            current_line[0] = '\0';
            line_width = 0;
        }

        if (current_line[0] != '\0')
        {
            strncat(current_line, " ", MAX_LINE_LENGTH - strlen(current_line) - 1);
            line_width += space_width;
        }

        strncat(current_line, word, sizeof(current_line) - strlen(current_line) - 1);
        line_width += word_width;
    }

    if (current_line[0] != '\0' && line_count < max_lines)
        strncpy(lines[line_count++], current_line, MAX_LINE_LENGTH);

    free(buffer);
    return line_count;
}

static const char *font_to_pdf_tag(report_summary_font_t font)
{
    switch(font)
    {
    case normal: return "F1";
    case bold: return FONT_TAG_BOLD;
    case italic: return "I1";
    default: return FONT_TAG_NORM;
    }
}

static const pdfio_obj_t *get_font_obj(font_cache_t *cache, report_summary_font_t font)
{
    switch(font)
    {
    case normal: return cache->normal;
    case bold: return cache->bold;
    case italic: return cache->italic;
    }
}

void render_wrapped_segments(
    pdfio_stream_t *stream,
    font_cache_t *font_cache,
    report_summary_segment_t *segments,
    size_t segment_count,
    layout_ctx_t *ctx)
{
    double start_x = ctx->cursor_x;
    double x = ctx->cursor_x;

    double space_width = pdfioContentTextMeasure(font_cache->normal, " ", ctx->font_size);

    const char *label = "Summary: ";
    pdfioContentSetTextFont(stream, FONT_TAG_BOLD, ctx->font_size);
    pdfioContentTextBegin(stream);
    pdfioContentTextMoveTo(stream, x, ctx->cursor_y);
    pdfioContentTextShow(stream, false, label);
    pdfioContentTextEnd(stream);

    double label_width = pdfioContentTextMeasure(font_cache->normal, label, ctx->font_size);
    x += label_width + space_width;

    // Iterate over segments
    for (size_t i = 0; i < segment_count; i++)
    {
        report_summary_segment_t *seg = &segments[i];

        const char *font_tag = font_to_pdf_tag(seg->font);

        const pdfio_obj_t *font_obj = get_font_obj(font_cache, seg->font);
        if (!font_obj)
            continue;

        pdfioContentSetTextFont(stream, font_tag, ctx->font_size);

        const char *cursor = seg->text;
        const char *word;
        size_t word_len;

        // Iterate over each word in the segment
        while ((word = next_word_const(&cursor, &word_len)) != NULL)
        {
            char buffer[256];
            if (word_len >= sizeof(buffer))
                word_len = sizeof(buffer) - 1; // We truncate

            memcpy(buffer, word, word_len);
            buffer[word_len] = '\0';

            double word_width = pdfioContentTextMeasure(font_cache->normal, buffer, ctx->font_size);
            if (x + word_width > start_x + ctx->line_width)
            {
                cursor_to_newline(ctx);
                x = start_x;
            }
            // Move before drawing
            pdfioContentTextBegin(stream);
            pdfioContentTextMoveTo(stream, x, ctx->cursor_y);
            pdfioContentTextShow(stream, false, buffer);
            pdfioContentTextEnd(stream);

            x += word_width;

            // Add space if needed
            if (*cursor != '\0')
            {
                if (x + space_width > start_x + ctx->line_width)
                {
                    cursor_to_newline(ctx);
                    x = start_x;
                }
                else
                {
                    x += space_width;
                }
            }
        }
        x += space_width;
    }

    cursor_to_newline(ctx);
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
    pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_PLATINUM); // Stroke

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
    const char *label;
    const char *value;
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
    font_cache_t *font_cache,
    layout_ctx_t *ctx,
    colour_t bgcol,
    report_summary_segment_t *summary_segments,
    size_t summary_segments_size)
{
    cursor_to_newline(ctx);
    rect_coordinates_t coord = calculate_coordinates(ctx);

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, bgcol.r, bgcol.g, bgcol.b); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_PLATINUM); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, coord.x, coord.y, coord.w, coord.h);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);    
    cursor_to_newline(ctx);

    render_wrapped_segments(
        stream, font_cache, summary_segments, summary_segments_size, ctx);

}

void draw_organism(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    font_cache_t *font_cache,
    char *organism)
{
    const char *label = "Organism: ";
    double label_width = pdfioContentTextMeasure(font_cache->normal, label, ctx->font_size);
    
    cursor_to_newline(ctx);
    cursor_to_newline(ctx);

    pdfioContentSetTextFont(stream, FONT_TAG_BOLD, ctx->font_size);    
    pdfioContentTextBegin(stream);
    pdfioContentTextMoveTo(stream, ctx->cursor_x, ctx->cursor_y);
    pdfioContentTextShow(stream, false, label);
    pdfioContentTextEnd(stream);

    pdfioContentSetTextFont(stream, FONT_TAG_ITAL, ctx->font_size);    
    pdfioContentTextBegin(stream);
    pdfioContentTextMoveTo(stream, ctx->cursor_x + label_width + ctx->font_size / 2, ctx->cursor_y);
    pdfioContentTextShow(stream, false, organism);
    pdfioContentTextEnd(stream);

    cursor_to_newline(ctx);
}

void write_string(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    const char *text,
    const char *font_tag)
{
    pdfioContentSetTextFont(stream, font_tag, ctx->font_size);    
    pdfioContentTextBegin(stream);
    pdfioContentTextMoveTo(stream, ctx->cursor_x, ctx->cursor_y);
    pdfioContentTextShow(stream, false, text);
    pdfioContentTextEnd(stream);
}

void draw_line(
    pdfio_stream_t *stream,
    double from_x,
    double from_y,
    double to_x,
    double to_y,
    colour_t colour)
{
    pdfioContentSetStrokeColorDeviceRGB(stream, colour.r, colour.g, colour.b);
    pdfioContentPathMoveTo(stream, from_x, from_y);
    pdfioContentPathLineTo(stream, to_x, to_y);
    pdfioContentStroke(stream);
}
    

void draw_bacterial_typing(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    font_cache_t *font_cache,
    report_bacterial_typing_t bacterial_typing)
{
    const char *label = "Bacterial typing: ";
    double label_width = pdfioContentTextMeasure(font_cache->normal, label, ctx->font_size);

    cursor_to_newline(ctx);
    write_string(stream, ctx, label, FONT_TAG_BOLD);

    cursor_to_coordinates(ctx, ctx->cursor_x + label_width + ctx->font_size, ctx->cursor_y);
    write_string(stream, ctx, bacterial_typing.description, FONT_TAG_NORM); // TODO: this should be wrapped
    cursor_to_newline(ctx);
    write_string(stream, ctx, bacterial_typing.url, FONT_TAG_NORM);
    cursor_to_newline(ctx);
    cursor_to_newline(ctx);
    write_string(stream, ctx, bacterial_typing.typing, FONT_TAG_NORM);

    const size_t seq_type_len = strlen(bacterial_typing.sequence_type);
    char seq_type[50] = "Sequence type (ST): ";
    strncat(seq_type, bacterial_typing.sequence_type, seq_type_len);
    cursor_to_newline(ctx);
    write_string(stream, ctx, seq_type, FONT_TAG_NORM);
    // We go lines back to write the second column with the profile info
    const char *profile_label = "Profile  ";
    double profile_label_width = pdfioContentTextMeasure(font_cache->normal, profile_label, ctx->font_size);
    cursor_to_coordinates(ctx, PAGE_WIDTH / 2.0, ctx->cursor_y + ctx->line_height / 2.0);
    write_string(stream, ctx, profile_label, FONT_TAG_NORM);
    colour_t line_col = { COLOUR_PLATINUM };
    double line_from_x = PAGE_WIDTH / 2.0 + profile_label_width;
    double line_to_x = PAGE_WIDTH - ctx->margin;
    cursor_to_coordinates(ctx, line_from_x, ctx->cursor_y + ctx->line_height * 1.2);
    draw_line(stream, ctx->cursor_x, ctx->cursor_y, line_to_x, ctx->cursor_y, line_col);
    set_font_size(ctx, 7.0);
    cursor_to_coordinates(ctx, line_from_x, ctx->cursor_y - ctx->line_height);
    write_string(stream, ctx, "PLACEHOLDER", FONT_TAG_NORM);
    cursor_to_coordinates(ctx, line_from_x, ctx->cursor_y - ctx->line_height / 2.5);
    draw_line(stream, line_from_x, ctx->cursor_y, line_to_x, ctx->cursor_y, line_col);
    cursor_to_coordinates(ctx, line_from_x, ctx->cursor_y - ctx->line_height);
    write_string(stream, ctx, "PLACEHOLDER", FONT_TAG_NORM);
    cursor_to_coordinates(ctx, line_from_x, ctx->cursor_y - ctx->line_height / 2.5);
    draw_line(stream, line_from_x, ctx->cursor_y, line_to_x, ctx->cursor_y, line_col);
    cursor_to_newline(ctx);
    set_font_size(ctx, 10.0);
}

void draw_determinants(
    pdfio_stream_t *stream,
    layout_ctx_t *ctx,
    font_cache_t *font_cache,
    report_determinants_t *determinants,
    size_t determinants_size)
{
    cursor_to_newline(ctx);
    cursor_to_newline(ctx);

    const char *label = "Antibiotic resistance determinants: ";
    double label_width = pdfioContentTextMeasure(font_cache->normal, label, ctx->font_size) + ctx->line_height * 1.2;    
    write_string(stream, ctx, label, FONT_TAG_BOLD);
    cursor_to_coordinates(ctx, ctx->cursor_x + label_width, ctx->cursor_y);
    write_string(stream, ctx, "Genetically determined resistance mechanisms have been identified.", FONT_TAG_NORM);
    cursor_to_newline(ctx);
    write_string(stream, ctx, "Genetic variants (homologs) are highlighted *. \"None found\" means no resistance mechanisms have been identified", FONT_TAG_NORM);
    cursor_to_newline(ctx);
    write_string(stream, ctx, "in the antimicrobial resistance databases that exist at the time of reporting. It is best practice to review these with the", FONT_TAG_NORM);
    cursor_to_newline(ctx);
    write_string(stream, ctx, "phenotypic antibody susceptibility profile https://github.com/klebgenomics/Kleborate", FONT_TAG_NORM);
    cursor_to_newline(ctx);
    
    cursor_to_newline(ctx);
    set_font_size(ctx, 8.0);    
    cursor_to_coordinates(ctx, ctx->margin + 12.0, ctx->cursor_y);
    write_string(stream, ctx, "Drug class", FONT_TAG_BOLD);
    cursor_to_coordinates(ctx, PAGE_WIDTH / 2.0 + 12.0, ctx->cursor_y);
    write_string(stream, ctx, "Acquired genotypes", FONT_TAG_BOLD);

    const char *drug;
    const char *acq_gen;
    for (size_t i = 0; i < determinants_size; i++)
    {
        drug = determinants[i].drug;
        acq_gen = determinants[i].acquired_gen;
        
        cursor_to_newline(ctx);

        pdfioContentSetStrokeColorDeviceRGB(stream, COLOUR_GREY); // Stroke
        pdfioContentPathMoveTo(stream, ctx->margin, ctx->cursor_y + ctx->line_height / 2.5);
        pdfioContentPathLineTo(stream, PAGE_WIDTH - ctx->margin, ctx->cursor_y + ctx->line_height / 2.5);
        pdfioContentStroke(stream);
        cursor_to_newline(ctx);
        
        cursor_to_coordinates(ctx, ctx->margin + 12.0, ctx->cursor_y);
        write_string(stream, ctx, drug, FONT_TAG_NORM);

        cursor_to_coordinates(ctx, PAGE_WIDTH / 2.0 + 12.0, ctx->cursor_y);
        write_string(stream, ctx, acq_gen, FONT_TAG_NORM);
    }
    (void)drug;
    (void)acq_gen;
}
