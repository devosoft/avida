#include "color_scale_widget.hh"

#include <qpainter.h>
#include <qfontmetrics.h>
#include <math.h>
#include <iostream>


using namespace std;



  // the following variables define the geometry of the color
  // scale widget, margins etc.
const int ColorScaleWidget::m_left_margin = 10; // the amount of pixels left of the color scale
const int ColorScaleWidget::m_top_margin = 10; // the amount of pixels above the color scale
const int ColorScaleWidget::m_bottom_margin = 10; // the amount of pixels below the color scale
const int ColorScaleWidget::m_right_margin = 10; // the amount of pixels right from the text labels
const int ColorScaleWidget::m_spacing = 10; // the spacing between the color scale and the text labels
const int ColorScaleWidget::m_stripe_width = 40; // the width of the color stripes
const int ColorScaleWidget::m_stripes = 100; // the number of colored stripes we are going to draw
const int ColorScaleWidget::m_step = 10; // the number of stripes from one text label to the next



ColorScaleWidget::ColorScaleWidget( QWidget *parent )
  : QWidget( parent ), m_activated( false ), m_was_activated( false )
{
  recalcSize();
}


ColorScaleWidget::~ColorScaleWidget()
{
}

double ColorScaleWidget::sigmoid( double x, double midpoint, double steepness )
{
  return  exp(steepness*(x-midpoint))/(1+exp(steepness*(x-midpoint)));
}

QColor ColorScaleWidget::doubleToColor( double x )
{
  if ( x>1 ) x = 1;
  else if ( x<0 ) x = 0;
  //  int h = (int) ((1.-x)*360 + 320) % 360;
  int h = (int) (x*360 + 100) % 360;
  int v = ((int) (sigmoid( x, .3, 10)*255));
  int s = ((int) (sigmoid( 1-x, .1, 30)*255));
  return QColor( h, s, v, QColor::Hsv );
}

void
ColorScaleWidget::paintEvent( QPaintEvent * )
{
  // we paint only when we are activated
  if ( !m_activated )
    return;

  if ( m_continuous )
    drawContinuousScale();
  else
    drawColorList();

}

void
ColorScaleWidget::drawContinuousScale()
{
  int w = width(); // the current width of the widget
  int h = height(); // the current height of the widget
  QPainter p( this );

  int stripe_height = (h-m_top_margin-m_bottom_margin)/m_stripes + 1;
  int plot_height = h-m_top_margin-m_bottom_margin-stripe_height+1;


  int text_width = w - m_left_margin - m_stripe_width - m_spacing;

  for ( int i=0; i<=m_stripes; i++ ){
    double x = (double) i / (double) m_stripes;
    p.fillRect( m_left_margin, (int) (m_top_margin + (1-x)*plot_height), m_stripe_width, stripe_height, doubleToColor( x ) );
    if ( i%m_step == 0 )
      p.drawText( m_left_margin+m_stripe_width + m_spacing, (int) (m_top_margin + (1-x)*plot_height-(m_step-1)*stripe_height/2), text_width, m_step*stripe_height, Qt::AlignVCenter | Qt::AlignLeft, getLabelString( m_min_value+x*(m_max_value - m_min_value) ) );
  }
}


void
ColorScaleWidget::drawColorList()
{
  QValueVector<QString>::const_iterator str_it = m_descr_vector.begin();
  QValueVector<QColor>::const_iterator color_it = m_color_vector.begin();

  int w = width(); // the current width of the widget
  int h = height(); // the current height of the widget
  QPainter p( this );

  int stripe_height = (h-m_top_margin-m_bottom_margin)/(m_color_list_length);
  int plot_height = h-m_top_margin-m_bottom_margin-stripe_height;


  int text_width = w - m_left_margin - m_stripe_width - m_spacing;

  int i,j;
  i = j = m_color_list_length-1;
  for ( ; i>=0; i--, str_it++, color_it++ ){
    if ( !(*str_it).isNull() ){ // draw only if description exists
      double x = (double) j / (double)(m_color_list_length-1);
      // the colored stripe
      p.fillRect( m_left_margin, (int) (m_top_margin + (1-x)*plot_height + .1*stripe_height), m_stripe_width, (int) (.8*stripe_height), *color_it );
      // the label
      p.drawText( m_left_margin+m_stripe_width + m_spacing, (int) (m_top_margin + (1-x)*plot_height), text_width, stripe_height, Qt::AlignVCenter | Qt::AlignLeft, *str_it );
      j--;
    }
  }

}

void
ColorScaleWidget::recalcSize()
{
  // don't change size if we were ever activated but aren't anymore.
  //if ( m_was_activated && !m_activated )
  //  return;
  int w;
  int h;

  QString label;
  if ( !m_activated ){
    w = 1;
    h = 1;
  }
  else if ( m_continuous ) { // continuous scale
    label = getLabelString( m_max_value );
    int min_label_width = fontMetrics().width( label );
    w = m_left_margin+m_stripe_width+m_spacing+min_label_width+m_right_margin;
    h = fontMetrics().height() * m_stripes / m_step;
  }
  else{
    int min_label_width = 0, x;
    QValueVector<QString>::const_iterator it = m_descr_vector.begin();

    for ( it; it!= m_descr_vector.end(); it++ ){
      label = *it;
      x = fontMetrics().width( label );
      if ( x>min_label_width )
	min_label_width = x;
    }
    w = m_left_margin+m_stripe_width+m_spacing+min_label_width+m_right_margin;
    h = fontMetrics().height() * m_color_list_length;
  }

  setMinimumWidth( w );
  setMaximumWidth( w );


  setMinimumHeight( h );
}


QString
ColorScaleWidget::getLabelString( double x )
{
  return QString("%1").arg( x, 0, 'g', 2 );
}




//#include "color_scale_widget.moc"

