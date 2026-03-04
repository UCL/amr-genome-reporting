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

bool draw_header(pdfio_stream_t *stream)
{
    const double page_width = 595.28;
    const double page_height = 841.89;

    double rect_x = MARGIN;
    double rect_y = page_height - MARGIN - HEADER_HEIGHT;
    double rect_w = page_width - 2 * MARGIN;
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

    draw_header(page);
    
    pdfioStreamClose(page);
    
    return pdf;
}
