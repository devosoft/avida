#ifndef AVIDA_DATA_HH
#define AVIDA_DATA_HH

class cPopulation;

/**
 * Class needed by @ref AvidaData. It describes one particular data
 * entry in Avida's output files, for example the average fitness.
 * It knows the filename and the column in the file in which this
 * data value can be found.
 **/

class AvidaDataEntry{
private:
  const char* m_description;
  const char* m_data_filename;
  const int m_column;

public:
  AvidaDataEntry( const char* description, const char* filename, int column ) :
    m_description( description ), m_data_filename( filename ),
    m_column( column ) {;}
  ~AvidaDataEntry() {;}

  /**
   * @return The name of the data file in which the particular type
   * of data can be found.
   **/
  const char* getDataFilename() const { return m_data_filename; }

  /**
   * @return The description of the particular type of data, e.g.,
   * "Average Fitness".
   **/
  const char* getDescription() const { return m_description; }

  /**
   * @return The column in the data file that contains the data.
   **/
  int getColumn() const { return m_column; }
};


/**
 * Contains information about different data Avida writes into files.
 *
 * If you want the viewer to support more data types, simply add a
 * corresponding enum in eTypes and add a new AvidaDataEntry corresponding
 * to your data type.
 **/
class AvidaData {
public:
  enum eTypes { AVE_MERIT         = 0,
		AVE_GESTATION_TIME,
		AVE_FITNESS,
		AVE_SIZE,
		AVE_GENERATION,
		AVE_NEUTRAL_METRIC,
		AVE_LINEAGE_LABEL,
		DOM_MERIT,
		DOM_GESTATION_TIME,
		DOM_FITNESS,
		DOM_SIZE,
		COUNT_CREATURES,
		COUNT_GENOTYPES,
		COUNT_THRESHOLD,
		COUNT_BIRTHS,
		COUNT_DEATHS,
                NONE };

  static const AvidaDataEntry entries[];

  /**
   * Returns the correct entry corresponding to the given data type.
   **/
  static AvidaDataEntry findEntry( eTypes type ){
    return entries[ (int) type ]; }
  
  /**
   * Returns the current value of the given data type from the given
   * Avida population.
   *
   * @param type The data type.
   * @param population A pointer to the corresponding population object.
   */
  static double getValue( eTypes type, cPopulation *population );
};


#endif
