#ifndef QHEADER_H
#include <qheader.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif
#ifndef QMENUBAR_H
#include <qmenubar.h>
#endif
#ifndef QSPLITTER_H
#include <qsplitter.h>
#endif

#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_VIEW_HH
#include "avd_analyzer_view.hh"
#endif


struct avd_o_AnalyzerViewData {
  QListView *m_batch_lv, *m_genotype_lv; 
};

QListView *avd_o_AnalyzerView::getBatchesListView(void){
  return (d)?(d->m_batch_lv):(0); }
QListView *avd_o_AnalyzerView::getGenotypesListView(void){
  return (d)?(d->m_genotype_lv):(0); }

avd_o_AnalyzerView::~avd_o_AnalyzerView(){
  AnGuiDebug << "destructor.";
}

avd_o_AnalyzerView::avd_o_AnalyzerView(
  QWidget *parent,
  const char *name,
  WFlags f
): QMainWindow(parent, name, f)
{
  AnGuiDebug << "entered.";

  d = new avd_o_AnalyzerViewData;

  QWidget *central_widget = new QWidget(
    this, "<avd_o_AnalyzerView::avd_o_AnalyzerView(central_widget)>");
  QVBoxLayout *top_layout = new QVBoxLayout(
    central_widget, 10, -1,
    "<avd_o_AnalyzerView::avd_o_AnalyzerView(top_layout)>");
  setCentralWidget(central_widget);

    //QLabel *experiment_editor_label = new QLabel(
    //  central_widget,
    //  "<avd_o_AnalyzerView::avd_o_AnalyzerView(experiment_editor_label)>"
    //);
    //experiment_editor_label->setText("experiment_editor_label.");
    //top_layout->addWidget(experiment_editor_label);

    QSplitter *horizontal_splitter = new QSplitter(
      Qt::Horizontal, central_widget,
      "<avd_o_AnalyzerView::avd_o_AnalyzerView(horizontal_splitter)>");
    horizontal_splitter->setOpaqueResize(true);
    top_layout->addWidget(horizontal_splitter, 1);

      QWidget *batch_layout_widget = new QWidget(
        horizontal_splitter,
        "<avd_o_AnalyzerView::avd_o_AnalyzerView(batch_layout_widget)>");
      batch_layout_widget->setSizePolicy(
        QSizePolicy::Preferred,
        batch_layout_widget->sizePolicy().verData(),
        batch_layout_widget->sizePolicy().hasHeightForWidth());
      horizontal_splitter->setResizeMode(
        batch_layout_widget, QSplitter::KeepSize);
      QVBoxLayout *selector_layout = new QVBoxLayout(
        batch_layout_widget, 10, -1,
        "<avd_o_AnalyzerView::avd_o_AnalyzerView(selector_layout)>");
      horizontal_splitter->moveToFirst(batch_layout_widget);

        d->m_batch_lv = new QListView(
          batch_layout_widget,
          "<avd_o_AnalyzerView::avd_o_AnalyzerView(m_batch_lv)>");
        d->m_batch_lv->setSorting(-1);
        d->m_batch_lv->addColumn("Batch Name");
        selector_layout->addWidget(d->m_batch_lv);

      QWidget *genotype_layout_widget = new QWidget(
        horizontal_splitter,
        "<avd_o_AnalyzerView::avd_o_AnalyzerView(genotype_layout_widget)>");
      QVBoxLayout *genotype_layout = new QVBoxLayout(
        genotype_layout_widget, 10, -1,
        "<avd_o_AnalyzerView::avd_o_AnalyzerView(genotype_layout)>");
      horizontal_splitter->moveToLast(genotype_layout_widget);

        d->m_genotype_lv = new QListView(
          genotype_layout_widget,
          "<avd_o_AnalyzerView::avd_o_AnalyzerView(m_genotype_lv)>");
        d->m_genotype_lv->setSizePolicy(
          QSizePolicy::Preferred, QSizePolicy::Preferred,
          d->m_genotype_lv->sizePolicy().hasHeightForWidth());
        d->m_genotype_lv->setSorting(-1);
        d->m_genotype_lv->header()->setMovingEnabled(true);
        //d->m_genotype_lv->addColumn("id");
        genotype_layout->addWidget(d->m_genotype_lv);

  AnGuiDebug << "done.";
}

// arch-tag: implementation file for old analyzer gui view
