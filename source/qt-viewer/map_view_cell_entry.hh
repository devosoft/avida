// -*- c++ -*-
#ifndef MAP_VIEW_CELL_ENTRY_HH
#define MAP_VIEW_CELL_ENTRY_HH

#include "color_scale_widget.hh"

#include <iostream>
#include <qcolor.h>
#include <qvaluevector.h>

class cGenotype;
class avd_MissionControl;
class cPopulation;
class cPopulationCell;

class AgeColorUtil {
private:
  ColorScaleWidget *m_color_scale_widget;

  QValueVector<QColor> m_color_vector;
  QValueVector<QString> m_name_vector;

  AgeColorUtil();
  AgeColorUtil( AgeColorUtil& );
public:
  AgeColorUtil( ColorScaleWidget *cs_widget );

  /**
   * Calculate and draw the age color scale.
   *
   * The color scale is only redrawn if force_redraw is set
   * to true (because currently, the color scale never changes).
   **/
  void setupColorScale( bool force_redraw = false );

  /**
   * Get the color for the given age
   **/
  QColor getAgeColor( int age ) const;
};


class ParasiteColorUtil {
private:
  ColorScaleWidget *m_color_scale_widget;

  QValueVector<QColor> m_color_vector;
  QValueVector<QString> m_name_vector;

  ParasiteColorUtil();
  ParasiteColorUtil( ParasiteColorUtil& );
public:
  ParasiteColorUtil( ColorScaleWidget *cs_widget );

  /**
   * Calculate and draw the age color scale.
   *
   * The color scale is only redrawn if force_redraw is set
   * to true (because currently, the color scale never changes).
   **/
  void setupColorScale( bool force_redraw = false );

  /**
   * Get the color for the given parasite status
   **/
  QColor getParasiteColor( int status ) const;
};


class GenotypeColorUtil {
private:
  ColorScaleWidget *m_color_scale_widget;

  //population object from which info is taken
  avd_MissionControl *m_mission_control;

  QValueVector<int> m_genotype_id_vector;
  QValueVector<QColor> m_color_vector;
  QValueVector<QString> m_name_vector;
  QValueVector<bool> m_used_vector;
  QValueVector<cGenotype *> m_new_genotypes;
  int m_max_color; // the last position of a color in m_color_vector
  // (after that, we have still gray and white, however).

  GenotypeColorUtil();
  GenotypeColorUtil( GenotypeColorUtil& );
public:
  GenotypeColorUtil(
    avd_MissionControl *mission_control,
    ColorScaleWidget *cs_widget
  );

  /**
   * Calculate the correct color scale, depending on the current state of
   * the population.
   *
   * Currently, force_redraw is ignored, and the color scale is redrawn
   * in any case.
   **/
  void setupColorScale( bool force_redraw );

  /**
   * Get the genotype color according to the current genotype-color map.
   **/
  QColor getGenotypeColor( cGenotype *genotype ) const;

protected:
  /**
   * Finds the position of the given genotype in the current genotype
   * color map. Returns -1 if the genotype is not found.
   **/
  int findGenotypeInColorList( cGenotype *genotype ) const;
};

class LineageColorUtil {
private:
  ColorScaleWidget *m_color_scale_widget;

  QValueVector<QColor> m_color_vector;
  QValueVector<QString> m_name_vector;

  LineageColorUtil();
  LineageColorUtil( AgeColorUtil& );
public:
  LineageColorUtil( ColorScaleWidget *cs_widget );

  /**
   * Calculate and draw the age color scale.
   *
   * The color scale is only redrawn if force_redraw is set
   * to true (because currently, the color scale never changes).
   **/
  void setupColorScale( bool force_redraw = false );

  /**
   * Get the color for the given lineage
   **/
  QColor getLineageColor( int lineage ) const;
};

class MapViewCellColorUtil {
public:
  // always leave OFF as the last mode of these enums!
  // this is needed to loop over all map modes (for example in
  // map_view.cc).
  enum MapMode{ MERIT, FITNESS, AGE, GENOTYPE, LINEAGE, PARASITE, OFF };

private:
  // fill in this array with the names for the above modes
  // (in map_view_cell_entry.cc).
  static const char * m_mode_names[];

  ColorScaleWidget *m_color_scale_widget;
  //the population object from which all info is taken
  avd_MissionControl *m_mission_control;
  AgeColorUtil m_age_color_util;
  ParasiteColorUtil m_parasite_color_util;
  GenotypeColorUtil m_genotype_color_util;
  LineageColorUtil m_lineage_color_util;

  MapMode m_mode; //the current mode of the map
  // the following variables are used if there is a continuous color scale
  double m_cs_min_value;
  double m_cs_value_range;

  /**
   * Transforms an integer between 0 and 360 into a color.
   **/
  QColor calcColor( int i ) const {
    return ColorScaleWidget::doubleToColor( (double) i/ 360. ); }


  MapViewCellColorUtil();
  MapViewCellColorUtil( MapViewCellColorUtil& );
public:
  MapViewCellColorUtil(
    avd_MissionControl *mission_control,
    ColorScaleWidget *cs_widget
  );


  /**
   * Calculate the correct color scale, depending on the current state of
   * the population and on the settings of the map mode.
   *
   * If force_redraw is set to true, the color scale is redrawn in any
   * case. Otherwise, it is redrawn only if it has changed.
   **/

  void calcColorScale( bool force_redraw = false );

  void setMapMode( MapMode mode ){ m_mode = mode; calcColorScale( true ); }

  MapMode getMapMode() const{
    return m_mode; }

  static const char * getModeName( MapMode mode ) {
    return m_mode_names[mode]; }

  QColor calcCellState( cPopulationCell *cell ) const;

protected:
  /**
   * Sets up the correct color scale, based on the values min, max
   * and force_redraw, which states whether the color scale needs to
   * be redrawn in any case or not.
   *
   * Function is used by calcColorScale().
   **/
  void setupColorScale( double min, double max, bool force_redraw );

  /**
   * Transforms a double value into a color, taking into account the
   * current values of m_cs_min_value and m_cs_value_range;
   **/
  QColor doubleToColor( double x ) const;
};


class MapViewCellEntry {
private:
  cPopulationCell *m_cell;
  MapViewCellColorUtil *m_color_util;
  bool m_changed;
  QColor m_state;

protected:
  int calcState();

public:
  MapViewCellEntry( cPopulationCell *cell, MapViewCellColorUtil *color_util );
  ~MapViewCellEntry();

  void setEnvironment( cPopulationCell *cell ){
    m_cell = cell;
  }

  void setChanged( bool val=true ){
    m_changed = val; }

  /**
   * Checks whether the state of the cell has changed.
   * The return value is 'true' if the state has changed since the
   * last call of getColor(), otherwise the return value is false.
   **/
  bool hasChanged();

  /**
   * Returns the current color of the cell. Always call 'hasChanged()'
   * before calling this function.
   **/
  QColor getColor(){
    m_changed = false;
    return m_state;
  }
};


#endif










