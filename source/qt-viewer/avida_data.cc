
#include "avida_data.hh"

#include "population.hh"
#include "stats.hh"


using namespace std;


// These entries must be defined in exactly the same order as
// the enums in avida_data.hh!!

const AvidaDataEntry AvidaData::entries[] = {
  AvidaDataEntry( "Average Merit",       	"average.dat",   2 ),
  AvidaDataEntry( "Average Gestation Time",	"average.dat",   3 ),
  AvidaDataEntry( "Average Fitness",     	"average.dat",   4 ),
  AvidaDataEntry( "Average Size",       	"average.dat",   6 ),
  AvidaDataEntry( "Average Generation",       	"average.dat",   13 ),
  AvidaDataEntry( "Average Neutral Metric",     "average.dat",   14 ),
  AvidaDataEntry( "Average Lineage Label",      "average.dat",   15 ),
  AvidaDataEntry( "Dominant Merit", 		"dominant.dat",  2 ),
  AvidaDataEntry( "Dominant Gestation Time", 	"dominant.dat",  3 ),
  AvidaDataEntry( "Dominant Fitness", 		"dominant.dat",  4 ),
  AvidaDataEntry( "Dominant Size", 		"dominant.dat",  6 ),
  AvidaDataEntry( "Number of Organisms", 	"count.dat",     3 ),
  AvidaDataEntry( "Number of Genotypes", 	"count.dat",     4 ),
  AvidaDataEntry( "Number of Threshold Genotypes", "count.dat",  5 ),
  AvidaDataEntry( "Number of Births",    	"count.dat",     9 ),
  AvidaDataEntry( "Number of Deaths",    	"count.dat",     10 )
};


double AvidaData::getValue( eTypes type, cPopulation *population )
{
  double result;
  cStats & stats = population->GetStats();

  // the result depends on the data type
  switch( type ){
  case AVE_MERIT:
    result = stats.GetAveMerit();
    break;
  case AVE_GESTATION_TIME:
    result = stats.GetAveGestation();
    break;
  case AVE_FITNESS:
    result = stats.GetAveFitness();
    break;
  case AVE_SIZE:
    result = stats.GetAveSize();
    break;
  case AVE_GENERATION:
    result = stats.SumGeneration().Average();
    break;
  case AVE_NEUTRAL_METRIC:
    result = stats.GetAveNeutralMetric();
    break;
  case AVE_LINEAGE_LABEL:
    result = stats.GetAveLineageLabel();
    break;
  case DOM_MERIT:
    result = stats.GetDomMerit();
    break;
  case DOM_GESTATION_TIME:
    result = stats.GetDomGestation();
    break;
  case DOM_FITNESS:
    result = stats.GetDomFitness();
    break;
  case DOM_SIZE:
    result = stats.GetDomSize();
    break;
  case COUNT_CREATURES:
    result = stats.GetNumCreatures();
    break;
  case COUNT_GENOTYPES:
    result = stats.GetNumGenotypes();
    break;
  case COUNT_THRESHOLD:
    result = stats.GetNumThreshold();
    break;
  case COUNT_BIRTHS:
    result = stats.GetNumBirths();
    break;
  case COUNT_DEATHS:
    result = stats.GetNumDeaths();
    break;
  default:
    result = 0;
  }

  return result;
}
