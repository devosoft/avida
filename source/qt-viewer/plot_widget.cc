
#include "plot_widget.hh"
#include "double_slider.hh"

#include <float.h>           // for DBL_MIN, DBL_MAX (PointArray)
#include <cmath>
#include <stdio.h>
#include <iostream>

#include <qpainter.h>
#include <qprinter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qlayout.h>
#include <qpushbutton.h>


using namespace std;


PointArray::PointArray() : m_x_min( DBL_MAX ), m_x_max( -DBL_MAX ),
  m_y_min( DBL_MAX ), m_y_max( -DBL_MAX ), m_last_x( -DBL_MAX ),
  m_x_ordered( true ), m_draw_points( 100 )
{
}


PointArray::~PointArray()
{
}

void
PointArray::addPoint( const DataPoint &point )
{
  if ( point.getX() < m_x_min ) m_x_min = point.getX();
  if ( point.getX() > m_x_max ) m_x_max = point.getX();
  if ( point.getY() < m_y_min ) m_y_min = point.getY();
  if ( point.getY() > m_y_max ) m_y_max = point.getY();

  // if the x value of the new point is smaller than
  // the one of the last point, the x order
  // has been destroyed.
  if ( point.getX() < m_last_x )
     m_x_ordered = false;

  m_last_x = point.getX();

  m_points.push_back( point );
}


void
PointArray::clear()
{
  m_points.clear();
  m_x_min = DBL_MAX;
  m_x_max = -DBL_MAX;
  m_y_min = DBL_MAX;
  m_y_max = -DBL_MAX;
  m_last_x = -DBL_MAX;
  m_x_ordered = true;
}



int
PointArray::findXInd( double x )
{
  int end = m_points.size();
  int result;
  int result_min = 0;
  int result_max = end-1;
  do {
    result = (result_max-result_min)/2
      +result_min;
    if (result==0){
      if (x < m_points[0].getX()) break;
      else result_min = 1;
    }
    else if (result==end-1){// should be revised. not 100% correct.
      if (x < m_points[result-1].getX())
        result_max = result-1;
      else break;
    }
    else if (x < m_points[result].getX()){
      if (x >= m_points[result-1].getX()){
        break;
      }
      else result_max = result-1;
    }
    else if (x < m_points[result+1].getX()){
      result++;
      break;
    }
    else result_min = result+1;
  }while (1);

  if ( result > 0 ) result -= 1;
  return result;
}



void
PointArray::draw( QPainter *p, double xmin, double ymin, double xmax, double ymax, int plot_width, int plot_height, bool omit_points )
{
  int start, finish, step, tmp;

  finish = m_points.size();
  if ( finish < 2 ) return; // is there anything to draw?

  // are the x-values ordered?
  if ( !m_x_ordered ){  // no
    start = 0;
    step = 1;
    // finish has been set above
  }
  else{ // x-values are ordered
    start = findXInd( xmin );
    tmp = findXInd( xmax ) + 2; // findXInd() finds always the first value smaller than the given value, we need to go 2 further
    if ( tmp < finish )
      finish = tmp;
    if ( !omit_points || finish-start < m_draw_points ) // if we have less points than we
      // should have, we draw them all
      step = 1;
    else{ // otherwise, adjust the step value
      step = (finish-start)/m_draw_points;

    }
  }


  if ( start == finish ) return; // do we have anything to draw?

  int x1,y1;

  // we have an area of plot_width x plot_height
  double w = (double) plot_width / ( xmax - xmin );
  double h = (double) plot_height/ ( ymax - ymin );

  double tempX,tempY;
  tempX = m_points[start].getX();
  x1 = (int) ( w *(tempX - xmin ) );

  tempY = m_points[start].getY();
  y1 = plot_height - (int) ( h * (tempY - ymin ) );

  int x2, y2;
  for ( int i = start+1; i < finish; i+=step ){
    tempX = m_points[i].getX();
    x2 = (int) ( w *(tempX - xmin ) );
    tempY = m_points[i].getY();
    y2 = plot_height - (int) ( h * (tempY - ymin ) );
    p->drawLine( x1, y1, x2, y2 );
    x1 = x2;
    y1 = y2;
  }
}




const int PlotCanvas::m_plot_width = 1000;
const int PlotCanvas::m_plot_height = 700;
const int PlotCanvas::m_left_margin = 160;
const int PlotCanvas::m_right_margin = 300;
const int PlotCanvas::m_top_margin = 60;
const int PlotCanvas::m_bottom_margin = 240;
const int PlotCanvas::m_font_size = 40;

PlotCanvas::PlotCanvas( DoubleSlider *x_slider, DoubleSlider *y_slider, QWidget *parent, const char * name, WFlags f )
  : QWidget( parent, name, f ), m_x_display_width( 200 ),
    m_global_initialized( false ), m_x_slider( x_slider ),
    m_y_slider( y_slider )
{
  m_printer = new QPrinter();
  connect( m_x_slider, SIGNAL( stateChanged() ),
	   this, SLOT( xSliderChanged() ) );
  connect( m_y_slider, SIGNAL( stateChanged() ),
	   this, SLOT( ySliderChanged() ) );
  
  setFocusPolicy( StrongFocus );
  
  m_mouse_pressed = false;
  m_auto_scale = true;
  m_y_effect = false;
  m_modeX = 0;  //normal mode;
  m_modeY = 0;  //normal mode;
}


PlotCanvas::~PlotCanvas()
{
  delete m_printer;
}


void
PlotCanvas::addPoint( const DataPoint & point, bool redraw )
{
  m_point_array.addPoint( point );
  if ( !m_mouse_pressed ){ // we repaint only if we are not currently doing
    // a selection
    m_auto_scale = true;
    if ( redraw ) {
      repaint(false);
    }
  }
}


void
PlotCanvas::clear( bool redraw )
{
  m_point_array.clear();
  m_auto_scale = true;
  m_mouse_pressed = false;
  if ( redraw ) repaint( false );
}


void
PlotCanvas::mousePressEvent( QMouseEvent *event )
{
  if ( !m_mouse_pressed && event->button() == Qt::LeftButton ){
    m_mouse_pressed = true;
    // m_pixmap.fill( Qt::white );
    QPainter p( &m_pixmap );
    p.setPen( Qt::black );
    p.setRasterOp( Qt::XorROP );
    p.setWindow( -m_left_margin, -m_top_margin,
		 m_plot_width + m_right_margin,
		 m_plot_height + m_bottom_margin );
    m_p1 = p.xFormDev( event->pos() );
    m_p2 = m_p1;
    p.drawRect( QRect( m_p1, m_p2 ) );
    p.end();
    bitBlt ( this, 0, 0, &m_pixmap );
  }	
}


void
PlotCanvas::mouseMoveEvent( QMouseEvent *event )
{
  if ( m_mouse_pressed ){
    QPainter p( &m_pixmap );
    p.setPen( Qt::white );
    p.setRasterOp( Qt::XorROP );
    p.setWindow( -m_left_margin, -m_top_margin,
		 m_plot_width + m_right_margin,
		 m_plot_height + m_bottom_margin );
    p.drawRect( QRect( m_p1, m_p2 ) );
    m_p2 = p.xFormDev( event->pos() );
    p.drawRect( QRect( m_p1, m_p2 ) );
    p.end();
    bitBlt ( this, 0, 0, &m_pixmap );
  }
}


void
PlotCanvas::mouseReleaseEvent( QMouseEvent *event )
{
  if ( m_mouse_pressed ){
    m_mouse_pressed = false;
    QPainter p( &m_pixmap );
    p.setPen( Qt::black );
    p.setRasterOp( Qt::XorROP );
    p.setWindow( -m_left_margin, -m_top_margin,
		 m_plot_width + m_right_margin,
		 m_plot_height + m_bottom_margin );
    p.drawRect( QRect( m_p1, m_p2 ) );
    m_p2 = p.xFormDev( event->pos() );

    p.end();
    bitBlt ( this, 0, 0, &m_pixmap );
    // set what we got

    double dx = fabs( (double)(m_p2.x() - m_p1.x()) ) / (double) m_plot_width;
    double dy = fabs( (double)(m_p2.y() - m_p1.y()) ) / (double) m_plot_height;

    if ( dx > .01 && dy > .01 ){ // do we have a reasonable size?
      m_x_display_min += m_x_display_width * QMIN( m_p1.x(), m_p2.x() )
			 / (double) m_plot_width;
      m_x_display_width *= dx;
      m_y_display_min += m_y_display_height *
			 (m_plot_height - QMAX( m_p1.y(), m_p2.y() ) )
			 / (double) m_plot_height;
      m_y_display_height *= dy;
      m_y_effect = true;

      m_auto_scale = false;
    }

    repaint( false );
  }
}


void
PlotCanvas::mouseDoubleClickEvent( QMouseEvent *event )
{

  if ( event->button() == //Qt::RightButton
       Qt::LeftButton ){
    m_x_display_width = m_point_array.getXMax() - m_point_array.getXMin();
    m_x_global_min = m_point_array.getXMin();
    m_x_global_max = m_point_array.getXMax();
    m_y_global_min = m_point_array.getYMin();
    m_y_global_max = m_point_array.getYMax();
    m_auto_scale = true;
    repaint( false );
  }
}


void
PlotCanvas::keyPressEvent( QKeyEvent *event )
{
  bool pressed = true;

  if ( event->state() & ShiftButton ){
    switch( event->key() ){
    case Key_Right:
      m_x_display_width += m_x_display_width / 20;
      break;
    case Key_Left:
      m_x_display_min -= m_x_display_width / 20;
      m_x_display_width += m_x_display_width / 20;
      break;
    case Key_Up:
      m_y_display_height += m_y_display_height / 20;
      break;
    case Key_Down:
      m_y_display_min -= m_y_display_height / 20;
      m_y_display_height += m_y_display_height / 20;
      break;
    default:
      event->ignore();
      pressed = false;
    }
  }
  else if ( event->state() & ControlButton ){
    switch( event->key() ){
    case Key_Right:
      m_x_display_min += m_x_display_width / 20;
      m_x_display_width -= m_x_display_width / 20;
      break;
    case Key_Left:
      m_x_display_width -= m_x_display_width / 20;
      break;
    case Key_Up:
      m_y_display_min += m_y_display_height / 20;
      m_y_display_height -= m_y_display_height / 20;
      break;
    case Key_Down:
      m_y_display_height -= m_y_display_height / 20;
      break;
    default:
      event->ignore();
      pressed = false;
    }
  }
  else {
    switch( event->key() ){
    case Key_Right:
      m_x_display_min += m_x_display_width / 20;
      break;
    case Key_Left:
      m_x_display_min -= m_x_display_width / 20;
      break;
    case Key_Up:
      m_y_display_min += m_y_display_height / 20;
      break;
    case Key_Down:
      m_y_display_min -= m_y_display_height / 20;
      break;
    case Key_P:
      print();
      break;
    default:
      event->ignore();
      pressed = false;
    }
  }
  if ( pressed ){
    m_auto_scale = false;
    repaint( false );
  }

}

void
PlotCanvas::closeEvent ( QCloseEvent * e )
{
  this->hide();
}

void
PlotCanvas::paintEvent( QPaintEvent * )
{
  if ( m_pixmap.width() != width() || m_pixmap.height() != height() )
    m_pixmap.resize( width(), height() );
  m_pixmap.fill( Qt::black );

  QPainter p( &m_pixmap );

  // define color for secondary lines
  QColor qc;
  qc.setRgb(0,21,200);

  // draw everything
  draw( &p, Qt::white, qc, true );

  p.end();

  bitBlt ( this, 0, 0, &m_pixmap );
}

void
PlotCanvas::print()
{
  m_printer->setOrientation( QPrinter::Landscape );

  if ( m_printer->setup( this ) ) {
    QPainter p( m_printer );
    draw( &p, Qt::black, Qt::lightGray, false );
    p.end();
  }
}


void
PlotCanvas::draw( QPainter *p, QColor main_color, QColor secondary_color, bool omit_points )
{
  p->setPen( main_color );
  p->setWindow( -m_left_margin, -m_top_margin,
	       m_plot_width + m_right_margin,
	       m_plot_height + m_bottom_margin );
//  p->drawLine( 0, 0, m_plot_width, 0 );
//  p->drawLine( m_plot_width, 0, m_plot_width, m_plot_height );
  p->drawLine( m_plot_width, m_plot_height, 0, m_plot_height );
  p->drawLine( 0, m_plot_height, 0, 0 );

  double xmax, ymax, xbase, xtop, ybase, ytop;
  int xrange_exp, yrange_exp;
  if ( m_auto_scale ){ // are we autoscaling?
    // do we already have more than one window full to display?

    // x-axis
    if ( m_point_array.getXMax() - m_point_array.getXMin()
	 > m_x_display_width ){

      xmax = m_point_array.getXMax();
      m_x_display_min = xmax - m_x_display_width;


    }
    else{
      m_x_display_min = m_point_array.getXMin();
      xmax = m_x_display_min + m_x_display_width;
    }

    // y-axis
    yrange_exp = calcAxisRange( m_point_array.getYMin(), m_point_array.getYMax(), &ybase, &ytop );
    m_y_display_min = ybase;
    ymax = ytop;
    m_y_display_height = ymax - m_y_display_min;
  }
  else { // no autoscaling
    xmax = m_x_display_min + m_x_display_width;
    ymax = m_y_display_min + m_y_display_height;
    yrange_exp = calcAxisRange( m_y_display_min, ymax, &ybase, &ytop );
  }

  // update the max plot range
  if ( m_global_initialized ){
    if ( m_x_display_min < m_x_global_min )
      m_x_global_min = m_x_display_min;
    if ( m_x_display_width > m_x_global_max - m_x_display_min )
      m_x_global_max = m_x_display_width + m_x_display_min;
    if ( m_y_display_min < m_y_global_min )
      m_y_global_min = m_y_display_min;
    if ( m_y_display_height > m_y_global_max - m_y_display_min )
      m_y_global_max = m_y_display_height + m_y_display_min;
  }
  else {
    m_x_global_min = QMIN( m_x_display_min, m_point_array.getXMin() );
    m_x_global_max = QMAX( m_x_display_width + m_x_global_min,
			   m_point_array.getXMax() );
    m_y_global_min = QMIN( m_y_display_min, m_point_array.getYMin() );
    m_y_global_max = QMAX( m_y_display_height + m_y_global_min,
			   m_point_array.getYMax() );
    m_global_initialized = true;
  }

  // now that we have the range we want to plot, we inform the sliders
  double p1, p2;
  p1 = ( m_x_display_min - m_x_global_min )
       / ( m_x_global_max - m_x_global_min );
  p2 = ( xmax -  m_x_global_min )
       / ( m_x_global_max - m_x_global_min );
  m_x_slider->blockSignals( true );
  m_x_slider->setPositions( pair<double, double> ( p1, p2 ) );
  m_x_slider->blockSignals( false );				

  p2 = 1 - ( ( m_y_display_min - m_y_global_min ) )
       / ( m_y_global_max - m_y_global_min );
  p1 = 1 - ( ( ymax -  m_y_global_min )
       / ( m_y_global_max - m_y_global_min ) );
  m_y_slider->blockSignals( true );
  m_y_slider->setPositions( pair<double, double> ( p1, p2 ) );
  m_y_slider->blockSignals( false );
						
  // draw the x-axis ticks
  xrange_exp = calcAxisRange( m_x_display_min, xmax, &xbase, &xtop );
  drawAxisTicks( p, m_x_display_min, xmax, xbase, xtop, xrange_exp, true, main_color, secondary_color );

  // draw the y-axis ticks
  drawAxisTicks( p, m_y_display_min, ymax, ybase, ytop, yrange_exp, false, main_color, secondary_color );

  drawAxisLabels( p, main_color );

  // for the actual drawing, clip inside the rectangle
  p->setClipRect( p->xForm( QRect( 0, 0, m_plot_width+1, m_plot_height+1 ) ) );

  // draw the data
  m_point_array.draw( p, m_x_display_min, m_y_display_min, xmax, ymax, m_plot_width, m_plot_height, omit_points );
}


double
PlotCanvas::splitCoeffExp( double value, int *exponent )
{
  // transform variable 'range' into coefficient between 1 and 10
  // and integral exponent to base 10

  int exp_2;

  double coeff = frexp( value, &exp_2 );
  double exp_10 = exp_2/3.3219280949;
  *exponent = (int) floor( exp_10 );
  coeff *= pow( double(10), exp_10 - (double)*exponent );

  if ( coeff < 1 ){
    coeff *= 10;
    *exponent -= 1;
  }

  // now exponent holds the exponent to base 10, and coeff holds the
  // coefficient.
  return coeff;
}



int
PlotCanvas::calcAxisRange( double min, double max, double *base, double *top )
{
  int range_exp;
  int min_exp;
  int max_exp;

  double range = max - min;

  // get the order of magnitude of the range
  splitCoeffExp( range, &range_exp );

  // now determine base and top value
  double min_coeff = splitCoeffExp( min, &min_exp );
  *base = floor( min_coeff * pow( (double)10, min_exp-range_exp ) )
		* pow( (double)10, range_exp );
  double max_coeff = splitCoeffExp( max, &max_exp );
  *top = ceil( max_coeff * pow( (double)10, max_exp-range_exp ) )
	       * pow( (double)10, range_exp );

  return range_exp;
}

void
PlotCanvas::drawAxisLabels( QPainter *p, QColor main_color )
{
  QFont f( "times", m_font_size );
  p->setFont( f );

  p->drawText( 0, m_plot_height + m_bottom_margin/3,
	       m_plot_width, 50, Qt::AlignCenter, m_x_label );

  p->save();
  p->translate( -m_left_margin, m_plot_height );
  p->rotate( -90 );
  p->drawText( 0, 0,
	       m_plot_height, 50, Qt::AlignCenter, m_y_label );
  p->restore();
}
	
	

void
PlotCanvas::drawAxisTicks( QPainter *p, double min, double max, double base, double top, int range_exp, bool horizontal, QColor main_color, QColor secondary_color )
{
  int x_log=0;
  int y_log=0;
  x_log = getModeX();
  y_log = getModeY();

  if (horizontal && x_log ){
    if (min <= 1) min = 0;
    else min = log10(min);
    max = log10(max);
    if (base <=1) base = 0;
    else base = log10(base);
    top = log10(top);
    double temprange = (max-min);
    splitCoeffExp( temprange, &range_exp );
  }
  if(!horizontal && y_log){
    if (min <= 1) min = 0;
    else min = log10(min);
    max = log10(max);
    if (base <=1) base = 0;
    else base = log10(base);
    top = log10(top);
    double temprange = (max-min);
    splitCoeffExp( temprange, &range_exp );
  }

  double w = m_plot_width / ( max - min );
  double h = m_plot_height / ( max - min );
  double range = pow( (double)10, range_exp );
  int ticks = (int) ( (top - base) / range );
  double tick_coeff = 1;
  double value;

  if ( ticks < 4 ){
    ticks *= 2;
    tick_coeff = .5;
  }
  else if ( ticks > 6 ){
    ticks /= 2;
    if ( (int) ( base / range ) % 2 > 0 )
      base -= range;
    tick_coeff = 2;
  }

  // sizes needed for plotting
  const int x_tick_length = m_plot_width/60;
  const int y_tick_length = m_plot_height/50;

  QFont f( "times", m_font_size );

  p->setFont( f );
  QString label;

  QPen qp1,qp2;

  qp1.setStyle( DotLine );
  qp1.setColor( secondary_color );

  qp2.setStyle( SolidLine );
  qp2.setColor( main_color );


  for ( int i=0; i<=ticks; i++ ){
    value = base + i * tick_coeff * pow( (double)10, range_exp );
    if ( value >= min && value <= max ){
      if ( horizontal ){ // draw x-axis
	int x1 = (int) ( w * ( value - min ) );
        if( x1 != 0 ){
          p->setPen( qp1 );
          p->drawLine( x1, 0, x1, m_plot_height );
          p->setPen( qp2 );
        }
	
	p->drawLine( x1, m_plot_height - y_tick_length, x1, m_plot_height );
//	p->drawLine( x1, 0, x1, y_tick_length );

	label.sprintf( "%g", value );
	p->drawText( x1-m_plot_width/2,
		     m_plot_height + y_tick_length,
		     m_plot_width, 50, Qt::AlignCenter, label );
      }
      else { // draw y-axis
	int y1 = m_plot_height - (int) ( h * ( value - min ) );
	
        if ( i != 0 ){
          p->setPen( qp1 );
          p->drawLine( 0, y1, m_plot_width, y1);
          p->setPen( qp2 );
        }
	p->drawLine( 0, y1, x_tick_length, y1 );
	
	label.sprintf( "%g ", value );
	p->drawText( -m_plot_width/6 - x_tick_length/2,
		     y1 - 2.2 * y_tick_length,
		     m_plot_width/6, 50, Qt::AlignRight, label );
      }
    }
  }
  p->setPen( qp2 );
}



void PlotCanvas::xSliderChanged()
{
  pair<double, double> p = m_x_slider->getPositions();

  double w;
  w = m_x_global_max - m_x_global_min;
  double display_min = m_x_global_min + p.first * w;
  double display_width = (p.second - p.first) * w;


  m_x_display_min = display_min;
  m_x_display_width = display_width;

  m_auto_scale = false;
  repaint( false );
}


void PlotCanvas::ySliderChanged()
{
  pair<double, double> p = m_y_slider->getPositions();

  double h;
  h = m_y_global_max - m_y_global_min;
  double display_min = m_y_global_min + (1-p.second) * h;
  double display_height = (p.second - p.first) * h;

  m_y_display_min = display_min;
  m_y_display_height = display_height;
  m_auto_scale = false;
  repaint( false );
}


PlotWidget::PlotWidget( QWidget *parent, const char * name, WFlags f )
  : QMainWindow( parent, name, f )
{
  QBoxLayout *topLayout = new QVBoxLayout( this, 0 );

  m_config_button = new QPushButton("Print",this,"config");
  m_config_button->setSizePolicy(
       QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::Fixed ) );

  m_x_slider = new DoubleSlider( Qt::Horizontal, this );
  m_y_slider = new DoubleSlider( Qt::Vertical, this );
  // set width of y slider to height of x slider
  m_y_slider->setMaximumWidth( m_config_button->sizeHint().height() );
  m_y_slider->setMinimumWidth( m_config_button->sizeHint().height() );


  m_plot_canvas = new PlotCanvas( m_x_slider, m_y_slider, this );

  connect( m_config_button, SIGNAL( clicked() ),
	   m_plot_canvas, SLOT( print() ) );
  
  QHBoxLayout *hlayout = new QHBoxLayout(topLayout);
  hlayout->addWidget(m_plot_canvas);
  hlayout->addWidget( m_y_slider );
  QHBoxLayout *hlayout2 = new QHBoxLayout(topLayout);
  hlayout2->addWidget( m_x_slider );
  hlayout2->addWidget( m_config_button );


  // let the plot canvas process key strokes
  setFocusPolicy( StrongFocus );
  setFocusProxy( m_plot_canvas );

  resize( 400, 215 );
}


PlotWidget::~PlotWidget()
{
}


//#include "plot_widget.moc"



