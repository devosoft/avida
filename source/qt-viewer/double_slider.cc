
#include "double_slider.hh"

#include <qapplication.h>
#include <qslider.h>
#include <qpainter.h>


using namespace std;


DoubleSlider::DoubleSlider( Qt::Orientation orientation, QWidget *parent )
  : QWidget( parent ), m_orientation( orientation )
{
  m_pos1 = .3;
  m_pos2 = .5;

  m_knob_width = 10;
  m_min_slider_dist = 0;
}

DoubleSlider::~DoubleSlider()
{
}

void DoubleSlider::drawGroove( QPainter *p, const QRect &r )
{
  if ( m_orientation == Qt::Horizontal ){
    p->drawLine( r.left(), r.bottom()/2, r.right(), r.bottom()/2 );
  }
  else {
    p->drawLine( r.right()/2, r.bottom(), r.right()/2, r.top() );
  }
}


void DoubleSlider::drawKnob( QPainter *p, const QRect &r,
			     const QColor &c, Slider slider )
{
  p->setBrush( c );

  if ( m_orientation == Qt::Horizontal ){
    int w = 1 + r.right() - r.left() - 2*m_knob_width;
    int h = 1 + r.bottom() - r.top();
    if ( slider == First )
      p->drawRect( (int) (m_pos1*w) + r.left(), r.top(),
		   m_knob_width, h );
    else
      p->drawRect( (int) (m_pos2*w) + r.left() + m_knob_width, r.top(),
		   m_knob_width, h );
  }
  else {
    int w = 1 + r.right() - r.left();
    int h = 1 + r.bottom() - r.top() - 2*m_knob_width;
    if ( slider == First )
      p->drawRect( r.left(), (int) (m_pos1*h) + r.top(),
		   w, m_knob_width );
    else
      p->drawRect( r.left(), (int) (m_pos2*h) + r.top() + m_knob_width,
		   w, m_knob_width );
  }
}

DoubleSlider::Slider DoubleSlider::testMouseClick( const QPoint &pos )
{
  QRect r = rect();

  if ( m_orientation == Qt::Horizontal ){
    // horizontal mode-> have to check only x coordinates
    int w = 1 + r.right() - r.left() - 2*m_knob_width;

    if ( pos.x() >= (int) (m_pos1*w) + r.left()
	 && pos.x() <= (int) (m_pos1*w) + r.left() + m_knob_width )
      return First;
    if ( pos.x() >= (int) (m_pos2*w) + r.left() + m_knob_width
	 && pos.x() <= (int) (m_pos2*w) + r.left() + 2*m_knob_width )
      return Second;
    return None;
  }
  else {
    // vertical mode-> have to check only y coordinates
    int h = 1 + r.bottom() - r.top() - 2*m_knob_width;

    if ( pos.y() >= (int) (m_pos1*h) + r.top()
	 && pos.y() <= (int) (m_pos1*h) + r.top() + m_knob_width )
      return First;
    if ( pos.y() >= (int) (m_pos2*h) + r.top() + m_knob_width
	 && pos.y() <= (int) (m_pos2*h) + r.top() + 2*m_knob_width )
      return Second;
    return None;
  }
}

double DoubleSlider::getSliderPos( const QPoint &pos )
{
  QRect r = rect();
  double x;

  if ( m_orientation == Qt::Horizontal ){
    // horizontal mode-> have to check only x coordinates
    int w = 1 + r.right() - r.left() - 2*m_knob_width;
    x = (double) (pos.x() - r.left() - m_knob_width)/ (double) w;
  }
  else {
    // vertical mode-> have to check only y coordinates
    int h = 1 + r.bottom() - r.top() - 2*m_knob_width;
    x = (double) (pos.y() - r.top() - m_knob_width)/ (double) h;
  }

  if ( x > 1 )
    x = 1;
  else if ( x < 0 )
    x = 0;

  return x;
}


void DoubleSlider::paintEvent( QPaintEvent *e )
{
  QPainter p;

  p.begin( this );

  QRect r = rect();

  drawGroove( &p, r );
  drawKnob( &p, r, Qt::red, First );
  drawKnob( &p, r, Qt::green, Second );

  p.end();
}


void DoubleSlider::mousePressEvent( QMouseEvent *e )
{
  m_pressed = testMouseClick( e->pos() );
}


void DoubleSlider::mouseReleaseEvent( QMouseEvent * )
{
  m_pressed = None;
}


void DoubleSlider::mouseMoveEvent( QMouseEvent * e )
{
  double x = getSliderPos( e->pos() );

  
  switch( m_pressed ){
  case First:
    m_pos1 = x;
    if ( m_pos1 + m_min_slider_dist > m_pos2 )
      m_pos2 = m_pos1 + m_min_slider_dist;
    if ( m_pos2 > 1 ){
      m_pos2 = 1;
      m_pos1 = 1 - m_min_slider_dist;
    }
    emit stateChanged();
    break;
  case Second:
    m_pos2 = x;
    if ( m_pos1 + m_min_slider_dist > m_pos2 )
      m_pos1 = m_pos2 - m_min_slider_dist;
    if ( m_pos1 < 0 ){
      m_pos1 = 0;
      m_pos2 = m_min_slider_dist;
    }
    emit stateChanged();
    break;
  case None:
    break;
  }
  
  update();
}


void DoubleSlider::mouseDoubleClickEvent( QMouseEvent * )
{
  m_pos1 = 0;
  m_pos2 = 1;

  emit stateChanged();
  
  update();
}



//#include "double_slider.moc"
