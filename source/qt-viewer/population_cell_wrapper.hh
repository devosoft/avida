//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_CELL_WRAPPER_H
#define POPULATION_CELL_WRAPPER_H

#include <qobject.h>


class MessagingPopulationWrapper;
class cPopulationCell;


/**
 * PopulationCellWrapper is a convenience class used by subclasses of
 * avd_CellViewListItem to encapsulate the class <pre>cPopulation</pre>
 * from the core of Avida.  Its purpose is the signaling to these
 * subclasses that the state of the encapsulated population cell has
 * changed, i.e., that the data represented by the cell has changed, or
 * that a new cell has been chosen by the user to be represented.
 *
 * The method <pre>setPopulationCellWrapper</pre> of
 * avd_CellViewListItem is used to prepare the appropriate data
 * connections.  The parent of a subclass of avd_CellViewListItem should
 * see to performing the call.
 *
 * To notify instances of this class of changes in the state of a
 * population cell, the following sequence should be used by the
 * top-level widget list subclass, where <pre>pop_wrap</pre> is a
 * pointer to an instance of the class <pre>MessagingPopulationWrapper</pre>,
 * <pre>stateChanged_signal()</pre> signals a
 * change of data represented by the population cell, and
 * <pre>cellSelected_signal(int)</pre> signals the selection of a new
 * cell by the user:
 *
 * <pre>
 *   population_cell_wrapper = new PopulationCellWrapper(this);
 *   connect(
 *     this,
 *     SIGNAL(stateChanged_signal()),
 *     m_population_cell_wrapper,
 *     SLOT(updateState())
 *   );
 *   connect(
 *     this,
 *     SIGNAL(cellSelected_signal(int)),
 *     m_population_cell_wrapper,
 *     SLOT(setPopulationCell(int))
 *   );
 *   m_population_cell_wrapper->setPopulationWrapper(pop_wrap);
 * </pre>
 *
 * When an instance of PopulationCellWrapper receives either signal,
 * appropriate subclasses of avd_CellViewListItem will recieve the
 * signal <pre>stateChanged_sig(cPopulationCell *cell)</pre> from the
 * PopulationCellWrapper instance.
 *
 * @short Encapsulates an Avida population cell and changes to its data.
 * @version $Id: population_cell_wrapper.hh,v 1.5 2003/06/04 18:34:56 goingssh Exp $
 * @see avd_CellViewListItem
 * @see avd_WidgetListItem
 * @see avd_WidgetList
 */
class
PopulationCellWrapper : public QObject {

  Q_OBJECT

public:

  /**
   * PopulationCellWrapper constructor.
   *
   * @param parent  Usually the parent avd_WidgetList subclass.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   */
  PopulationCellWrapper(
    QObject *parent = 0,
    const char *name = 0
  );

  /**
   * sets the Avida cPopulation instance to which this class refers when
   * emitting <pre>stateChanged_sig(cPopulationCell *cell)</pre> signals.
   *
   * @param pop_wrap  The Avida MessagingPopulationWrapper.
   */
  void setPopulationWrapper(MessagingPopulationWrapper *pop_wrap);

  /**
   * accessor to obtain pointer to encapsulated avida population cell.
   *
   * @returns the encapsulated Avida cPopulationCell
   */
  cPopulationCell *getPopulationCell(void);

public slots:

  /**
   * A slot used to inform the PopulationCellWrapper instance that the
   * data represented by the encapsulated population cell has changed.
   */
  void updateState(void);

  /**
   * A slot used to inform the PopulationCellWrapper instance that the
   * encapsulated population cell should be changed.
   */
  void setPopulationCell(int cell_number);

signals:

  /**
   * A signal emitted to appropriate subclasses of avd_CellViewListItem
   * to inform them that cell data has changed.
   */
  void stateChanged_sig(PopulationCellWrapper *cw);

private:

  /**
   * The tracked Avida population.
   */
  MessagingPopulationWrapper *m_pop_wrap;

  /**
   * The tracked Avida population cell.
   */
  cPopulationCell *m_population_cell;

signals:
  void temporaryStartSig(void);
  void temporaryStopSig(void);
  void temporaryStepSig(int cell_id);
  void temporaryUpdateSig(void);
};


#endif /* !POPULATION_CELL_WRAPPER_H */

