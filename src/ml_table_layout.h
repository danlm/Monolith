/* Monolith table layout class.
 * - by Richard W.M. Jones <rich@annexia.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: ml_table_layout.h,v 1.3 2002/11/13 20:46:38 rich Exp $
 */

#ifndef ML_TABLE_LAYOUT_H
#define ML_TABLE_LAYOUT_H

#include <ml_widget.h>

struct ml_table_layout;
typedef struct ml_table_layout *ml_table_layout;

/* Function: new_ml_table_layout - monolith table layout widget
 * Function: ml_table_layout_pack
 * Function: ml_table_layout_add_row
 * Function: ml_table_layout_set_colspan
 * Function: ml_table_layout_set_rowspan
 * Function: ml_table_layout_set_align
 * Function: ml_table_layout_set_valign
 * Function: ml_table_layout_set_class
 * Function: ml_table_layout_set_header
 *
 * The monolith table layout widget is a very powerful
 * layout tool. It is modelled on, and indeed implemented using,
 * HTML <table>s. Table layouts are grids of widgets with
 * a fixed number of rows and columns. Each widget normally
 * occupies a single cell of the table, but widgets may occupy
 * a rectangle of several adjacent cells. All cells in the table
 * are referenced using the row and column number, with row
 * and column numbers starting from zero.
 *
 * Note that when creating forms, it is often better to use
 * the simpler @code{ml_multicol_layout} widget (see
 * @ref{new_ml_multicol_layout(3)}).
 *
 * For single row or single column tables, it is usually better
 * to use @code{ml_horizontal_layout} or @code{ml_vertical_layout}
 * respectively (see @ref{new_ml_horizontal_layout(3)},
 * @ref{new_ml_vertical_layout(3)}).
 *
 * @code{new_ml_table_layout} creates a new table layout widget
 * with @code{rows} cells across and @code{cols} cells down. The
 * cells are numbered @code{0 .. rows-1} across and code{0 .. cols-1}
 * down.
 *
 * The following properties can be changed on tables (see
 * @ref{ml_widget_set_property(3)}):
 *
 * @code{class}: The stylesheet class.
 *
 * @code{ml_table_layout_pack} packs a widget at position
 * @code{(row,col)} within the table. To remove a widget and
 * leave a cell empty, pack a @code{NULL} widget.
 *
 * @code{ml_table_layout_add_row} adds a single row at the end
 * of the table.
 *
 * @code{ml_table_layout_set_colspan} and
 * @code{ml_table_layout_set_rowspan} set the column span
 * and row span for a particular table cell respectively.
 * The col/row-span allow a cell to occupy one or more
 * adjacent cells in the table (any widgets packed in those
 * adjacent cells are silently ignored). The default
 * column and row span for every cell is 1.
 *
 * @code{ml_table_layout_set_align} sets the horizontal
 * alignment for the content of a cell. The possibilities
 * are @code{"left"}, @code{"center"}
 * or @code{"right"}, with the default being left-aligned.
 *
 * @code{ml_table_layout_set_valign} sets the vertical
 * alignment for the content of a cell. The possibilities
 * are @code{"top"}, @code{"middle"}
 * or @code{"bottom"}, with the default being middle.
 *
 * @code{ml_table_layout_set_class} sets the stylesheet class for a
 * cell (the default being no class).
 *
 * @code{ml_table_layout_set_header} sets a boolean flag on a cell. When
 * try, this is a header cell. Otherwise, this is a normal table body cell.
 * This can be used in conjunction with stylesheets on the table.
 *
 * See also: @ref{new_ml_multicol_layout(3)},
 * @ref{new_ml_horizontal_layout(3)},
 * @ref{new_ml_vertical_layout(3)},
 * @ref{new_ml_widget(3)}.
 */
extern ml_table_layout new_ml_table_layout (pool, int rows, int cols);
extern void ml_table_layout_pack (ml_table_layout, ml_widget, int row, int col);
extern void ml_table_layout_add_row (ml_table_layout);
extern void ml_table_layout_set_colspan (ml_table_layout, int row, int col, int colspan);
extern void ml_table_layout_set_rowspan (ml_table_layout, int row, int col, int rowspan);
extern void ml_table_layout_set_align (ml_table_layout, int row, int col, const char *align);
extern void ml_table_layout_set_valign (ml_table_layout, int row, int col, const char *valign);
extern void ml_table_layout_set_class (ml_table_layout, int row, int col, const char *clazz);
extern void ml_table_layout_set_header (ml_table_layout, int row, int col, int is_header);

#endif /* ML_TABLE_LAYOUT_H */
