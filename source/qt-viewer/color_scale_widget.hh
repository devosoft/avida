// -*- c++ -*-
#ifndef COLOR_SCALE_WIDGET_H
#define COLOR_SCALE_WIDGET_H


#include <qwidget.h>
#include <qvaluevector.h>

class ColorScaleWidget : public QWidget
{
  Q_OBJECT
private:
  double m_min_value;
  double m_max_value;
  bool m_activated;
  bool m_continuous;
  bool m_was_activated;
  
  // for discrete color scale
  QValueVector<QColor> m_color_vector;
  QValueVector<QString> m_descr_vector;
  int m_color_list_length;
    
  // the following variables define the geometry of the color
  // scale widget, margins etc.
  static const int m_left_margin; // the amount of pixels left of the color scale
  static const int m_top_margin; // the amount of pixels above the color scale
  static const int m_bottom_margin; // the amount of pixels below the color scale
  static const int m_right_margin; // the amount of pixels right from the text labels
  static const int m_spacing; // the spacing between the color scale and the text labels
  static const int m_stripe_width; // the width of the color stripes
  static const int m_stripes; // the number of colored stripes we are going to draw
  static const int m_step; // the number of stripes from one text label to the next


public:
  ColorScaleWidget( QWidget * parent );
  ~ColorScaleWidget();

  /**
   * Returns a sigmoid function.
   **/

  static double sigmoid( double x, double midpoint, double steepness );

  /**
   * Translates a value between 0 and 1 into a color, where
   * 0 is a very 'cold' color, and '1' is a very 'hot' color.
   **/
  static QColor doubleToColor( double x );

  /**
   * Sets the range for the continuous color scale. Also automatically
   * activates the continuous scale.
   **/
  void setRange( double min, double max ){
    m_min_value = min; m_max_value = max; m_continuous = true;}

  /**
   * Defines the discrete color scale, and switches the widget to
   * discrete mode.
   **/
  void setColorList( QValueVector<QColor> color_vector, QValueVector<QString> descr_vector ){
    m_color_vector = color_vector; m_descr_vector = descr_vector;
    m_color_list_length = QMIN( m_color_vector.size(), m_descr_vector.size() );
    m_continuous = false;
  }
  
  
  /**
   * Activates the color scale. Needs to be called for the color scale
   * to be drawn!
   **/
  void activate( bool a = true ){
    m_activated = a ;
    if ( a ) m_was_activated = true;
    recalcSize(); repaint();}

protected:
  void paintEvent( QPaintEvent * );

  /**
   * Draws the continuous color scale.
   **/
  void drawContinuousScale();
  
  /**
   * Draws the discrete color list.
   **/
  void drawColorList();
  
  /**
   * Recalculates the currently needed size.
   **/
  void recalcSize();

  QString getLabelString( double x );
};


#endif
