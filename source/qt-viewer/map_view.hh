#ifndef MAP_VIEW_HH
#define MAP_VIEW_HH


// porting to gcc 3.1 -- k
// i'd _really rather not use "using namespace std" in a header file,
// but it's required by our nested #includes.  which we should unnest...
// except that some templates and inlines require nesting?  what to do?
// -- K.

#ifndef MAP_VIEW_WIDGET_HH
#include "map_view_widget.hh"
#endif

#include <qwidget.h>

class QSlider;
class QComboBox;
class QSpinBox;

//class cPopulation;
class avd_MissionControl;

class MapView : public QWidget
{
  Q_OBJECT
private:
  //cPopulation *m_population;  // a pointer to the avida population object
  avd_MissionControl *m_mission_control;  // pointer to avida population
  MapViewWidget *m_map_view_widget; // the main widget
  ColorScaleWidget *m_color_scale_widget; // the color scale widget
  QSpinBox *m_zoom_spinbox;
  QSlider *m_center_x_slider;
  QSlider *m_center_y_slider;
  QComboBox *m_mode_combo;

  void setup();

protected slots:
  void generateZoomValue( int value );
  void setMapMode( int value );

public:
  MapView(
    //cPopulation *population,
    avd_MissionControl *mission_control,
    QWidget * parent=0 );
  ~MapView();

  /**
   * This function returns the cell that is currently selected.
   **/
  int GetSelectedCell() const {
    return m_map_view_widget->GetSelectedCell(); }

public slots:
  void repaintMap();

signals:
  /**
   * This signal is emited if the zoom value has changed.
   **/
  void zoomValueChanged( double );

  /**
   * This signal is emited if a particular cell has been selected
   * by clicking on it.
   **/
  void cellSelected( int );
};


#endif


