
#include "map_view_cell_entry.hh"

#include "population.hh"
#include "stats.hh"
#include "population_cell.hh"
#include "genotype.hh"
#include "genebank.hh"
#include "phenotype.hh"
#include "organism.hh"

#include "avd_mission_control.hh"

#include "plot_widget.hh"
#include "color_scale_widget.hh"

#include <iostream>


using namespace std;


const char *MapViewCellColorUtil::m_mode_names[]
            = { "merit",
		"fitness",
		"age",
		"genotype",
		"lineage",
		"parasites",
		"off" };

AgeColorUtil::AgeColorUtil( ColorScaleWidget *cs_widget )
  : m_color_scale_widget( cs_widget )
{
  assert( cs_widget != 0 );

  m_color_vector.push_back( Qt::lightGray);
  m_name_vector.push_back( ">200 Updates" );
  m_color_vector.push_back( Qt::darkBlue );
  m_name_vector.push_back( "181-200 Updates" );
  m_color_vector.push_back( Qt::darkCyan );
  m_name_vector.push_back( "161-180 Updates" );
  m_color_vector.push_back( Qt::darkYellow );
  m_name_vector.push_back( "141-160 Updates" );
  m_color_vector.push_back( Qt::darkGreen );
  m_name_vector.push_back( "121-140 Updates" );
  m_color_vector.push_back( Qt::green );
  m_name_vector.push_back( "101-120 Updates" );
  m_color_vector.push_back( Qt::cyan );
  m_name_vector.push_back( "81-100 Updates" );
  m_color_vector.push_back( Qt::blue );
  m_name_vector.push_back( "61-80 Updates" );
  m_color_vector.push_back( Qt::yellow );
  m_name_vector.push_back( "41-60 Updates" );
  m_color_vector.push_back( Qt::magenta );
  m_name_vector.push_back( "21-40 Updates" );
  m_color_vector.push_back( Qt::red );
  m_name_vector.push_back( "0-20 Updates" );
}

void
AgeColorUtil::setupColorScale( bool force_redraw)
{
  if (force_redraw){
    m_color_scale_widget->setColorList( m_color_vector, m_name_vector );
    m_color_scale_widget->activate();
  }
}

QColor
AgeColorUtil::getAgeColor( int age ) const
{
  if ( age>0 )
    age -= 1;
  int i = age / 20;
  if ( i>10 )
    i=0;
  else
    i=10-i;
  return m_color_vector[i];
}


ParasiteColorUtil::ParasiteColorUtil( ColorScaleWidget *cs_widget )
  : m_color_scale_widget( cs_widget )
{
  assert( cs_widget != 0 );

  m_color_vector.push_back( Qt::lightGray);
  m_name_vector.push_back( "Uninfected" );
  m_color_vector.push_back( Qt::yellow );
  m_name_vector.push_back( "Infected" );
  m_color_vector.push_back( Qt::red );
  m_name_vector.push_back( "Virulent" );
  m_color_vector.push_back( Qt::blue );
  m_name_vector.push_back( "Infected/Virulent" );
}

void
ParasiteColorUtil::setupColorScale( bool force_redraw)
{
  if (force_redraw){
    m_color_scale_widget->setColorList( m_color_vector, m_name_vector );
    m_color_scale_widget->activate();
  }
}

QColor
ParasiteColorUtil::getParasiteColor( int status ) const
{
  return m_color_vector[status];
}


GenotypeColorUtil::GenotypeColorUtil(
  avd_MissionControl *mission_control,
  ColorScaleWidget *cs_widget
)
: m_color_scale_widget( cs_widget ),
  m_mission_control( mission_control )
{
  assert( m_mission_control != 0 );
  assert( m_color_scale_widget != 0 );
  /*
  m_color_vector.push_back( Qt::red );
  m_color_vector.push_back( Qt::green );
  m_color_vector.push_back( Qt::blue );
  m_color_vector.push_back( Qt::cyan );
  m_color_vector.push_back( Qt::magenta );
  m_color_vector.push_back( Qt::yellow );
  m_color_vector.push_back( Qt::darkRed );
  m_color_vector.push_back( Qt::darkGreen );
  m_color_vector.push_back( Qt::darkBlue );
  m_color_vector.push_back( Qt::darkCyan );
  m_color_vector.push_back( Qt::darkMagenta );
  m_color_vector.push_back( Qt::darkYellow );
  m_color_vector.push_back( Qt::white );
  m_color_vector.push_back( Qt::darkGray );
  m_max_color = 12;
  */
  m_max_color = 36;
  for(int i = 0; i < m_max_color; i++){
    m_color_vector.push_back(QColor((i*360/m_max_color)%360, 255, 255, QColor::Hsv));
  }
  m_color_vector.push_back( Qt::white );
  m_color_vector.push_back( Qt::darkGray );
  /**/

  m_genotype_id_vector.resize( m_max_color );
  QValueVector<int>::iterator it = m_genotype_id_vector.begin();
  for ( ; it != m_genotype_id_vector.end(); it++)
    *it = -1;
  m_name_vector.resize( m_max_color );
  QValueVector<QString>::iterator it2 = m_name_vector.begin();
  for ( ; it2 != m_name_vector.end(); it2++ )
    *it2 = QString::null;
  m_name_vector.push_back( "threshold genotype" );
  m_name_vector.push_back( "non-thresh. genotype" );
  m_used_vector.resize( m_max_color );
  m_new_genotypes.resize( m_max_color );
}

void
GenotypeColorUtil::setupColorScale( bool )
{
  // mark all positions as empty
  QValueVector<bool>::iterator it = m_used_vector.begin();
  for ( ; it != m_used_vector.end(); it++ )
    *it = false;

  // initially, we have no genotypes that are not yet entered
  // into the color map.
  int max_new_genotype = 0;

  // now, give colors for the first m_max_color positions
  m_mission_control->lock();
  cGenotype *temp_gen =
    m_mission_control->getPopulation()->GetGenebank().GetBestGenotype();
  m_mission_control->unlock();

  int i=0;
  do{
    // exit loop if we don't have a valid genotype anymore
    // or if the current genotype is not threshold anymore
    if ( !temp_gen || !temp_gen->GetThreshold() )
      break;

    // check if genotype exists already in color chart
    int pos = findGenotypeInColorList( temp_gen );
    if ( pos >= 0 ) // yes, so mark as used
      m_used_vector[pos] = true;
    else{ // otherwise, store in list
      m_new_genotypes[max_new_genotype] = temp_gen;
      max_new_genotype++;
    }
    temp_gen = temp_gen->GetNext();
    i++;
  } while ( i < m_max_color );

  // i marks the positions in the genotype-color map,
  // j marks the positions in the list of new genotypes.
  int j=i=0;
  while ( j<max_new_genotype ){
    while ( m_used_vector[i] && i<m_max_color ) // find the next empty slot
      i++;
    if ( i>=m_max_color ) // no more slots available
      break;
    temp_gen = m_new_genotypes[j];
    m_genotype_id_vector[i] = temp_gen->GetID();
    m_name_vector[i] = QString( temp_gen->GetName() );
    m_used_vector[i] = true;
    i++; j++;
  }

  // set the remaining unused names to
  // null (will prevent the entries to be drawn).
  for ( ; i < m_max_color; i++ )
    if ( !m_used_vector[i] )
      m_name_vector[i] = QString::null;
  m_color_scale_widget->setColorList( m_color_vector, m_name_vector );
  m_color_scale_widget->activate();
}


int
GenotypeColorUtil::findGenotypeInColorList( cGenotype *g ) const
{
  int id = g->GetID();
  int pos = -1;
  for ( int i=0; i<m_max_color; i++ )
    if ( m_genotype_id_vector[i] == id ){
      pos = i;
      break;
    }
  return pos;
}


QColor
GenotypeColorUtil::getGenotypeColor( cGenotype *genotype ) const
{
  QColor state;
  int pos = findGenotypeInColorList( genotype );
  if ( pos >= 0 ){
    state = m_color_vector[pos];
  }
  else {
    if ( genotype->GetThreshold() )
      state = Qt::white;
    else
      state = Qt::darkGray;
  }
  return state;
}

LineageColorUtil::LineageColorUtil( ColorScaleWidget *cs_widget )
  : m_color_scale_widget( cs_widget )
{
  assert( cs_widget != 0 );

  m_color_vector.push_back( Qt::red );
  m_name_vector.push_back( "0" );
  m_color_vector.push_back( Qt::green );
  m_name_vector.push_back( "1" );
  m_color_vector.push_back( Qt::magenta );
  m_name_vector.push_back( "2" );
  m_color_vector.push_back( Qt::cyan );
  m_name_vector.push_back( "3" );
  m_color_vector.push_back( Qt::yellow );
  m_name_vector.push_back( "4" );
  m_color_vector.push_back( Qt::blue );
  m_name_vector.push_back( "5" );
  m_color_vector.push_back( Qt::white );
  m_name_vector.push_back( "other" );
}

void
LineageColorUtil::setupColorScale( bool force_redraw)
{
  if (force_redraw){
    m_color_scale_widget->setColorList( m_color_vector, m_name_vector );
    m_color_scale_widget->activate();
  }
}

QColor
LineageColorUtil::getLineageColor( int lineage ) const
{
  if ( lineage<0 || lineage>6 )
    lineage = 6;
  return m_color_vector[lineage];
}


MapViewCellColorUtil::MapViewCellColorUtil(
  avd_MissionControl *mission_control,
  ColorScaleWidget *cs_widget
)
  : m_color_scale_widget( cs_widget ),
    m_mission_control( mission_control ),
    m_age_color_util( cs_widget ),
    m_parasite_color_util( cs_widget ),
    m_genotype_color_util( mission_control, cs_widget ),
    m_lineage_color_util( cs_widget ),
    m_cs_min_value( 1e20 ), // set some absurd values here
    m_cs_value_range( -1 ) // dito
{
  assert( m_mission_control != 0 );
}

void
MapViewCellColorUtil::setupColorScale( double min, double max, bool force_redraw )
{
  double old_min = m_cs_min_value;
  double old_range = m_cs_value_range;

  PlotCanvas::calcAxisRange( min, max, &m_cs_min_value, &m_cs_value_range );
  m_cs_value_range -= m_cs_min_value;
  if ( !force_redraw && m_cs_value_range < old_range )
    m_cs_value_range = old_range;
  if ( force_redraw || old_min != m_cs_min_value || old_range != m_cs_value_range ){
    m_color_scale_widget->setRange( m_cs_min_value, m_cs_min_value + m_cs_value_range );
    m_color_scale_widget->activate();
  }
}

void
MapViewCellColorUtil::calcColorScale( bool force_redraw )
{
  switch( m_mode ){
  case MERIT:
    m_mission_control->lock();
    setupColorScale(
      0,
      m_mission_control->getPopulation()->GetStats().GetMaxMerit(),
      force_redraw
    );
    m_mission_control->unlock();
    break;
  case FITNESS:
    m_mission_control->lock();
    setupColorScale(
      0,
      m_mission_control->getPopulation()->GetStats().GetMaxFitness(),
      force_redraw
    );
    m_mission_control->unlock();
    break;
  case AGE:
    m_age_color_util.setupColorScale( force_redraw );
    break;
  case GENOTYPE:
    m_genotype_color_util.setupColorScale( force_redraw );
    break;
  case LINEAGE:
    m_lineage_color_util.setupColorScale( force_redraw );
    break;
  case PARASITE:
    m_parasite_color_util.setupColorScale( force_redraw );
    break;
  case OFF:
    m_color_scale_widget->activate( false );
    break;
  }

}

QColor
MapViewCellColorUtil::doubleToColor( double x ) const
{
  double y;
  if ( m_cs_value_range == 0 )
    y = 1;
  else
    y = (x-m_cs_min_value)/m_cs_value_range;
  return ColorScaleWidget::doubleToColor( y );
}

QColor
MapViewCellColorUtil::calcCellState( cPopulationCell *cell ) const
{
  QColor state = Qt::black;
  double x; int i;

  if ( cell->IsOccupied() ){
    cPhenotype &phenotype = cell->GetOrganism()->GetPhenotype();
    switch( m_mode ){
    case MERIT:
      x = phenotype.GetMerit().GetDouble();
      state = doubleToColor( x );
      break;
    case FITNESS:
      x = phenotype.GetFitness();
      state = doubleToColor( x );
      break;
    case AGE:
      state = m_age_color_util.getAgeColor( phenotype.GetAge() );
      break;
    case GENOTYPE:
      state = m_genotype_color_util.getGenotypeColor(
	 cell->GetOrganism()->GetGenotype() );
      break;
    case LINEAGE:
      state = m_lineage_color_util.getLineageColor(
	 cell->GetOrganism()->GetLineageLabel() );
      break;
    case PARASITE:
      i = 0;
      if ( phenotype.IsModified() )
	i += 1;
      if ( phenotype.IsModifier() )
	i += 2;
      state = m_parasite_color_util.getParasiteColor( i );
      break;
    case OFF:
      break;
    }
  }

  return state;
}

MapViewCellEntry::MapViewCellEntry( cPopulationCell *cell, MapViewCellColorUtil *color_util )
    : m_cell( cell ), m_color_util( color_util ), m_changed( true ),
      m_state( Qt::black )
{
  assert( m_cell != 0 );
  assert( m_color_util != 0 );

  hasChanged();
}

MapViewCellEntry::~MapViewCellEntry()
{
}



bool
MapViewCellEntry::hasChanged()
{
  // the trick of this function is that it sets m_changed to true
  // in certain cases, but never to false.
  QColor new_state = m_color_util->calcCellState( m_cell );

  if ( m_state != new_state ){
    m_changed = true;
    m_state = new_state;
  }
  return m_changed;
}










