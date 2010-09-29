//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAnalyzeView.h"

#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cHardwareBase.h"

#include "cMenuWindow.h"
#include "cTextWindow.h"
#include "cBarScreen.h"
#include "cAnalyzeScreen.h"

#include <fstream>

using namespace std;

cTextWindow* cAnalyzeView::base_window = NULL;
cBarScreen* cAnalyzeView::bar_screen = NULL;


cAnalyzeView::cAnalyzeView(cWorld* world) : info(world, this)
{
  Setup(world->GetDefaultContext(), "Avida");

  analyze_screen = new cAnalyzeScreen(world, 0, 0, 3, 0, info);
}

cAnalyzeView::~cAnalyzeView()
{
  if (analyze_screen) delete analyze_screen;
  EndProg(0);
}

void cAnalyzeView::Setup(cAvidaContext& ctx, const cString& in_name)
{
  // Setup text-interface
  StartProg();

  bar_screen = new cBarScreen(&info.GetWorld(), 3, 0, 0, 0, info, in_name);
  base_window = new cTextWindow(0,0,3,0);
  bar_screen->Draw(ctx);
}

void cAnalyzeView::Refresh(cAvidaContext& ctx)
{
  base_window->Redraw();
  bar_screen->Redraw();
  analyze_screen->Clear();
  analyze_screen->Draw(ctx);
}

void cAnalyzeView::Redraw()
{
  bar_screen->Redraw();
  analyze_screen->Redraw();
}


void cAnalyzeView::NotifyUpdate(cAvidaContext& ctx)
{
  bar_screen->Redraw();
  analyze_screen->Redraw();
  DoInputs(ctx);
}

void cAnalyzeView::NotifyError(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Error: ");
  Notify(out_string);
  EndProg(1);
}

void cAnalyzeView::NotifyWarning(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Warning: ");
  Notify(out_string);
}

void cAnalyzeView::NotifyComment(const cString & in_string)
{
  analyze_screen->Notify(in_string);
}

void cAnalyzeView::NotifyOutput(const cString & in_string)
{
  analyze_screen->Notify(in_string);
  analyze_screen->Refresh();
}


void cAnalyzeView::DoInputs(cAvidaContext& ctx)
{
  // @CAO For the moment, redirect to update!
  //  analyze_screen->Update();

  int cur_char = ERR;

  // If there is any input in the buffer, process all of it.
  while ((cur_char = GetInput()) != ERR) {
    switch (cur_char) {
    case 'q':
      if (!Confirm("Are you sure you want to quit?")) break;
    case 'Q':      // Note: Capital 'Q' quits w/o confirming.
      // clear the windows before we go.  Do bar window last to end at top.
      base_window->Redraw();
      bar_screen->Clear();
      bar_screen->Refresh();
      EndProg(0);  // This implementation calls exit(), blowing us clean away
      break;
    case 12: // Ideally this is CTRL-L...
      Refresh(ctx);
      break;
    default:
      analyze_screen->DoInput(ctx, cur_char);
      break;
    }
  }
}


int cAnalyzeView::Confirm(const cString & message)
{
  const int mess_length = message.GetSize();

  // Create a confirm window, and draw it on the screen.

  cTextWindow * conf_win
    = new cTextWindow(3, mess_length + 10, 10, (base_window->Width() - 10 - mess_length) / 2);
  conf_win->Box();
  conf_win->SetBoldColor(COLOR_WHITE);
  conf_win->Print(1, 2, "%s (y/n)", static_cast<const char*>(message));
  conf_win->SetBoldColor(COLOR_CYAN);
  conf_win->Print(1, mess_length + 4, 'y');
  conf_win->Print(1, mess_length + 6, 'n');
  conf_win->SetColor(COLOR_WHITE);
  conf_win->Refresh();

  // Wait for the results.
  bool finished = false;
  bool result = false;
  int cur_char;

  while (finished == false) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case 'n':
    case 'N':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      result = false;
      break;
    case 'y':
    case 'Y':
      finished = true;
      result = true;
      break;
    }
  }

  // Delete the window, redraw the screen, and return the results.
  delete conf_win;
  Redraw();
  return result;
}

void cAnalyzeView::Notify(const cString & message)
{
  cString mess_copy(message);

  // Setup all of the individual lines.
  int num_lines = message.CountNumLines();
  cString * line_array = new cString[num_lines];
  int max_width = 0;
  for (int i = 0; i < num_lines; i++) {
    line_array[i] = mess_copy.PopLine();
    if (line_array[i].GetSize() > max_width)
      max_width = line_array[i].GetSize();
  }

  // Create a window and draw it on the screen.

  cTextWindow * notify_win
    = new cTextWindow(2 + num_lines, max_width + 4, (24 - num_lines - 3) / 2,
		      (70 - max_width) / 2);
  notify_win->Box();
  notify_win->SetBoldColor(COLOR_WHITE);
  for (int j = 0; j < num_lines; j++) {
    notify_win->Print(1 + j, 2, "%s", static_cast<const char*>(line_array[j]));
  }
  notify_win->Refresh();

  // Wait for the results.
  bool finished = false;
  int cur_char;

  while (!finished) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      break;
    }
  }

  // Delete the window and redraw the screen.
  delete notify_win;
  delete [] line_array;
  Redraw();
}

