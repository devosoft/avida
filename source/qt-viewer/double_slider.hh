#ifndef DOUBLE_SLIDER_HH
#define DOUBLE_SLIDER_HH


// porting to gcc 3.1 -- k
// i'd _really rather not use "using namespace std" in a header file,
// but it's required by our nested #includes.  which we should unnest...
// except that some templates and inlines require nesting?  what to do?
// -- K.


#include <qwidget.h>
#include <utility> // for pair<>

class DoubleSlider : public QWidget
{
  Q_OBJECT
private:
  enum Slider { First, Second, None };

  Qt::Orientation m_orientation;

  double m_pos1; // position of first slider
  double m_pos2; // position of second slider

  int m_knob_width; // width of the knob in pixel
  double m_min_slider_dist; // minimum distance the two sliders can be apart
  Slider m_pressed; // which slider was pressed on last mouse button down?


  DoubleSlider();
  DoubleSlider( const DoubleSlider & );
public:
  DoubleSlider( Qt::Orientation orientation, QWidget *parent );
  ~DoubleSlider();

  void setPositions( std::pair<double, double> p ) {
    m_pos1 = p.first;
    if ( m_pos1 < 0 ) m_pos1 = 0;
    if ( m_pos1 > 1 ) m_pos1 = 1;
    m_pos2 = p.second;
    if ( m_pos2 < 0 ) m_pos2 = 0;
    if ( m_pos2 > 1 ) m_pos2 = 1;
    emit stateChanged();
    update();
  }

  std::pair<double, double> getPositions() const {
    return std::pair<double, double> ( m_pos1, m_pos2 ); }
  
protected:
  void drawGroove( QPainter *p, const QRect &r );
  void drawKnob( QPainter *p, const QRect &r,
		 const QColor &c, Slider slider );
  Slider testMouseClick( const QPoint &pos );
  double getSliderPos( const QPoint &pos );

protected slots:
  void paintEvent( QPaintEvent *e );
  void mousePressEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseDoubleClickEvent( QMouseEvent *e );
  
signals:
  void stateChanged();
};



#endif
