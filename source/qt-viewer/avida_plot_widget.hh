#ifndef AVIDA_PLOT_WIDGET_HH
#define AVIDA_PLOT_WIDGET_HH


// porting to gcc 3.1 -- k
// i'd _really rather not use "using namespace std" in a header file,
// but it's required by our nested #includes.  which we should unnest.
// -- K.

#include "plot_widget.hh"
#include "avida_data.hh"


class avd_MissionControl;


class AvidaPlotWidget : public PlotWidget {
  Q_OBJECT
private:
  avd_MissionControl *m_mission_control;
  AvidaData::eTypes m_type;
  AvidaDataEntry m_entry;

  
  AvidaPlotWidget &operator=( AvidaPlotWidget &rhs );
  AvidaPlotWidget( AvidaPlotWidget &rhs );
  AvidaPlotWidget();
public:
  /**
   * @param avida A pointer to the avida object whose data should be plotted.
   * @param type The particular type of data to be plotted by this widget
   * (ave fitness, ave merit, and so on).
   **/
  AvidaPlotWidget(
    avd_MissionControl *mission_control,
    AvidaData::eTypes type,
    QWidget *parent = 0,
    const char * name = 0,
    WFlags f=WDestructiveClose
  );
  ~AvidaPlotWidget();

public slots:
  /**
   * Loads a file of the given name, and plots the first versus
   * the second column.
   *
   * @param filename The name of the file to plot.
   * @param colx The number of the column to plot in x-direction (starting with 1).
   * @param coly The number of the column to plot in y-direction (starting with 1).
   **/
  void load( const char* filename, int colx, int coly );

  /**
   * This slot has to be called when a new data point should be added
   * to the plot widget.
   **/
  void updateData();

  /**
   * This slot has to be called when the avida object gets destroyed, so that
   * no more calls to this object are issued.
   **/
  void avidaDestroyed();

  void helpContents();
  void helpGenesis();
  void helpInstructionSet();
  void helpEvents();
  void helpEnvironments();
  void helpAnalyzeMode();


};

#endif


