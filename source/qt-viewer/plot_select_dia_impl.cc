#include "plot_select_dia_impl.hh"

#include <qlistbox.h>


#ifndef AVIDA_DATA_HH
#include "avida_data.hh"
#endif


/*
 *  Constructs a PlotSelectDiaImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 */
PlotSelectDiaImpl::PlotSelectDiaImpl( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : PlotSelectDia( parent, name, modal, fl )
{
  // set up the list box
  for ( int i=0; i < (int) AvidaData::NONE; i++ )
    m_data_lb->insertItem( AvidaData::entries[i].getDescription() );
  m_data_lb->setSelected( 0, true );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PlotSelectDiaImpl::~PlotSelectDiaImpl()
{
    // no need to delete child widgets, Qt does it all for us
}


AvidaData::eTypes
PlotSelectDiaImpl::result() const
{
  return (AvidaData::eTypes) m_data_lb->currentItem();
}



// we simply include both moc files here, makes it easier to keep track of
//#include "plot_select_dia.moc" 
//#include "plot_select_dia_impl.moc"

