
#include "map_view_widget.hh"

#include "config.hh"
#include "population.hh"

#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif
#include "avd_mission_control.hh"

#include <iostream>

#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qevent.h>


using namespace std;


//
// Construct the MapViewWidget
//

MapViewWidget::MapViewWidget(
  avd_MissionControl *mission_control,
  ColorScaleWidget *cs_widget,
  QWidget * parent, const char * name, WFlags f )
  : QScrollView( parent, name, f )
{
  m_mission_control = mission_control;
  m_world_x = cConfig::GetWorldX();
  m_world_y = cConfig::GetWorldY();
  m_zoom = .1;
  m_cell_width = 10;
  m_cell_height = 10;
  m_x_offset = 0;
  m_y_offset = 0;
  m_no_cells = m_world_x * m_world_y;

  m_x_selected = 0;
  m_y_selected = 0;

  // create the cell info stuff
  m_color_util = new MapViewCellColorUtil(
    m_mission_control,
    cs_widget
  );
  
  m_cell_info.reserve( m_no_cells );

  for( int i=0; i<m_no_cells; i++ ){
    m_mission_control->lock();
    m_cell_info.push_back(
      MapViewCellEntry(
        &(m_mission_control->getPopulation()->GetCell( i )),
        m_color_util
      )
    );
    m_mission_control->unlock();
  }

  m_pixmap = new QPixmap(  m_world_x * m_zoom * m_cell_width + 1, m_world_y * m_zoom *m_cell_height + 1 ); // used for drawing
  resizeContents( m_world_x * m_zoom * m_cell_width + 1, m_world_y * m_zoom *m_cell_height + 1 );
  resize( 500, 500 );
}

//
// Clean up
//
MapViewWidget::~MapViewWidget()
{
  delete m_pixmap;
}

void
MapViewWidget::setZoomFactor( double value )
{
// uncomment the following line if you want to play around
// with the automatic centering after zoom.
#define __AUTO_CENTER_ON_ZOOM__

  if ( value > 0 ){
#ifdef __AUTO_CENTER_ON_ZOOM__
    double old_zoom = m_zoom; // needed for automatic centering
#endif

    m_zoom = value;
    

    blockSignals( true ); // block all signals (we want to draw only once).

    /*
    m_pixmap->resize( m_world_x * m_zoom * m_cell_width + 1, m_world_y * m_zoom *m_cell_height + 1 );

    resizeContents( m_world_x * m_zoom * m_cell_width + 1, m_world_y * m_zoom *m_cell_height + 1 );
    */

    m_pixmap->resize( m_world_x * m_zoom * m_cell_width, m_world_y * m_zoom *m_cell_height);

    resizeContents( m_world_x * m_zoom * m_cell_width, m_world_y * m_zoom *m_cell_height );

    allChanged(); // mark all cells changed

    // calculate the current cell width, and determine whether we should
    // draw cell boundaries
    if ( m_zoom*m_cell_width > 7 )	
      m_draw_cell_boundaries = true;
    else
      m_draw_cell_boundaries = false;

    // get the current center:
#ifdef __AUTO_CENTER_ON_ZOOM__
    
    int width;
    int height;

    // use appropriate width to calculate new center, mapsize if it's 
    // smaller than visible area, otherwise visible area
    if( visibleWidth() < m_world_x*old_zoom*m_cell_width )
      width = visibleWidth();
    else
      width = m_world_x*old_zoom*m_cell_width;
    if( visibleHeight() < m_world_y*old_zoom*m_cell_height )
      height = visibleHeight();
    else
      height = m_world_y*old_zoom*m_cell_height;
    
    // recalculate the new center:
    double dcx = contentsX() + width/2.;
    double dcy = contentsY() + height/2.;
    int cx = (int) (dcx*m_zoom/old_zoom);
    int cy = (int) (dcy*m_zoom/old_zoom);
    center( cx, cy ); // make sure the center stays the same after zoom
#endif

    //if zooming out so map is getting smaller, first erase then draw
    bool redraw = false;            
    if (old_zoom > m_zoom) redraw = true;

    drawViewport(redraw);
    blockSignals( false ); // now we can allow signals again.
  }

#ifdef __AUTO_CENTER_ON_ZOOM__
#undef __AUTO_CENTER_ON_ZOOM__
#endif
}

void
MapViewWidget::setXOffset( int value )
{
  if ( value >= 0 || value < m_world_x ){
    // reverse offset for better user experience
    m_x_offset = m_world_x - 1 - value;
    allChanged(); // mark all cells as changed
    drawViewport(false);
  }
}

void
MapViewWidget::setYOffset( int value )
{
  if ( value >= 0 || value < m_world_y ){
    // reverse offset for better user experience
    m_y_offset = m_world_y - 1 - value;
    allChanged(); // mark all cells as changed
    drawViewport(false);
  }
}

void
MapViewWidget::setMapMode( MapViewCellColorUtil::MapMode mode )
{
  m_color_util->setMapMode( mode );
  drawViewport(false);
}

void
MapViewWidget::allChanged()
{
  // This function is called when everything needs to be redrawn
  // (for example because of a change in zoom),
  // so that it is not sufficient anymore not to redraw only if
  // the color of a cell is still the same. Therefore, we have
  // to inform all cells of that.

  vector<MapViewCellEntry>::iterator it = m_cell_info.begin();
  for ( ; it != m_cell_info.end(); it++ ){
    (*it).setChanged();
  }
}

// coordinate transf.
int
MapViewWidget::translX( int x )
{
  x += m_x_offset;
  x %= m_world_x;
  return x;
}

int
MapViewWidget::translY( int y )
{
  y += m_y_offset;
  y %= m_world_y;
  return y;
}


QPoint
MapViewWidget::upLeft( int x, int y )
{
  return QPoint( m_zoom*x*m_cell_width, m_zoom*y*m_cell_height );
}

QPoint
MapViewWidget::upRight( int x, int y )
{
  return QPoint( m_zoom*(x+1)*m_cell_width, m_zoom*y*m_cell_height );
}

QPoint
MapViewWidget::lowLeft( int x, int y )
{
  return QPoint( m_zoom*x*m_cell_width, m_zoom*(y+1)*m_cell_height );
}

QPoint
MapViewWidget::lowRight( int x, int y )
{
  return QPoint( m_zoom*(x+1)*m_cell_width, m_zoom*(y+1)*m_cell_height );
}

int
MapViewWidget::screenToCellX( int x )
{
  int maxwidth = (int) (m_zoom*m_world_x*m_cell_width);
  if ( x >= maxwidth ) x = maxwidth-1;

  int col = (int) (x/(m_cell_width * m_zoom)) - m_x_offset;
  col %= m_world_x;
  if ( col < 0 ) col += m_world_x;
  return col;
}

int
MapViewWidget::screenToCellY( int y )
{
  int maxheight = (int) (m_zoom*m_world_y*m_cell_height);
  if ( y >= maxheight ) y = maxheight-1;

  int row = (int) (y/(m_cell_height * m_zoom)) - m_y_offset;
  row %= m_world_y;
  if ( row < 0 ) row += m_world_y;
  return row;
}


void
MapViewWidget::drawViewport(bool redraw)
{
  viewport()->repaint( redraw );
}

void
MapViewWidget::drawContents( QPainter *p, int clipx, int clipy, int clipw, int cliph )
{
  // we always redraw the whole viewport. Otherwise, it doesn't really
  // work right now.
  clipx = contentsX();
  clipy = contentsY();
  clipw = visibleWidth();
  cliph = visibleHeight();

  /*
  cheap hack...

  under OS X / Aqua, if the blitted pixmap is too small, the Qt bitBlt
  will stretch the pixmap to fit the target dimensions.  under linux,
  the stretch does not occur.

  The following prevents the stretching under OS X / Aqua, giving us
  consistent behavior between the two platforms.

  -- kaben.
  */
  if(m_pixmap->width() < clipw) clipw = m_pixmap->width();
  if(m_pixmap->height() < cliph) cliph = m_pixmap->height();
  /* end cheap hack. */

  QPainter p2( m_pixmap );

  int xlo = screenToCellX( clipx );
  int xhi = screenToCellX( clipx + clipw );
  int ylo = screenToCellY( clipy );
  int yhi = screenToCellY( clipy + cliph );

  if ( xlo > xhi ) xhi += m_world_x;
  if ( ylo > yhi ) yhi += m_world_y;

  // let the cell color utility figure out the correct colors
  m_color_util->calcColorScale();

  for ( int x = xlo; x <= xhi; x++ )
    for ( int y = ylo; y <= yhi; y++ ){
      int xm = x % m_world_x;
      int ym = y % m_world_y;
      int i = xm + m_world_x * ym;
      if ( m_cell_info[i].hasChanged() ){
	drawCell( &p2, xm, ym,  m_cell_info[i].getColor() );
      }
    }

  // finally, draw the selected cell
  drawSelected( &p2 );

  p2.end();

  bitBlt ( p->device(), 0, 0, m_pixmap, clipx, clipy, clipw, cliph );
}



void
MapViewWidget::drawCell( QPainter *p, int x, int y, QColor color )
{
  QBrush b1 = QBrush( color );
  x = translX( x );  // shift the origin
  y = translY( y );
  QPoint ul = upLeft( x, y );  // calculate screen coordinates
  QPoint lr = lowRight( x, y );
  if ( m_draw_cell_boundaries ){
    p->setBrush( b1 );
    p->drawRect( ul.x(), ul.y(), lr.x()-ul.x(), lr.y()-ul.y() );
  }
  else {
    p->fillRect( ul.x(), ul.y(), lr.x()-ul.x(), lr.y()-ul.y(), b1 );
  }
}


void
MapViewWidget::drawSelected( QPainter *p )
{
  int x = translX( m_x_selected );  // shift the origin
  int y = translY( m_y_selected );
  QPoint ul = upLeft( x, y );  // calculate screen coordinates
  QPoint lr = lowRight( x, y );

  // draw only if we also draw cell boundaries
  if ( m_draw_cell_boundaries ){
    QPen pen( black, 2 ); // use black, fat pen
    p->setPen( pen );
    p->drawLine( ul.x()+1, ul.y()+1, lr.x()-1, lr.y()-1 );
    p->drawLine( ul.x()+1, lr.y()-1, lr.x()-1, ul.y()+1 );
  }
}

void
MapViewWidget::contentsMouseReleaseEvent( QMouseEvent *event )
{
  int old_cell = m_x_selected + m_world_x * m_y_selected;
  m_x_selected = screenToCellX( event->x() );
  m_y_selected = screenToCellY( event->y() );

  m_cell_info[ old_cell ].setChanged();

  emit cellSelected( m_x_selected
		     + m_world_x * m_y_selected );
  drawViewport(false);
}

//#include "map_view_widget.moc"



