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
#ifndef LAYOUT_H
#define LAYOUT_H

#define PAGE_WIDTH 595.28
#define PAGE_HEIGHT 841.89
#define MARGIN 36.0

#define FONT_NORM "Helvetica"
#define FONT_BOLD "Helvetica-Bold"
#define FONT_ITAL "Helvetica-Italic"
#define FONT_TAG_NORM "F1"
#define FONT_TAG_BOLD "B1"
#define FONT_TAG_ITAL "I1"

#define RGB(r,g,b) (r)/255.0, (g)/255.0, (b)/255.0
#define COLOUR_YELLOW RGB(255,255,153)
#define COLOUR_BLACK RGB(0,0,0)
#define COLOUR_WHITE RGB(255,255,255)
#define COLOUR_GREY RGB(192,192,192)

typedef struct layout_ctx_s {
    double margin;
    double cursor_x;
    double cursor_y;
    double font_size;
    double line_height;
    double line_width;
    double line_padding;
} layout_ctx_t;

typedef struct colour_s {
    double r;
    double g;
    double b;
} colour_t;

void init_layout_ctx(layout_ctx_t *ctx);

void set_font_size(layout_ctx_t *ctx, double size);

void cursor_to_newline(layout_ctx_t *ctx);

void cursor_to_coordinates(layout_ctx_t *ctx, double x, double y);

#endif /* LAYOUT_H */
