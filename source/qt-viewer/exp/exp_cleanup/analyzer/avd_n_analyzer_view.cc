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
#ifndef QSPLITTER_H
#include <qsplitter.h>
#endif

#ifndef AVD_N_ANALYZER_VIEW_HH
#include "avd_n_analyzer_view.hh"
#endif


avdAnalyzerView::avdAnalyzerView( QWidget *parent, const char *name, WFlags f)
:QMainWindow(parent, name, f){
  QWidget *central_widget = new QWidget(
    this, "<avdAnalyzerView(central_widget)>");
  QVBoxLayout *top_layout = new QVBoxLayout(
    central_widget, 10, -1,
    "<avdAnalyzerView(top_layout)>");
  setCentralWidget(central_widget);

    //QLabel *experiment_editor_label = new QLabel(
    //  central_widget,
    //  "<avdAnalyzerView(experiment_editor_label)>"
    //);
    //experiment_editor_label->setText("experiment_editor_label.");
    //top_layout->addWidget(experiment_editor_label);

    QSplitter *horizontal_splitter = new QSplitter(
      Qt::Horizontal, central_widget,
      "<avdAnalyzerView(horizontal_splitter)>");
    horizontal_splitter->setOpaqueResize(true);
    top_layout->addWidget(horizontal_splitter, 1);

      QWidget *batch_layout_widget = new QWidget(
        horizontal_splitter,
        "<avdAnalyzerView(batch_layout_widget)>");
      batch_layout_widget->setSizePolicy(
        QSizePolicy::Preferred,
        batch_layout_widget->sizePolicy().verData(),
        batch_layout_widget->sizePolicy().hasHeightForWidth());
      horizontal_splitter->setResizeMode(
        batch_layout_widget, QSplitter::KeepSize);
      QVBoxLayout *selector_layout = new QVBoxLayout(
        batch_layout_widget, 10, -1,
        "<avdAnalyzerView(selector_layout)>");
      horizontal_splitter->moveToFirst(batch_layout_widget);

        m_batch_lv = new QListView(
          batch_layout_widget,
          "<avdAnalyzerView(m_batch_lv)>");
        m_batch_lv->setSorting(-1);
        m_batch_lv->addColumn("Batch Name");
        selector_layout->addWidget(m_batch_lv);
  
      QWidget *genotype_layout_widget = new QWidget(
        horizontal_splitter,
        "<avdAnalyzerView(genotype_layout_widget)>");
      QVBoxLayout *genotype_layout = new QVBoxLayout(
        genotype_layout_widget, 10, -1,
        "<avdAnalyzerView(genotype_layout)>");
      horizontal_splitter->moveToLast(genotype_layout_widget);
  
        m_genotype_lv = new QListView(
          genotype_layout_widget,
          "<avdAnalyzerView(m_genotype_lv)>");
        m_genotype_lv->setSizePolicy(
          QSizePolicy::Preferred, QSizePolicy::Preferred,
          m_genotype_lv->sizePolicy().hasHeightForWidth());
        m_genotype_lv->setSorting(-1);
        m_genotype_lv->header()->setMovingEnabled(true);
        //m_genotype_lv->addColumn("id");
        genotype_layout->addWidget(m_genotype_lv);
}

// arch-tag: implementation file for analyzer gui view
