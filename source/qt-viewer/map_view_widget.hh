#ifndef MAP_VIEW_WIDGET_HH
#define MAP_VIEW_WIDGET_HH


// porting to gcc 3.1 -- k
// i'd _really rather not use "using namespace std" in a header file,
// but it's required by our nested #includes.  which we should unnest...
// except that some templates and inlines require nesting?  what to do?
// -- K.

#ifndef MAP_VIEW_CELL_ENTRY_HH
#include "map_view_cell_entry.hh"
#endif

#include <qscrollview.h>

#ifndef VECTOR_STL
#include <vector>
#define VECTOR_STL
#endif

class QPoint;
class QPainter;
class QPixmap;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

class avd_MissionControl;
class ColorScaleWidget;

class MapViewWidget : public QScrollView
{
  Q_OBJECT
private:
  avd_MissionControl *m_mission_control;
  QPixmap *m_pixmap;

  int m_world_x;        // number of cells along x-axis
  int m_world_y;        // number of cells along y-axis
  int m_no_cells;       // total number of cells

  int m_x_offset;       // x-offset to the origin
  int m_y_offset;	// y-offset to the origin

  int m_cell_width;     // width of cell in pixels
  int m_cell_height;    // height of cell in pixels

  double m_zoom;   // zoom factor
  bool m_draw_cell_boundaries; // whether or not cell boundaries should be drawn

  int m_x_selected;     // x-position of the selected cell
  int m_y_selected;     // y-position of the selected cell

  std::vector<MapViewCellEntry> m_cell_info;
  MapViewCellColorUtil *m_color_util;

protected:
  void drawContents( QPainter *, int clipx, int clipy, int clipw, int cliph );

  /**
   * Draw the cell at the specified position in the given color.
   **/
  void drawCell( QPainter *p, int x, int y, QColor color );

  /**
   * Draw the selected cell.
   **/
  void drawSelected( QPainter *p );

  // coordinate transf.
  /**
   * Does a translation of a cell coordinate x by m_x_offset. Correctly
   * wraps the boundary if resulting x>=m_world_x.
   **/
  int translX( int x );

  /**
   * Does a translation of a cell coordinate y by m_y_offset. Correctly
   * wraps the boundary if resulting y>=m_world_y.
   **/
  int translY( int y );

  /**
   * Marks the complete map for redrawing.
   *
   * Is needed after translX, translY, or zoom;
   **/
  void allChanged();

  /**
   * Calculates the upper left corner in screen coords for the
   * given cell position (cells are counted starting with 0).
   **/
  QPoint upLeft( int x, int y );

  /**
   * Calculates the upper right corner in screen coords for the
   * given cell position (cells are counted starting with 0).
   **/
  QPoint upRight( int x, int y );

  /**
   * Calculates the lower left corner in screen coords for the
   * given cell position (cells are counted starting with 0).
   **/
  QPoint lowLeft( int x, int y );

  /**
   * Calculates the lower right corner in screen coords for the
   * given cell position (cells are counted starting with 0).
   **/
  QPoint lowRight( int x, int y );

  /**
   * Calculates the x position of a cell from the x coordinate on the screen.
   **/
  int screenToCellX( int x );

  /**
   * Calculates the y position of a cell from the y coordinate on the screen.
   **/
  int screenToCellY( int x );


protected slots:
  /**
   * Overloaded from QScrollView. Processes mouse clicks into the view.
   **/
  void contentsMouseReleaseEvent( QMouseEvent *event );

public:
  MapViewWidget(
    avd_MissionControl *mission_control,
    ColorScaleWidget *cs_widget,
    QWidget * parent=0, const char * name=0, WFlags f=0 );
  ~MapViewWidget();

public slots:
  /**
   * Allows to specify the zoom factor.
   **/
  void setZoomFactor( double value );
  void setXOffset( int value );
  void setYOffset( int value );
  /**
   * Allows to specify the mode of the map (which property of the creatures
   * should be displayed).
   **/
  void setMapMode( MapViewCellColorUtil::MapMode mode );

  /**
   * Redraws everything.
   **/
  void drawViewport(bool redraw);


  /**
   * Returns the currently selected cell.
   **/
  int GetSelectedCell() const {
    return m_x_selected + m_world_x * m_y_selected; }
signals:
  /**
   * This signal is emited if a cell is selected by mouseclick.
   **/
  void cellSelected( int cell );
};


#endif


