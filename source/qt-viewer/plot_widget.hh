#ifndef PLOT_WIDGET_HH
#define PLOT_WIDGET_HH


// porting to gcc 3.1 -- k
// i'd _really rather not use "using namespace std" in a header file,
// but it's required by our nested #includes.  which we should unnest...
// except that some templates and inlines require nesting?  what to do?
// -- K.


#include <vector>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>


//forward declarations
class QPainter;
class QPrinter;
class QPaintEvent;
class QKeyEvent;
class DoubleSlider;

/**
 * This class contains a 2d data point (2 doubles).
 * The different member functions should be clear.
 **/


class DataPoint {
private:
  double m_x;
  double m_y;

public:

  DataPoint() : m_x(0), m_y(0) {;}
  DataPoint( double x, double y ) : m_x(x), m_y(y) {;}
  DataPoint( const DataPoint &rhs ) : m_x( rhs.m_x ), m_y( rhs.m_y ) {;}
  ~DataPoint() {;}

  const DataPoint & operator=( const DataPoint &rhs ) {
    m_x = rhs.m_x; m_y = rhs.m_y; return *this; }

  // manipulators
  void setPoint( double x, double y ) {
    m_x = x; m_y = y; }

  void setX( double x ) {
    m_x = x; }

  void setY( double y ) {
    m_y = y; }

  // accessors
  double getX() const {
    return m_x; }

  double getY() const {
    return m_y; }

};


/**
 * This class manages an array of data points. It is meant for
 * online-monitoring of some data stream, which means you can add points,
 * but you cannot remove them. It just keeps growing. Maybe I will change
 * this at some point.
 **/


class PointArray {
private:
  std::vector<DataPoint> m_points;

  double m_x_min;
  double m_x_max;
  double m_y_min;
  double m_y_max;

  double m_last_x;

  bool m_x_ordered; // is true if the x-values are monotonously increasing

  int m_draw_points;  // how many points should be drawn

  /**
   * Finds the index corresponding to the largest x-value in the
   * array *smaller* than the given value x. Returns 0 if
   * the given value
   * is smaller than the smallest value in the array. Returns the size of
   * the array minus one if the given value is larger than the
   * largest value in the array.
   *
   * Should never be called when m_last_x is set to false!
   **/
  int findXInd( double x );


  PointArray( const PointArray & );
  const PointArray & operator=( const PointArray & );
public:
  PointArray();
  ~PointArray();

  // manipulators
  /**
   * Adds a point to the point array.
   **/
  void addPoint( const DataPoint &point );

  /**
   * Removes all points.
   **/
  void clear();

  // accessors
  double getXMin(){
    return m_x_min; }

  double getXMax(){
    return m_x_max; }

  double getYMin(){
    return m_y_min; }

  double getYMax(){
    return m_y_max; }



  /**
   * This function draws the array onto a painter. It expects the coordinate
   * system to be with the origin in the upper left
   * corner. The parameters xmin, ymin, xmax, ymax specify the range of the
   * plot data that should be displayed. The parameters plot_width,
   * plot_height specify the size of the area to plot into.
   *
   * When the variable omit_points is set to true, the drawing function may
   * drop some of the points in order to speed up the drawing. When omit_points
   * is false, all points are drawn.
   **/
  void draw( QPainter *p, double xmin, double ymin, double xmax, double ymax, int plot_width, int plot_height, bool omit_points = true );

};


class PlotCanvas : public QWidget {
  Q_OBJECT
private:
  PointArray m_point_array;  // the data points
  QPixmap m_pixmap; // a pixmap to plot into
  double m_x_display_width; // the data width in x-dimension that should
  // be shown
  double m_x_display_min; // the minimum x value to be displayed

  double m_y_display_height; // the data width in y-dimension that should
  // be shown
  double m_y_display_min; // the minimum y value to be displayed

  double m_x_global_min;
  double m_x_global_max;
  double m_y_global_min;
  double m_y_global_max;
  bool m_global_initialized;

  bool m_auto_scale;

  bool m_mouse_pressed; // is true when the mouse button has been pressed.
  bool m_y_effect; // is true after first release the mouse.
  QPoint m_p1; // the first and the second point
  QPoint m_p2; // of a mouse drag

  int m_modeX; // whether x or y are in log scale...
  int m_modeY;

  QString m_x_label;
  QString m_y_label;


  // the following values are not pixel coordinates, but
  // abstract coordinates.
  // they define the coordinate system of the plot
  // (and need not be touched normally)
  static const int m_plot_width;  // the width of the plot
  static const int m_plot_height; // the height of the plot
  static const int m_left_margin; // the left margin (from the side of the widget to the beginning of the plot)
  static const int m_right_margin;  // the right margin
  static const int m_top_margin;    // the top margin
  static const int m_bottom_margin; // the bottom margin
  static const int m_font_size;     // the size of the font used

  // pointer to the two sliders of the plot widget
  // at some point, we might want to make the plot canvas not
  // depend on these pointers anymore, but it is actually not
  // so bad, and easier to write at this point.
  DoubleSlider *m_x_slider;
  DoubleSlider *m_y_slider;
  QPrinter *m_printer;

public:
  PlotCanvas( DoubleSlider *x_slider, DoubleSlider *y_slider, QWidget *parent = 0, const char * name = 0, WFlags f = 0 );
  ~PlotCanvas();

public slots:
  /**
   * Add a data point to the widget.
   *
   * @param point The point to add.
   * @param redraw Bool that indicates whether the widget should be redrawn
   * after the clear. Default is true.
   **/
  void addPoint( const DataPoint & point, bool redraw = true );

  /**
   * Removes all the data points from the widget.
   *
   * @param redraw Bool that indicates whether the widget should be redrawn
   * after the clear. Default is true.
   **/
  void clear( bool redraw = true );

  int getModeX() const {
    return m_modeX;
  }

  void setModeX(int x_mode){
    m_modeX = x_mode;
  }

  int getModeY() const {
    return m_modeY;
  }

  void setModeY(int y_mode){
    m_modeY = y_mode;
  }

  void setXLabel( const QString & l ){
    m_x_label = l; }

  void setYLabel( const QString & l ){
    m_y_label = l; }

  /**
   * Opens a print dialog, asks for a file name, and prints
   * the current graph into the file.
   */
  void print();


  /**
   * Splits a double value into a value between 1 and 10 and an exponent
   * to the base of 10.
   *
   * @param value The value that should be split.
   * @param exponent A pointer to an integer that will hold the exponent
   * when the function returns.
   * @return The value between 1 and 10.
   **/
  static double splitCoeffExp( double value, int *exponent );

  /**
   * Function used to calculate the position of axis ticks.
   *
   * @param min The minimum value to plot.
   * @param max The maximum value to plot.
   * @param base A pointer to a double that will hold the proposed minimum
   * value of the plot range.
   * @param top A pointer to a double that will hold the proposed maximum
   * value of the plot range.
   * @return The exponent to the base of 10 of the proposed tick steps.
   **/
  static int calcAxisRange( double min, double max, double *base, double *top );

protected:
  /**
   paint the widget.
   **/
  void paintEvent( QPaintEvent * );
  void closeEvent ( QCloseEvent * e );

  void mousePressEvent( QMouseEvent *event );
  void mouseMoveEvent( QMouseEvent *event );
  void mouseReleaseEvent( QMouseEvent *event );
  void mouseDoubleClickEvent( QMouseEvent *event );
  void keyPressEvent( QKeyEvent *event );

  /**
   * Performs the actual drawing of the axis ticks.
   *
   * @param p A QPainter to draw on.
   * @param min The minimum value to plot.
   * @param max The maximum value to plot.
   * @param base The base value for the axis ticks.
   * @param top The top value for the axis ticks.
   * @param range_exp The exponent to the base of 10 of the proposed tick
   * steps.
   * @param horizontal A boolean that indicates whether the horizontal
   * (true) or vertical (false) axis should be given the ticks.
   **/
  void drawAxisTicks( QPainter *p, double min, double max, double base, double top, int range_exp, bool horizontal, QColor main_color, QColor secondary_color );

  void drawAxisLabels( QPainter *p, QColor main_color );


  /**
   * Draws the whole plot, with axes etc.
   *
   * When the variable omit_points is set to true, the drawing function may
   * drop some of the points in order to speed up the drawing. When omit_points
   * is false, all points are drawn.
   */
  void draw( QPainter *p, QColor main_color, QColor secondary_color, bool omit_points );


protected slots:
  void xSliderChanged();
  void ySliderChanged();

};



class PlotWidget : public QMainWindow {
  Q_OBJECT
private:
  // sub widgets
  PlotCanvas *m_plot_canvas;
  QPushButton *m_config_button;
  DoubleSlider *m_y_slider;
  DoubleSlider *m_x_slider;

public:
  PlotWidget( QWidget *parent = 0, const char * name = 0, WFlags f = 0 );
  ~PlotWidget();

public slots:
  /**
   * Add a data point to the widget.
   *
   * @param point The point to add.
   * @param redraw Bool that indicates whether the widget should be redrawn
   * after the clear. Default is true.
   **/
  void addPoint( const DataPoint & point, bool redraw = true ){
    m_plot_canvas->addPoint( point, redraw ); }

  /**
   * Removes all the data points from the widget.
   *
   * @param redraw Bool that indicates whether the widget should be redrawn
   * after the clear. Default is true.
   **/
  void clear( bool redraw = true ){
    m_plot_canvas->clear( redraw ); }

  int getModeX() const {
    return m_plot_canvas->getModeX();
  }

  void setModeX(int x_mode){
    m_plot_canvas->setModeX( x_mode );
  }

  int getModeY() const {
    return m_plot_canvas->getModeY();
  }

  void setModeY(int y_mode){
    m_plot_canvas->setModeY( y_mode );
  }

  void setXLabel( const QString & l ){
    m_plot_canvas->setXLabel( l ); }

  void setYLabel( const QString & l ){
    m_plot_canvas->setYLabel( l ); }

};



#endif








