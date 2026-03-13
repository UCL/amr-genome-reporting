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

#include <pdfio.h>
#include <pdfio-content.h>
#include <stdio.h>
#include <sys/types.h>

#define PAGE_WIDTH 595.28
#define PAGE_HEIGHT 841.89
#define MARGIN 36.0
#define HEADER_HEIGHT 50.0

// Both boxes set in 1/72nd of an inch units
pdfio_rect_t a4_media_box = { 0, 0, 595.28, 841.89 };
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

bool draw_header_banner(pdfio_stream_t *stream, double *line_position)
{
    double rect_x = MARGIN;
    double rect_y = PAGE_HEIGHT - MARGIN - HEADER_HEIGHT;
    double rect_w = PAGE_WIDTH - 2 * MARGIN;
    double rect_h = HEADER_HEIGHT;

    // Colours
    pdfioContentSetFillColorDeviceRGB(stream, 1.0, 1.0, 153.0/255.0); // Fill
    pdfioContentSetStrokeColorDeviceRGB(stream, 0.0, 0.0, 0.0); // Stroke

    // Draw rectangle
    pdfioContentPathRect(stream, rect_x, rect_y, rect_w, rect_y);

    // Fill and stroke
    pdfioContentFillAndStroke(stream, false);

    // Draw text
    pdfioContentSetFillColorDeviceGray(stream, 0.0);
    pdfioContentSetStrokeColorGray(stream, 0.0);
    pdfioContentTextBegin(stream);
    pdfioContentSetTextFont(stream, "F1", 14.0);

    // Inset text
    pdfioContentTextMoveTo(stream, rect_x + 12.0, rect_y + rect_h / 2.0 - 6.0);
//    pdfioContentTextMoveTo(stream, 12.0, 36.0);

    pdfioContentTextShow(stream, /*unicode*/false, "CLINICAL GENOME REPORT");
    pdfioContentTextEnd(stream);

    *line_position = rect_y;
    return true;
}

bool draw_header_content(pdfio_stream_t *stream, report_clinical_header_t *header, size_t header_size, double *line_position)
{
    char *label;
    char *value;
    char content[256];
    float line_pos = *line_position - 18.0;
    
    for (size_t i = 0; i < header_size; i++)
    {
        label = header[i].label;
        value = header[i].value;
        snprintf(content, sizeof(content), "%s: %s", label, value);

        pdfioContentTextBegin(stream);
        pdfioContentSetTextFont(stream, "F1", 10.0);
        // Test if index is even/odd
        if ((i & 1) == 0) // first column
        {
            line_pos = line_pos - 8.0 * i;
            pdfioContentTextMoveTo(stream, MARGIN + 12.0, line_pos);
        } else // second column
        {
            pdfioContentTextMoveTo(stream, PAGE_WIDTH / 2.0 + 12.0, line_pos);
        }
        pdfioContentTextShow(stream, false, content);
        pdfioContentTextEnd(stream);
    }

    *line_position = line_pos - 18.0;

    (void)label;
    (void)value;
    (void)content;

    return true;
}

pdfio_file_t *create_pdf_file(const char *filename, const report_data_t data)
{
    (void)data;
    
    int error_cbdata = 2;
    pdfio_file_t *pdf = pdfioFileCreate(
        filename,
        "2.0",
        &a4_media_box,
        &a4_crop_box,
        pdfio_error_cb,
        &error_cbdata);
    // TODO check pdf is not null

    pdfio_obj_t *font = pdfioFileCreateFontObjFromBase(pdf, "Helvetica");
    // TODO check font is not null

    // Create page dictionary with font
    pdfio_dict_t *dict = pdfioDictCreate(pdf);
    pdfioPageDictAddFont(dict, "F1", font);

    // Create page with its content stream
    pdfio_stream_t *page = pdfioFileCreatePage(pdf, dict);

    double line_position = MARGIN;

    draw_header_banner(page, &line_position);
    draw_header_content(
        page,
        data.clinical_header,
        data.clinical_header_size,
        &line_position);
    
    pdfioStreamClose(page);
    
    return pdf;
}
