#ifndef QVBUTTONGROUP_H
#include "qvbuttongroup.h"
#endif
#ifndef QCHECKBOX_H
#include "qcheckbox.h"
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QMESSAGEBOX_H
#include <qmessagebox.h>
#endif
#ifndef QPUSHBUTTON_H
#include "qpushbutton.h"
#endif
#ifndef AVD_N_ANALYZER_LISTVIEW_HH
#include "analyzer/avd_n_analyzer_listview.hh"
#endif

#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "analyzer/avd_n_analyzer_dbg_msgs.hh"
#endif

#ifndef AVD_N_ANALYZER_GENOTYPE_SEL_VIEW_HH
#include "analyzer/avd_n_analyzer_genotype_sel_view.hh"
#endif


void avdAnalyzerGenotypeSelView::setListView(avdAnalyzerListView *view)
{ SETretainable(m_list_view, view); }

avdAnalyzerGenotypeSelView::avdAnalyzerGenotypeSelView(
  QWidget *parent, const char *name, bool modal, WFlags f
):QDialog(parent, name, modal, f)
{
  QVBoxLayout *top_layout = new QVBoxLayout(
    this, 10, -1, "<avdAnalyzerGenotypeSelView(top_layout)>");

    QVButtonGroup *bgp = new QVButtonGroup(this, "<avdAnalyzerGenotypeSelView()>");
    top_layout->addWidget(bgp);

      m_num_cpus = new QCheckBox("num_cpus", bgp, "<avdAnalyzerGenotypeSelView(m_num_cpus)>");
      m_total_cpus = new QCheckBox("total_cpus", bgp, "<avdAnalyzerGenotypeSelView(total_cpus)>");
      m_fitness = new QCheckBox("fitness", bgp, "<avdAnalyzerGenotypeSelView(m_fitness)>");
      m_merit = new QCheckBox("merit", bgp, "<avdAnalyzerGenotypeSelView(m_merit)>");
      bgp->insert(m_num_cpus);
      bgp->insert(m_total_cpus);
      bgp->insert(m_fitness);
      bgp->insert(m_merit);

    avdAnalyzerListView *lv = new avdAnalyzerListView(
      this, "<avdAnalyzerGenotypeSelView(m_list_view)>");
    setListView(lv); lv->release();
    getListView()->nameWatched("<avdAnalyzerGenotypeSelView(m_list_view)>");
    getListView()->addColumn("Genome ID");
    top_layout->addWidget(getListView());

  QHBoxLayout *btn_layout = new QHBoxLayout(
    top_layout, -1, "<avdAnalyzerGenotypeSelView(m_list_view)>");

    m_add_btn = new QPushButton(
      "Add ID", this, "<avdAnalyzerGenotypeSelView(m_add_btn)>");
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding);
    m_cancel_btn = new QPushButton(
      "Close", this, "<avdAnalyzerGenotypeSelView(m_cancel_btn)>");
    m_okay_btn = new QPushButton(
      "Okay", this, "<avdAnalyzerGenotypeSelView(m_okay_btn)>");
    btn_layout->addWidget(m_add_btn);
    btn_layout->addItem(spacer);
    btn_layout->addWidget(m_cancel_btn);
    btn_layout->addWidget(m_okay_btn);

    connect(getListView(), SIGNAL(itemRenamed(QListViewItem *, int, const QString &)),
      this, SLOT(itemRenamed(QListViewItem *, int, const QString &)));
    connect(getListView(), SIGNAL(deletePressed(QListViewItem *)),
      this, SLOT(deletePressedSlot(QListViewItem *)));
    connect(getAddBtn(), SIGNAL(clicked()), this, SLOT(addItem()));
    connect(getCancelBtn(), SIGNAL(clicked()), this, SLOT(close()));
}

void avdAnalyzerGenotypeSelView::deletePressedSlot(QListViewItem *lvi){ if(lvi) delete lvi; }
void avdAnalyzerGenotypeSelView::itemRenamed(
  QListViewItem *, int, const QString &text
){
  bool ok;
  text.toUInt(&ok);
  if(!ok){
    QMessageBox::warning(this, "Bad ID",
    "Genotype ID should be a non-negative in=teger.");
  }
}
void avdAnalyzerGenotypeSelView::addItem(){
  QListViewItem *lvi = new QListViewItem(getListView());
  lvi->setEnabled(true);
  lvi->setSelectable(true);
}

// arch-tag: definition file for analyzer genotype selector gui view
