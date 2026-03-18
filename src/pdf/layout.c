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

void init_layout_ctx(layout_ctx_t *ctx)
{
    ctx->margin = MARGIN;
    ctx->cursor_x = MARGIN;
    ctx->cursor_y = PAGE_HEIGHT - 2 * MARGIN;
    ctx->font_size = 10.0;
    ctx->line_height = 12.0;
    ctx->line_padding = 0;
    ctx->line_width = PAGE_WIDTH - 2 * MARGIN;
}

void set_font_size(layout_ctx_t *ctx, double size)
{
    ctx->font_size = size;
    ctx->line_height = size * 1.2;
    ctx->line_padding = 2 * size;
}

void cursor_to_newline(layout_ctx_t *ctx)
{
    ctx->cursor_x = ctx->margin;
    ctx->cursor_y -= ctx->line_height;
}
