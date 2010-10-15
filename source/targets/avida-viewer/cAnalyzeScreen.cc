//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAnalyzeScreen.h"

#include "cAnalyze.h"
#include "cAnalyzeCommand.h"
#include "cAnalyzeCommandDefBase.h"
#include "cAnalyzeFlowCommand.h"
#include "cEnvironment.h"
#include "cPopulation.h"
#include "cReaction.h"
#include "cReactionProcess.h"
#include "cResource.h"
#include "cStats.h"
#include "cPopulationCell.h"
#include "cOrganism.h"

#include "Platform.h"

#include <csignal>

#if AVIDA_PLATFORM(WINDOWS)
# include <process.h>
# define kill(x, y)
#else
# include <unistd.h>
#endif

using namespace std;

void cAnalyzeScreen::Notify(const cString & in_string)
{
  screen_hist.PushRear(in_string);
}

void cAnalyzeScreen::Draw(cAvidaContext& ctx)
{
  if (mode == ANALYZE_MODE_COMMAND_LINE) {
    DrawCommandLine();
  } else {
    DrawMenu(ctx);
  }
  Update(ctx);
}

void cAnalyzeScreen::DrawCommandLine()
{
  // Bottom-right corner gives current mode.
  PrintOption(Height() - 1, Width() - 25, "[<] Command Line [>]");

  PrintOption(Height() - 1, 2, "Change mode to escape command line ---->");
  
  // Last 3 lines are command prompt.
  SetColor(COLOR_WHITE);
  Box(Height() - 4, 0, 3, Width(), true);
}

void cAnalyzeScreen::DrawMenu(cAvidaContext& ctx)
{
  // Bottom-right corner gives current mode.
  PrintOption(Height() - 1, 1, "Use [TAB] and [SHIFT-TAB] to cycle through tabs.");
  PrintOption(Height() - 1, Width() - 25, "[<] Analyze Menu [>]");
  UpdateMenu(ctx);
}


void cAnalyzeScreen::Update(cAvidaContext& ctx)
{
  if (mode == ANALYZE_MODE_COMMAND_LINE) {
    UpdateCommandLine(ctx);
  } else {
    UpdateMenu(ctx);
  }
  // Refresh();
}


void cAnalyzeScreen::UpdateCommandLine_Body()
{
  SetBoldColor(COLOR_YELLOW);
  Print(Height() - 3, 3, "-> ");
  for (int i = 0; i < nest_depth; i++) {
    Print(Height() - 3, 6+i*2, "> ");
  }

  // Flush everything we have in the buffers.
  m_world->GetDriver().Flush();

  // Print history for the moment...
  int hist_height = Height() - 6;
  int start = 0;
  if (hist_height < screen_hist.GetSize()) {
    start = screen_hist.GetSize() - hist_height;
  }
  
  int y_line = 1;
  for (int i = start; i < screen_hist.GetSize(); i++) {
    const cString & cur_line = screen_hist.GetLine(i);
    if (cur_line.GetSize() == 0);
    else if (cur_line[0] == '-') SetColor(COLOR_YELLOW);
    else if (cur_line[0] == '!') SetBoldColor(COLOR_RED);
    else SetColor(COLOR_CYAN);
    Print(y_line, 1, cur_line);
    ClearToEOL();
    y_line++;
  }
  
  // Print whatever the user is typing.
  const int offset = 6 + nest_depth * 2;
  SetBoldColor(COLOR_YELLOW);
  Print(Height() - 3, offset, cur_input);
  ClearToEOL();
  Move(Height() - 3, offset+cursor_pos);
  Refresh();
}

void cAnalyzeScreen::UpdateCommandLine(cAvidaContext& ctx)
{
  // Draw user input.
  UpdateCommandLine_Body();
  DoInput_CommandLine(ctx);
}

void cAnalyzeScreen::UpdateMenu_Body()
{
  // Most of the screen should be  menu of batches, with extra info at bottom.
  tab_box.Draw();

  // Now, fill in the active tab!
  SetBoldColor(COLOR_CYAN);
  switch (tab_box.GetActiveTab()) {
  case ANALYZE_MENU_TAB_GENOTYPES:
    UpdateMenu_Genotypes();
    break;
  case ANALYZE_MENU_TAB_VARIABLES:
    UpdateMenu_Variables();
    break;
  case ANALYZE_MENU_TAB_COMMANDS:
    UpdateMenu_Commands();
    break;
  case ANALYZE_MENU_TAB_FUNCTIONS:
    UpdateMenu_Functions();
    break;
  }

  Refresh();
}

void cAnalyzeScreen::UpdateMenu_Genotypes()
{
  cString cur_line;
  cAnalyze& analyze = m_world->GetAnalyze();
  int cur_batch = analyze.GetCurrentBatchID();
  int line_num = 0;
  const int col_width = (Width() - 2) / 3;
  
  // Column 1: Batch List
  SetBoldColor(COLOR_WHITE);
  Print(4, 3, "Batch  Count  Flags");

  SetBoldColor(COLOR_CYAN);
  for (int i = 0; i < MAX_BATCHES; i++) {
    cGenotypeBatch & print_batch = analyze.GetBatch(i);
    if (i == cur_batch || print_batch.List().GetSize() > 0) {
      cur_line.Set("%5d  %5d  ", i, print_batch.List().GetSize());
      if (i == cur_batch) cur_line += "C ";
      else cur_line += "  ";
      if (print_batch.IsLineage() == true) cur_line += "L ";
      else cur_line += "  ";
      if (print_batch.IsAligned() == true) cur_line += "A";
      else cur_line += " ";
      
      Print(5+line_num, 3, cur_line);
      line_num++;
    }
  }

  SetBoldColor(COLOR_WHITE);
  Print(6+line_num, 3, "C = Current Batch");
  Print(7+line_num, 3, "L = Is Lineage");
  Print(8+line_num, 3, "A = Is Aligned");

  // Column 2: Genotype List
  SetColor(COLOR_WHITE);
  VLine(col_width, 2, Height()-3);

  SetBoldColor(COLOR_WHITE);
  Print(4, col_width+2, "ID       Fitness   Length");

  // Column 3: Genotype Stats
  SetColor(COLOR_WHITE);
  VLine(2*col_width, 2, Height()-3);
}

void cAnalyzeScreen::UpdateMenu_Variables()
{
  Print(5, 3, "This will be a menu listing the state of all variables.");
}

void cAnalyzeScreen::UpdateMenu_Commands()
{
  Print(5, 3, "This will be a menu of available analyze commands.");
}

void cAnalyzeScreen::UpdateMenu_Functions()
{
  Print(5, 3, "This will be a menu of user-defined functions.");
}


void cAnalyzeScreen::UpdateMenu(cAvidaContext& ctx)
{
  UpdateMenu_Body();
  DoInput_Menu(ctx);
}

void cAnalyzeScreen::DoInput_CommandLine(cAvidaContext& ctx)
{
  // Do not let go of input until told to.
  NoDelay(false);
  bool finished = false;
  while (!finished) {
    int cur_char = GetInput();
    if (cur_char == ERR) continue;

    // Determine if a recognized symbol was typed...
    bool symbol = false;
    if ((cur_char == ' ') || (cur_char == '!') || (cur_char == '@') ||
	(cur_char == '#') || (cur_char == '$') || (cur_char == '%') ||
	(cur_char == '^') || (cur_char == '&') || (cur_char == '*') ||
	(cur_char == '(') || (cur_char == ')') || (cur_char == '-') ||
	(cur_char == '_') || (cur_char == '+') || (cur_char == '=') ||
	(cur_char == '{') || (cur_char == '}') || (cur_char == '[') ||
	(cur_char == ']') || (cur_char == ':') || (cur_char == ';') ||
	(cur_char == '"') || (cur_char == '\'') || (cur_char == '|') ||
	(cur_char == '\\') || (cur_char == '<') || (cur_char == '>') ||
	(cur_char == ',') || (cur_char == '.') || (cur_char == '?') ||
	(cur_char == '/') || (cur_char == '~') || (cur_char == '`')) {
      symbol = true;
    }

    // See if we should add the character typed to the regular input.
    if ((cur_char >= 'a' && cur_char <= 'z') ||
	(cur_char >= 'A' && cur_char <= 'Z') ||
	(cur_char >= '0' && cur_char <= '9') ||
	cur_input.GetSize() > 0 && symbol == true) {
      // A legal character has been added to the command at the prompt.
      // cur_input += (char) cur_char;
      cur_input.Insert((char) cur_char, cursor_pos);
      rollback_line = 0;
      cursor_pos++;
      UpdateCommandLine_Body();
      continue;
    }

    // For other keystrokes, we can use a switch statement...
    switch (cur_char) {
    case KEY_BACKSPACE:
      if (cursor_pos > 0) {
	cur_input.Clip(cursor_pos-1, 1);
	cursor_pos--;
      }
      rollback_line = 0;
      break;

    case 1:  // CTRL-A (jump to beginning of line)
      cursor_pos = 0;
      break;

//  case 2: CTRL-B == move left (see KEY_LEFT below)

    case 3: // CTRL-C (quit)
      exit(0);
      break;

    case 4: // CTRL-D (delete)
      if (cursor_pos < cur_input.GetSize()) {
	cur_input.Clip(cursor_pos, 1);
      }
      rollback_line = 0;
      break;

    case 5: // CTRL-E (end-of-line);
      cursor_pos = cur_input.GetSize();
      break;

//  case 6: CTRL-F == move right (see KEY_RIGHT below)

    case 11: // CTRL-K (cut)
      if (cursor_pos == cur_input.GetSize()) break;
      cut_line = cur_input.Substring(cursor_pos, cur_input.GetSize() - cursor_pos);
      cur_input.Clip(cursor_pos);
      break;

    case 21:     // CTRL-U (delete line)
      cur_input = "";
      cursor_pos = 0;
      rollback_line = 0;
      break;

    case 25:     // CTRL-Y (paste)
      if (cut_line.GetSize() > 0) {
	cur_input.Insert(cut_line, cursor_pos);
	cursor_pos += cut_line.GetSize();
      }
      break;

    case 26:     // CTRL-Z (suspend process)
      kill(getpid(), SIGTSTP);
      break;

    case '\n':
    case '\r':
      // Only look at non-empty lines...
      if (cur_input != "") ProcessCommandLine();
      break;

    case '\t':
      // @CAO TAB COMPLETE!
      break;

    case '<':
    case ',':
    case '>':
    case '.':
      mode = ANALYZE_MODE_MENU;
      Clear();
      Draw(ctx);
      finished = true;
      break;

    case KEY_UP:
      {
	rollback_line++;
	int id = command_hist.GetSize() - rollback_line;
	if (id < 0) { id = 0; rollback_line = command_hist.GetSize(); }
	if (command_hist.GetSize() > 0) cur_input = command_hist.GetLine(id);
	cursor_pos = cur_input.GetSize();
      }
      break;
      
    case KEY_DOWN:
      rollback_line--;
      if (rollback_line > 0) {
	int id = command_hist.GetSize() - rollback_line;
	cur_input = command_hist.GetLine(id);
      } else {
	rollback_line = 0;
	cur_input = "";
      }
      cursor_pos = cur_input.GetSize();
      break;

    case KEY_LEFT:
    case 2:             // CTRL-B (move left)
      cursor_pos--;
      if (cursor_pos < 0) cursor_pos = 0;
      break;

    case KEY_RIGHT:
    case 6:             // CTRL-F (move right)
      cursor_pos++;
      if (cursor_pos > cur_input.GetSize()) cursor_pos = cur_input.GetSize();
      break;

    default:
      Print(10, 70, "** %d **", (int) cur_char);
    }

    // If we're not finished, refresh the screen...
    // (remember: we only made it here if there *was* a keypress.)
    if (finished == false) {
      UpdateCommandLine_Body();
    }
  }

  NoDelay(true);
}


void cAnalyzeScreen::DoInput_Menu(cAvidaContext& ctx)
{
  // Do not let go of input until told to.
  NoDelay(false);
  bool finished = false;

  while (!finished) {
    int cur_char = GetInput();
    if (cur_char == ERR) continue;

    // Test the keystroke...
    switch (cur_char) {
    case 25:     // CTRL-Y or SHIFT-TAB
      tab_box.PrevTab();      
      break;

    case 26:     // CTRL-Z (suspend process)
      kill(getpid(), SIGTSTP);
      break;

   case '\t':
      tab_box.NextTab();
      break;

    case '<':
    case ',':
    case '>':
    case '.':
      mode = ANALYZE_MODE_COMMAND_LINE;
      Clear();
      Draw(ctx);
      finished = true;
      break;

    case KEY_LEFT:
      break;

    case KEY_RIGHT:
      break;

    default:
      {
	int tab_result = tab_box.DoKeypress(cur_char);
	if (tab_result == -1) {
	  if (m_world->GetDriver().ProcessKeypress(cur_char) == true) {
	    finished = true;
	  } else {
	    Print(10, 70, "** %d **", (int) cur_char);
	  }
	}
      }
    }

    // If we're not finished, refresh the screen...
    // (remember: we only made it here if there *was* a keypress.)
    if (finished == false) {
      UpdateMenu_Body();
    }
  }

  NoDelay(true);
}

void cAnalyzeScreen::ProcessCommandLine()
{
  // Get the analyze module...
  cAnalyze& analyze = m_world->GetAnalyze();

  // Save the original input and divide up the rest into command and args.
  cString orig_input = cur_input;
  cString command = cur_input.PopWord();
  cString command_args = cur_input;

  // Reset various tracking values.
  cur_input = "";
  rollback_line = 0;
  cursor_pos = 0;

  // Grab the command portion of this input and set to uppercase.
  command.ToUpper();
  bool command_found = false;
  
  // Check some builtin commands.
  if (command == "QUIT" || command == "EXIT") exit(0);
  
  // Check to see if we are ending a loop.
  if (command == "END") {
    if (nest_depth == 0) exit(0);
    // Reduce the nest_depth.
    nest_depth--;

    // If the loop we finished is at the outermost level, execute it!
    if (nest_depth == 0) {
      cAnalyzeCommand * cur_command = nest_command.Pop();
      command_args = cur_command->GetArgs();
      analyze.PreProcessArgs(command_args);
      cAnalyzeCommandDefBase* command_fun =
	analyze.FindAnalyzeCommandDef(cur_command->GetCommand());
      command_fun->Run(&analyze, command_args, *cur_command);
    }
    // Otherwise store it...
    else {
      cAnalyzeCommand * cur_command = nest_command.Pop();
      nest_command.GetFirst()->GetCommandList()->PushRear(cur_command);
    }

    command_found = true;
  }

  // Check some other built-in commands.
  if (command == "CLEAR") {
    screen_hist.Clear();
    command_found = true;
  }

  // Store this command in the history.
  command_hist.PushRear(orig_input);
  cString out_input("-> ");
  for (int i = 0; i < nest_depth; i++) {
    out_input +=  "> ";
  }
  out_input += orig_input;
  screen_hist.PushRear(out_input);  

  // If the command has already been found, stop here.
  if (command_found == true) return;

  // Check the analyze mode for the command.
  cAnalyzeCommand* cur_command;
  cAnalyzeCommandDefBase* command_fun = analyze.FindAnalyzeCommandDef(command);
  
  // First check for built-in functions...
  if (command_fun != NULL) {
    if (command_fun->IsFlowCommand()) {
      // This is a flow command; it should have a body to it.
      cur_command = new cAnalyzeFlowCommand(command, command_args);
      nest_command.Push(cur_command);
      nest_depth++;
    } else {  // This is a regular command.
      // If this command is at the outermost level, Execute it!
      if (nest_depth == 0) {
	cur_command = new cAnalyzeCommand(command, command_args);
	analyze.PreProcessArgs(command_args);
	command_fun->Run(&analyze, command_args, *cur_command);
      }
      // Otherwise store it...
      else {
	cur_command = new cAnalyzeCommand(command, command_args);
	nest_command.GetFirst()->GetCommandList()->PushRear(cur_command);
      }
    }
  }
  // Then for user defined functions
  else if (analyze.FunctionRun(command, command_args) == true) { }
  // Otherwise, give an error.
  else {
    cString out_string = "! Error: Unknown command '";
    out_string += command;
    out_string += "'.";
    screen_hist.PushRear(out_string);
  }
  
  // Deal with collected feedback from analyze mode.
  m_world->GetDriver().Flush();
}

void cAnalyzeScreen::DoInput(cAvidaContext& ctx, int in_char)
{
  // For the moment, redirect to update!
  Update(ctx);


  SetBoldColor(COLOR_CYAN);
  
  // Keys that work similarly in both modes.
  switch (in_char) {
  case '>':
  case '.':
  case '<':
  case ',':
    if (mode == ANALYZE_MODE_COMMAND_LINE) {
      mode = ANALYZE_MODE_MENU;
    } else {
      mode = ANALYZE_MODE_COMMAND_LINE;
    }
    Clear();
    Draw(ctx);
    break;
  }
  
  if (mode == ANALYZE_MODE_COMMAND_LINE) {
    switch (in_char) {
    case KEY_DOWN:
      // Scroll down through previous selections.
      Update(ctx);
      break;
    case KEY_UP:
      Update(ctx);
      break;
    }
  }
  else if (mode == ANALYZE_MODE_MENU) {
    switch (in_char) {
    case KEY_DOWN:
      // Scroll down through previous selections.
      Update(ctx);
      break;
    case KEY_UP:
      Update(ctx);
      break;
    }
  }
  else { // Invalid mode state!
  }

}
