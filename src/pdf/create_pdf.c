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
#include "create_pdf.h"
#include "layout.h"
#include "writer.h"

#include <pdfio.h>
#include <pdfio-content.h>
#include <stdio.h>
#include <sys/types.h>

#define HEADER_HEIGHT 40.0

// Both boxes set in 1/72nd of an inch units
pdfio_rect_t a4_media_box = { 0, 0, PAGE_WIDTH, PAGE_HEIGHT };
// set 0.5'' margins in points
pdfio_rect_t a4_crop_box = { 36.0, 36.0, 559.28, 805.89 };

/*
ssize_t pdfio_output_cb(void *ctx, const void *data, size_t datalen)
{
    (void)ctx;
    (void)data;

    printf("pdfio requested write of %zu bytes\n", datalen);

    return (ssize_t) datalen; // pretend everything was written
}
*/

bool pdfio_error_cb(pdfio_file_t *pdf, const char *message, void *data)
{
    (void)pdf;
    (void)data;
    (void)message;
    return false;
}

bool draw_definition_box(
    pdfio_stream_t *stream,
    double *line_position,
    char *label,
    char *text,
    double font_size)
{
        double rect_x = MARGIN;
    double rect_y = *line_position - HEADER_HEIGHT;
    double rect_w = PAGE_WIDTH - 2 * MARGIN;
    double rect_h = HEADER_HEIGHT;

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, 1.0, 1.0, 1.0); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, 0.0, 0.0, 0.0); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, rect_x, rect_y, rect_w, rect_h);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    // Draw text
    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);
    pdfioContentTextBegin(stream);
    pdfioContentSetTextFont(stream, "I1", font_size);

    // Inset text
    pdfioContentTextMoveTo(stream, rect_x + 12.0, rect_y + rect_h / 2.0 - 6.0);
//    pdfioContentTextMoveTo(stream, 12.0, 36.0);

    pdfioContentTextShow(stream, /*unicode*/false, text);
    pdfioContentTextEnd(stream);

    *line_position = rect_y;
    return true;
}

pdfio_file_t *create_pdf_file(const char *filename, const report_data_t *data)
{
    int error_cbdata = 2;
    pdfio_file_t *pdf = pdfioFileCreate(
        filename,
        "2.0",
        &a4_media_box,
        NULL,
        pdfio_error_cb,
        &error_cbdata);
    // TODO check pdf is not null

    pdfio_obj_t *font = pdfioFileCreateFontObjFromBase(pdf, FONT_NORM);
    pdfio_obj_t *bold = pdfioFileCreateFontObjFromBase(pdf, FONT_BOLD);
    pdfio_obj_t *ital = pdfioFileCreateFontObjFromBase(pdf, FONT_ITAL);
    // TODO check font is not null

    // Create page dictionary with fonts
    pdfio_dict_t *dict = pdfioDictCreate(pdf);
    pdfioPageDictAddFont(dict, FONT_TAG_NORM, font);
    pdfioPageDictAddFont(dict, FONT_TAG_BOLD, bold);
    pdfioPageDictAddFont(dict, FONT_TAG_ITAL, ital);

    // Create page with its content stream
    pdfio_stream_t *page = pdfioFileCreatePage(pdf, dict);

    colour_t colour = { COLOUR_YELLOW };
    
    // Initialise layout context
    layout_ctx_t layout;
    init_layout_ctx(&layout);
    set_font_size(&layout, 14.0);
    cursor_to_newline(&layout);

    draw_banner(page, &layout, colour, "CLINICAL GENOME REPORT");

    set_font_size(&layout, 10.0);
    draw_header_content(
        page,
        &layout,
        data->clinical_header,
        data->clinical_header_size);
    
    set_font_size(&layout, 12.0);
    draw_banner(page, &layout, colour, "Section I: Clinical microbiology");

    set_font_size(&layout, 10.0);
    draw_summary(page, &layout, colour, data->clinical_microbiology.summary_segments, data->clinical_microbiology.summary_segments_size);
//    draw_definition_box(page, &line_position, "Summary", "Klebsiella", 11.0);
    pdfioStreamClose(page);
    
    return pdf;
}
