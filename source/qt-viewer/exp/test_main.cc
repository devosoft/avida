

#include <iostream>

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef QWIDGET_H
#include <qwidget.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QSPLITTER_H
#include <qsplitter.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif
#ifndef QHEADER_H
#include <qheader.h>
#endif

#ifndef TCOMPOSITE_HDRS_HH
#include "tComposite_hdrs.hh"
#endif
#ifndef TDICTIONARY_HH
#include "tDictionary.hh"
#endif
#ifndef AVD_SETUP_HH
#include "avd_setup.hh"
#endif
#ifndef AVD_SETUP_DBG_HDRS_HH
#include "avd_setup_dbg_hdrs.hh"
#endif
#ifndef AVD_WIDGETLIST_HH
#include "avd_widget_list.hh"
#endif
#ifndef AVD_WIDGETLISTITEM_HH
#include "avd_widget_list_item.hh"
#endif


using namespace std;


void frell(tComponent<cFrob> *me, int depth){
  for(int i=0; i < depth; i++) cout << " ";
  cout << "frell me: " << me->getData()->getListViewTitle() << endl;

  if(!me->getChildList()) return;

  tListIterator<tComponent<cFrob> > it(*me->getChildList());
  it.Reset();
  tComponent<cFrob> *him = 0;
  while(him = it.Next()){
    frell(him, depth + 1);
  }
}


int main(int argc, char **argv){
  GenDebug("entered.");

  int result;
  QApplication a( argc, argv );
  

  QMainWindow *main_window = new QMainWindow(
    0,
    "<main(main_window)>"
  );

    QWidget *central_widget = new QWidget(
      main_window,
      "<main(central_widget)>"
    );
    QVBoxLayout *top_layout = new QVBoxLayout(
      central_widget,
      10,
      -1,
      "<main(top_layout)>"
    );
    main_window->setCentralWidget(central_widget);


      QLabel *experiment_editor_label = new QLabel(
        central_widget,
        "<main(experiment_editor_label)>"
      );
      experiment_editor_label->setText("experiment_editor_label.");
      top_layout->addWidget(experiment_editor_label);

      QSplitter *horizontal_splitter = new QSplitter(
        Qt::Horizontal,
        central_widget,
        "<main(horizontal_splitter)>"
      );
      horizontal_splitter->setOpaqueResize(true);
      top_layout->addWidget(horizontal_splitter, 1);

        QWidget *selector_layout_widget = new QWidget(
          horizontal_splitter,
          "<main(selector_layout_widget)>"
        );
        selector_layout_widget->setSizePolicy(
          QSizePolicy::Preferred,
          selector_layout_widget->sizePolicy().verData(),
          selector_layout_widget->sizePolicy().hasHeightForWidth()
        );
        horizontal_splitter->setResizeMode(
          selector_layout_widget,
          QSplitter::KeepSize);
        QVBoxLayout *selector_layout = new QVBoxLayout(
          selector_layout_widget,
          10,
          -1,
          "<main(selector_layout)>"
        );
        horizontal_splitter->moveToFirst(selector_layout_widget);

          QListView *selector_listview = new QListView(
            selector_layout_widget,
            "<main(selector_listview)>"
          );
          selector_listview->header()->hide();
          selector_listview->setRootIsDecorated(TRUE);
          selector_listview->setSorting(-1);
          selector_listview->addColumn("Summary");
          selector_layout->addWidget(selector_listview);

  //tDictionary<tComponent<cFrob> *> cmp_dict;
  cVrkl cmp_dict;
  cmp_dict.connect(
    selector_listview, SIGNAL(selectionChanged(QListViewItem *)),
    &cmp_dict, SLOT(selectionChangedSlot(QListViewItem *))
  );
  cmp_dict.connect(
    selector_listview, SIGNAL(clicked(QListViewItem *)),
    &cmp_dict, SLOT(listViewClicked(QListViewItem *))
  );

            tComponent<cFrob> *selector_cmp = new tComposite<cFrob>;
            selector_cmp->setData(new cFrobEmptyPlaceholder);
            selector_cmp->getData()->setListViewTitle("Selector");
            selector_cmp->getData()->setListViewTitle("Selector");

            cmp_dict.setRoot(selector_cmp);

              tComponent<cFrob> *genesis_cmp = new tComposite<cFrob>;
              genesis_cmp->setData(new cFrobHeader);
              genesis_cmp->getData()->setListViewTitle("Genesis Settings");
              genesis_cmp->getData()->setWidgetListTitle("Genesis Settings");
              selector_cmp->PushRear(genesis_cmp);

                tComponent<cFrob> *genesis_simple_cmp = new tComposite<cFrob>;
                genesis_simple_cmp->setData(new cFrobHeader);
                genesis_simple_cmp->getData()->setListViewTitle("Simple View");
                genesis_simple_cmp->getData()->setWidgetListTitle("Simple View");
                genesis_cmp->PushRear(genesis_simple_cmp);

                  tComponent<cFrob> *gen_general_cmp = new tComposite<cFrob>;
                  gen_general_cmp->setData(new cFrobWidgetList);
                  gen_general_cmp->getData()->setListViewTitle("General Settings");
                  gen_general_cmp->getData()->setWidgetListTitle("General Settings");
                  genesis_simple_cmp->PushRear(gen_general_cmp);

                    tComponent<cFrob> *testyfoo_cmp = new tLeaf<cFrob>;
                    testyfoo_cmp->setData(new cFrobWidgetFoo<>);
                    testyfoo_cmp->getData()->setKey("TESTYFOO");
                    testyfoo_cmp->getData()->setListViewTitle("Testy-Foo");
                    testyfoo_cmp->getData()->setWidgetListTitle("Testy-Foo");
                    gen_general_cmp->PushRear(testyfoo_cmp);

                    tComponent<cFrob> *gen_filepath_cmp = new tLeaf<cFrob>;
                    gen_filepath_cmp->setData(new cFrobWidgetFoo<>);
                    gen_filepath_cmp->getData()->setKey("GENESIS_PATH");
                    gen_filepath_cmp->getData()->setListViewTitle("Genesis File Path");
                    gen_filepath_cmp->getData()->setWidgetListTitle("Genesis File Path");
                    gen_general_cmp->PushRear(gen_filepath_cmp);

                    tComponent<cFrob> *version_id_cmp = new tLeaf<cFrob>;
                    version_id_cmp->setData(new cFrobWidgetFoo<>);
                    version_id_cmp->getData()->setKey("VERSION_ID");
                    version_id_cmp->getData()->setListViewTitle("Avida Version ID");
                    version_id_cmp->getData()->setWidgetListTitle("Avida Version ID");
                    gen_general_cmp->PushRear(version_id_cmp);

                  tComponent<cFrob> *gen_architecture_cmp = new tComposite<cFrob>;
                  gen_architecture_cmp->setData(new cFrobWidgetList);
                  gen_architecture_cmp->getData()->setListViewTitle("Architecture");
                  gen_architecture_cmp->getData()->setWidgetListTitle("Architecture");
                  genesis_simple_cmp->PushRear(gen_architecture_cmp);

                    tComponent<cFrob> *max_updates_cmp = new tLeaf<cFrob>;
                    max_updates_cmp->setData(new cFrobWidgetFoo<>);
                    max_updates_cmp->getData()->setKey("MAX_UPDATES");
                    max_updates_cmp->getData()->setListViewTitle("Max Updates");
                    max_updates_cmp->getData()->setWidgetListTitle("Max Updates");
                    gen_architecture_cmp->PushRear(max_updates_cmp);

                    tComponent<cFrob> *max_generations_cmp = new tLeaf<cFrob>;
                    max_generations_cmp->setData(new cFrobWidgetFoo<>);
                    max_generations_cmp->getData()->setKey("MAX_GENERATIONS");
                    max_generations_cmp->getData()->setListViewTitle("Max Generations");
                    max_generations_cmp->getData()->setWidgetListTitle("Max Generations");
                    gen_architecture_cmp->PushRear(max_generations_cmp);

                    tComponent<cFrob> *end_condition_cmp = new tLeaf<cFrob>;
                    end_condition_cmp->setData(new cFrobWidgetFoo<>);
                    end_condition_cmp->getData()->setKey("END_CONDITION_MODE");
                    end_condition_cmp->getData()->setListViewTitle("End Condition Mode");
                    end_condition_cmp->getData()->setWidgetListTitle("End Condition Mode");
                    gen_architecture_cmp->PushRear(end_condition_cmp);

                    tComponent<cFrob> *world_x_cmp = new tLeaf<cFrob>;
                    world_x_cmp->setData(new cFrobWidgetFoo<>);
                    world_x_cmp->getData()->setKey("WORLD-X");
                    world_x_cmp->getData()->setListViewTitle("World Width");
                    world_x_cmp->getData()->setWidgetListTitle("World Width");
                    gen_architecture_cmp->PushRear(world_x_cmp);

                    tComponent<cFrob> *world_y_cmp = new tLeaf<cFrob>;
                    world_y_cmp->setData(new cFrobWidgetFoo<>);
                    world_y_cmp->getData()->setKey("WORLD-Y");
                    world_y_cmp->getData()->setListViewTitle("World Height");
                    world_y_cmp->getData()->setWidgetListTitle("World Height");
                    gen_architecture_cmp->PushRear(world_y_cmp);

                    tComponent<cFrob> *max_cpu_threads_cmp = new tLeaf<cFrob>;
                    max_cpu_threads_cmp->setData(new cFrobWidgetFoo<>);
                    max_cpu_threads_cmp->getData()->setKey("MAX_CPU_THREADS");
                    max_cpu_threads_cmp->getData()->setListViewTitle("Max CPU Threads");
                    max_cpu_threads_cmp->getData()->setWidgetListTitle("Max CPU Threads");
                    gen_architecture_cmp->PushRear(max_cpu_threads_cmp);

                    tComponent<cFrob> *random_seed_cmp = new tLeaf<cFrob>;
                    random_seed_cmp->setData(new cFrobWidgetFoo<>);
                    random_seed_cmp->getData()->setKey("RANDOM_SEED");
                    random_seed_cmp->getData()->setListViewTitle("Random Seed");
                    random_seed_cmp->getData()->setWidgetListTitle("Random Seed");
                    gen_architecture_cmp->PushRear(random_seed_cmp);

                  tComponent<cFrob> *timeslicing_cmp = new tComposite<cFrob>;
                  timeslicing_cmp->setData(new cFrobWidgetList);
                  timeslicing_cmp->getData()->setListViewTitle("Timeslicing");
                  timeslicing_cmp->getData()->setWidgetListTitle("Timeslicing");
                  genesis_simple_cmp->PushRear(timeslicing_cmp);

                    tComponent<cFrob> *ave_time_slice_cmp = new tLeaf<cFrob>;
                    ave_time_slice_cmp->setData(new cFrobWidgetFoo<>);
                    ave_time_slice_cmp->getData()->setKey("AVE_TIME_SLICE");
                    ave_time_slice_cmp->getData()->setListViewTitle("Average Time Slice");
                    ave_time_slice_cmp->getData()->setWidgetListTitle("Average Time Slice");
                    timeslicing_cmp->PushRear(ave_time_slice_cmp);

                    tComponent<cFrob> *slicing_method_cmp = new tLeaf<cFrob>;
                    slicing_method_cmp->setData(new cFrobWidgetFoo<>);
                    slicing_method_cmp->getData()->setKey("SLICING_METHOD");
                    slicing_method_cmp->getData()->setListViewTitle("Slicing Method");
                    slicing_method_cmp->getData()->setWidgetListTitle("Slicing Method");
                    timeslicing_cmp->PushRear(slicing_method_cmp);

                    tComponent<cFrob> *size_merit_method_cmp = new tLeaf<cFrob>;
                    size_merit_method_cmp->setData(new cFrobWidgetFoo<>);
                    size_merit_method_cmp->getData()->setKey("SIZE_MERIT_METHOD");
                    size_merit_method_cmp->getData()->setListViewTitle("Size Merit Method");
                    size_merit_method_cmp->getData()->setWidgetListTitle("Size Merit Method");
                    timeslicing_cmp->PushRear(size_merit_method_cmp);

                    tComponent<cFrob> *base_size_merit_cmp = new tLeaf<cFrob>;
                    base_size_merit_cmp->setData(new cFrobWidgetFoo<>);
                    base_size_merit_cmp->getData()->setKey("BASE_SIZE_MERIT");
                    base_size_merit_cmp->getData()->setListViewTitle("Base Size Merit");
                    base_size_merit_cmp->getData()->setWidgetListTitle("Base Size Merit");
                    timeslicing_cmp->PushRear(base_size_merit_cmp);

                    tComponent<cFrob> *task_merit_method_cmp = new tLeaf<cFrob>;
                    task_merit_method_cmp->setData(new cFrobWidgetFoo<>);
                    task_merit_method_cmp->getData()->setKey("TASK_MERIT_METHOD");
                    task_merit_method_cmp->getData()->setListViewTitle("Task Merit Method");
                    task_merit_method_cmp->getData()->setWidgetListTitle("Task Merit Method");
                    timeslicing_cmp->PushRear(task_merit_method_cmp);

                    tComponent<cFrob> *max_label_exe_size_cmp = new tLeaf<cFrob>;
                    max_label_exe_size_cmp->setData(new cFrobWidgetFoo<>);
                    max_label_exe_size_cmp->getData()->setKey("MAX_LABEL_EXE_SIZE");
                    max_label_exe_size_cmp->getData()->setListViewTitle("Max Label Execution Size");
                    max_label_exe_size_cmp->getData()->setWidgetListTitle("Max Label Execution Size");
                    timeslicing_cmp->PushRear(max_label_exe_size_cmp);

                  tComponent<cFrob> *births_cmp = new tComposite<cFrob>;
                  births_cmp->setData(new cFrobWidgetList);
                  births_cmp->getData()->setListViewTitle("Births");
                  births_cmp->getData()->setWidgetListTitle("Births");
                  genesis_simple_cmp->PushRear(births_cmp);

                  tComponent<cFrob> *deaths_cmp = new tComposite<cFrob>;
                  deaths_cmp->setData(new cFrobWidgetList);
                  deaths_cmp->getData()->setListViewTitle("Deaths");
                  deaths_cmp->getData()->setWidgetListTitle("Deaths");
                  genesis_simple_cmp->PushRear(deaths_cmp);

                  tComponent<cFrob> *merit_cmp = new tComposite<cFrob>;
                  merit_cmp->setData(new cFrobWidgetList);
                  merit_cmp->getData()->setListViewTitle("Merit");
                  merit_cmp->getData()->setWidgetListTitle("Merit");
                  genesis_simple_cmp->PushRear(merit_cmp);

                  tComponent<cFrob> *genotype_cmp = new tComposite<cFrob>;
                  genotype_cmp->setData(new cFrobWidgetList);
                  genotype_cmp->getData()->setListViewTitle("Genotypes");
                  genotype_cmp->getData()->setWidgetListTitle("Genotypes");
                  genesis_simple_cmp->PushRear(genotype_cmp);

                tComponent<cFrob> *genesis_expert_cmp = new tComposite<cFrob>;
                genesis_expert_cmp->setData(new cFrobWidgetList);
                genesis_expert_cmp->getData()->setListViewTitle("Expert View");
                genesis_expert_cmp->getData()->setWidgetListTitle("Expert View");
                genesis_cmp->PushRear(genesis_expert_cmp);

  // testing traversal of composite tree.
  frell(selector_cmp, 0);

  // this builds the list view.
  tComponent<cFrob> *him = 0;
  tListIterator<tComponent<cFrob> > it(*selector_cmp->getChildList());
  it.Reset();
  while(him = it.Prev()){
    him->getData()->setupListViewItems(him, selector_listview);
    avd_Setup_Debug(him->getData()->getListViewTitle());
  }

        QWidget *editor_layout_widget = new QWidget(
          horizontal_splitter,
          "<main(editor_layout_widget)>"
        );
        QVBoxLayout *editor_layout = new QVBoxLayout(
          editor_layout_widget,
          10,
          -1,
          "<main(editor_layout)>"
        );
        horizontal_splitter->moveToLast(editor_layout_widget);

          avd_WidgetList *selector_wl = new avd_WidgetList(
            editor_layout_widget,
            "selector_wl"
          );
          editor_layout->addWidget(selector_wl);
          selector_wl->setSizePolicy(
            QSizePolicy::Preferred,
            QSizePolicy::Preferred,
            selector_wl->sizePolicy().hasHeightForWidth()
          );

  // this builds the widget list.
  it.Reset();
  while(him = it.Next()){
    him->getData()->setupWidgetListItems(him, selector_wl);
    avd_Setup_Debug(him->getData()->getListViewTitle());
  }
  cmp_dict.listViewClicked(0);

  main_window->resize(640, 480);
  main_window->setCaption("Experiment-Setup Editor");
  main_window->statusBar();
  main_window->show();
  
  a.connect(
    &a, SIGNAL(lastWindowClosed()),
    &a, SLOT(quit())
  );
  
  result = a.exec();

  GenDebug("done.");

  return result;
}

