
#include "map_view.hh"

#include "config.hh"
#include "avd_mission_control.hh"

#include "map_view_widget.hh"

#include <iostream>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qslider.h>
#include <qspinbox.h>


using namespace std;


//
// Construct the MapViewWidget
//

MapView::MapView(
  avd_MissionControl *mission_control,
  QWidget * parent
)
: QWidget( parent ),
  m_mission_control( mission_control )
{
  // set the window caption
  setCaption( "Avida - Map View" );

  // setup all sub-widgets
  setup();
}

void
MapView::setup()
{
  // top-level layout
  QVBoxLayout *layout = new QVBoxLayout( this );
  
  QGridLayout *glayout = new QGridLayout();

  // the color scale
  //m_color_scale_widget = new ColorScaleWidget( this );
  //glayout->addWidget( m_color_scale_widget, 0, 2 ); 
  QScrollView *scrollview = new QScrollView(this, "<MapView::(scrollview)>");
  glayout->addWidget( scrollview, 0, 2 ); 

  scrollview->setResizePolicy(QScrollView::AutoOneFit);
  QWidget *layout_widget = new QWidget(
    scrollview->viewport(), "<MapView::(layout_widget)>");
  scrollview->addChild(layout_widget);
  QHBoxLayout *sublayout = new QHBoxLayout(layout_widget);
  //vbox = new QVBox(layout_widget, "<MapView::(vbox)>");
  //layout->addWidget(vbox);
  //layout->addStretch(10);
  m_color_scale_widget = new ColorScaleWidget( layout_widget );
  scrollview->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  //scrollview->setHScrollBarMode(QScrollView::AlwaysOff);
  sublayout->addWidget(m_color_scale_widget);
  /* Yuck. */
  sublayout->addStretch(10);
  glayout->setColStretch(0, 1);

  //m_color_scale_widget = new ColorScaleWidget(scrollview->viewport());
  //scrollview->addChild(m_color_scale_widget);
  //scrollview->setVScrollBarMode(QScrollView::Auto);
  //scrollview->setHScrollBarMode(QScrollView::Auto);

  // the map
  m_map_view_widget = new MapViewWidget(
    m_mission_control,
    m_color_scale_widget,
    this
  );
  glayout->addWidget( m_map_view_widget, 0, 0 );

  // the sliders that can be used to set the origin
  int world_x = cConfig::GetWorldX();
  int world_y = cConfig::GetWorldY();

  m_center_x_slider = new QSlider( QSlider::Horizontal, this );
  m_center_x_slider->setRange( 0, world_x-1 );
  glayout->addWidget( m_center_x_slider, 1, 0 );

  connect( m_center_x_slider, SIGNAL( valueChanged ( int ) ),
	   m_map_view_widget, SLOT( setXOffset( int ) ) );
  m_center_x_slider->setValue( world_x/2 );

  m_center_y_slider = new QSlider( QSlider::Vertical, this );
  m_center_y_slider->setRange( 0, world_y-1 );
  glayout->addWidget( m_center_y_slider, 0, 1 );

  connect( m_center_y_slider, SIGNAL( valueChanged ( int ) ),
	   m_map_view_widget, SLOT( setYOffset( int ) ) );
  m_center_y_slider->setValue( world_y/2 );

  layout->addLayout( glayout );
  
  // the bottom line for zoom and mode
  QHBoxLayout *hlayout = new QHBoxLayout();
  hlayout->addWidget( new QLabel( "Map mode: ", this ) );
  m_mode_combo = new QComboBox( false, this );
  for ( int i = 0; i<= MapViewCellColorUtil::OFF; i++ )
    m_mode_combo->insertItem(
      MapViewCellColorUtil::getModeName( static_cast<MapViewCellColorUtil::MapMode>( i ) ) );
  hlayout->addWidget( m_mode_combo );
  hlayout->addStretch( 3 );

  connect( m_mode_combo, SIGNAL( activated( int ) ),
	   this, SLOT( setMapMode( int ) ) );

  // select "genotype" map mode.
  m_mode_combo->setCurrentItem(3);
  setMapMode( 3 );

  // add box for zoom value
  hlayout->addWidget( new QLabel( "Zoom: ", this ) );
  m_zoom_spinbox = new QSpinBox( this );
  m_zoom_spinbox->setMinValue( 10 );
  m_zoom_spinbox->setMaxValue( 500 );
  m_zoom_spinbox->setLineStep( 10 );
  hlayout->addWidget( m_zoom_spinbox);

  layout->addLayout( hlayout );

  connect( m_zoom_spinbox, SIGNAL( valueChanged ( int ) ),
	   this, SLOT( generateZoomValue( int ) ) );
  connect( this, SIGNAL( zoomValueChanged( double ) ),
	   m_map_view_widget, SLOT( setZoomFactor( double ) ) );
  m_zoom_spinbox->setValue( 50 );
  
  // activate the layout
  layout->activate();
  
  connect( m_map_view_widget, SIGNAL( cellSelected( int ) ),
	   this, SIGNAL( cellSelected( int ) ) );
}

//
// Clean up
//
MapView::~MapView()
{
}


void
MapView::repaintMap()
{
  m_map_view_widget->drawViewport(false);
}

void
MapView::generateZoomValue( int value )
{
  emit zoomValueChanged( value/100. );
}

void
MapView::setMapMode( int value )
{
  m_map_view_widget->setMapMode( static_cast<MapViewCellColorUtil::MapMode>( value ) );
}

//#include "map_view.moc"



