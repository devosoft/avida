#ifndef QMENUBAR_H
#include <qmenubar.h>
#endif
#ifndef QPOPUPMENU_H
#include <qpopupmenu.h>
#endif

#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_MENUBAR_HANDLER_HH
#include "avd_analyzer_menubar_handler.hh"
#endif

void avd_o_AnalyzerMenuBarHandler::setupMenuBar(QMenuBar *menubar){
  if(menubar){
      QPopupMenu *file_menu = new QPopupMenu(
        menubar, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(file_menu)");
        file_menu->insertItem(
          "Load Current Genotypes...",
          this, SIGNAL(loadCurrentGenotypesSig())
        );
        file_menu->insertItem(
          "Load Historic Genotypes...",
          this, SIGNAL(loadHistoricGenotypesSig())
        );  
        file_menu->insertItem(
          "Open...", this, SIGNAL(openSig())
        );  
    menubar->insertItem("File", file_menu);

      QPopupMenu *edit_menu = new QPopupMenu(
        menubar, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(edit_menu)"
      );
        QPopupMenu *batches_submenu = new QPopupMenu(
          edit_menu, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(batches_submenu)"
        );
          batches_submenu->insertItem(
            "Delete", this, SIGNAL(deleteBatchesSig())
          );
          batches_submenu->insertItem(
            "Merge", this, SIGNAL(mergeBatchesSig())
          );
          batches_submenu->insertItem(
            "Duplicate", this, SIGNAL(duplicateBatchesSig())
          );
        edit_menu->insertItem("Batches", batches_submenu);
        QPopupMenu *genotypes_submenu = new QPopupMenu(
          edit_menu, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(genotypes_submenu)"
        );
        edit_menu->insertItem("Genotypes", genotypes_submenu);
    menubar->insertItem("Edit", edit_menu);

      QPopupMenu *select_menu = new QPopupMenu(
        menubar, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(select_menu)"
      );
        QPopupMenu *reduction_submenu = new QPopupMenu(
          select_menu, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(reduction_submenu)"
        );
          reduction_submenu->insertItem(
            "Find Genotype", this, SIGNAL(findGenotypeSig())
          );
          reduction_submenu->insertItem(
            "Find Lineage", this, SIGNAL(findLineageSig())
          );
          reduction_submenu->insertItem(
            "Find Clade", this, SIGNAL(findCladeSig())
          );
          reduction_submenu->insertItem(
            "Sample Organisms", this, SIGNAL(sampleOrganismsSig())
          );
          reduction_submenu->insertItem(
            "Sample Genotypes", this, SIGNAL(sampleGenotypesSig())
          );
        select_menu->insertItem("Reduction", reduction_submenu);
        select_menu->insertItem(
          "Invert", this, SIGNAL(invertSig())
        );
    menubar->insertItem("Select", select_menu);

      QPopupMenu *analyze_menu = new QPopupMenu(
        menubar, "<avd_o_AnalyzerMenuBarHandler::setupMenuBar(analyze_menu)"
      );
        analyze_menu->insertItem(
          "Genotype/Phenotype Map", this, SIGNAL(genotypePhenotypeMapSig())
        );
        analyze_menu->insertItem(
          "Choose Columns...", this, SIGNAL(chooseColumnsSig())
        );
    menubar->insertItem("Analyze", analyze_menu);
  } else {
    AnGuiError << "asked to setup null menubar!";
  }
}

// arch-tag: implementation file for old analyzer menubar handler
