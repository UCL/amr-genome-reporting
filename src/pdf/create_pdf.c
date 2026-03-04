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

    pdfioContentTextBegin(page);
    pdfioContentSetTextFont(page, "F1", 14.0);
    pdfioContentTextMoveTo(page, 200.0, 750.0);
    pdfioContentTextShow(page, /*unicode*/false, "CLINICAL GENOME REPORT");
    pdfioContentTextEnd(page);

    pdfioStreamClose(page);
    
    return pdf;
}
