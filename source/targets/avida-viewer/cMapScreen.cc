//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cMapScreen.h"

#include "cPopulation.h"
#include "cPopulationCell.h"

#include <fstream>

using namespace std;

cMapScreen::cMapScreen(int _y_size, int _x_size, int _y_start,
      int _x_start, cViewInfo & in_info, cPopulation & in_pop) :
  cScreen(_y_size, _x_size, _y_start, _x_start, in_info),
  x_size(in_pop.GetWorldX()),
  y_size(in_pop.GetWorldY()),
  population(in_pop)
{
  info.SetActiveCell( &(population.GetCell(0)) );
  CenterActiveCPU();
}

cMapScreen::~cMapScreen()
{
}

void cMapScreen::Draw(cAvidaContext& ctx)
{
  CenterActiveCPU();
  Update(ctx);
 
}

void cMapScreen::Update(cAvidaContext& ctx)
{
  // Get working in multiple modes!!

  SetBoldColor(COLOR_WHITE);

  const int name_x = Width() - 20;
  const int name_y = Height() - 1;
  if (info.GetMapMode() == MAP_BASIC)           Print(name_y, name_x, " Genotype View ");
  else if (info.GetMapMode() == MAP_INJECT)     Print(name_y, name_x, " Modified View ");
  else if (info.GetMapMode() == MAP_RESOURCE)   Print(name_y, name_x, " Resource View ");
  else if (info.GetMapMode() == MAP_AGE)        Print(name_y, name_x, "   Age View    ");
  else if (info.GetMapMode() == MAP_BREED_TRUE) Print(name_y, name_x, "Breed True View");
  else if (info.GetMapMode() == MAP_PARASITE)   Print(name_y, name_x, " Parasite View ");
  else if (info.GetMapMode() == MAP_MUTATIONS)  Print(name_y, name_x, " Mutation View ");
  else if (info.GetMapMode() == MAP_THREAD)     Print(name_y, name_x, "  Thread View  ");
  else if (info.GetMapMode() == MAP_LINEAGE)    Print(name_y, name_x, " Lineage View  ");


  // Draw the [<] and [>] around the map mode....
  Print(Height() - 1, Width() - 24, "[ ]");
  Print(Height() - 1, Width() -  4, "[ ]");
  SetBoldColor(COLOR_CYAN);
  Print(Height() - 1, Width() - 23, '<');
  Print(Height() - 1, Width() -  3, '>');
  SetColor(COLOR_WHITE);

  if (y_size >= Height() && Width() > 60) {
    // Map is too big for the display window (since last line is reserved for
    // the display mode text). Tell users how much is clipped.
    Print(Height() - 1, 0, "* Clipping last %i line(s) *",
		  y_size - Height() + 1);
  }

  int virtual_x = corner_id % x_size;
  int virtual_y = corner_id / x_size;

  info.SetupSymbolMaps(info.GetMapMode(), HasColors());

  for (int y = 0; y < Height() - 1 && y < y_size; y++) {
    Move(y, 0);
    int cur_y = (y + virtual_y) % y_size;
    for (int x=0; (AVIDA_MAP_X_SPACING * x) < Width() - 1 && x < x_size; x++){
      int cur_x = (x + virtual_x) % x_size;
      int index = cur_y * x_size + cur_x;

      SetSymbolColor(info.ColorSymbol(index));
      if (info.MapSymbol(index) > 0) Print(info.MapSymbol(index));
      else Print(CHAR_BULLET);

      // Skip spaces before the next map symbol
      for (int i = 0; i < AVIDA_MAP_X_SPACING - 1; i++)  Print(' ');
    }
  }
  SetColor(COLOR_WHITE);

  Refresh();
}


void cMapScreen::DoInput(cAvidaContext& ctx, int in_char)
{
  switch(in_char) {
  case '2':
  case KEY_DOWN:
    corner_id += x_size;
    corner_id %= population.GetSize();
    Update(ctx);
    break;
  case '8':
  case KEY_UP:
    corner_id -= x_size;
    if (corner_id < 0) corner_id += population.GetSize();
    Update(ctx);
    break;
  case '6':
  case KEY_RIGHT:
    corner_id++;
    if (corner_id == population.GetSize()) corner_id = 0;
    Update(ctx);
    break;
  case '4':
  case KEY_LEFT:
    corner_id--;
    if (corner_id < 0) corner_id += population.GetSize();
    Update(ctx);
    break;
  case '>':
  case '.':
    info.IncMapMode();
    Update(ctx);
    break;
  case '<':
  case ',':
    info.DecMapMode();
    Update(ctx);
    break;
  }
}

void cMapScreen::CenterActiveCPU()
{
  assert(x_size > 0 && y_size > 0);

  // first find the map position of the active cpu
  int active_x = info.GetActiveID() % x_size;
  int active_y = info.GetActiveID() / x_size;

  // Now find the relative position of the corner to adjust for this
  int corner_x = active_x - Width() / (2 * AVIDA_MAP_X_SPACING);
  int corner_y = active_y - Height() / 2;

  // Make sure the corner co-ordinates are positive
  if (corner_x < 0) corner_x = (corner_x % x_size) + x_size;
  if (corner_y < 0) corner_y = (corner_y % y_size) + y_size;

  // Now, find the ID for these corner co-ordinates
  corner_id = corner_y * x_size + corner_x;
}

void cMapScreen::CenterXCoord()
{
  // first find the map position of the active cpu
  int active_x = info.GetActiveID() % x_size;

  // Now find the relative position of the corner to adjust for this
  int corner_x = active_x - Width() / (2 * AVIDA_MAP_X_SPACING);
  int corner_y = corner_id / x_size;

  // Make sure the corner co-ordinates are positive
  if (corner_x < 0) corner_x = (corner_x % x_size) + x_size;

  // finally find the ID for these corner co-ordinates
  corner_id = corner_y * x_size + corner_x;
}

void cMapScreen::CenterYCoord()
{
  // first find the map position of the active cpu
  int active_y = info.GetActiveID() / x_size;

  // Now find the relative position of the corner to adjust for this
  int corner_x = corner_id % x_size;
  int corner_y = active_y - Height() / 2;

  // Make sure the corner co-ordinates are positive
  if (corner_y < 0) corner_y = (corner_y % y_size) + y_size;

  // finally find the ID for these corner co-ordinates
  corner_id = corner_y * x_size + corner_x;
}

void cMapScreen::PlaceCursor(cAvidaContext& ctx)
{
  int x_offset = info.GetActiveID() - corner_id;
  x_offset %= x_size;
  if (x_offset < 0) x_offset += x_size;

  int y_offset = (info.GetActiveID() / x_size) - (corner_id / x_size);
  if (y_offset < 0) y_offset += y_size;

  cBioGroup* cpu_gen = info.GetActiveGenotype();

  if (!cpu_gen) {
    Print(Height() - 1, 33,
	      "(%2d, %2d) - (empty)  ", info.GetActiveID() % x_size,
	      info.GetActiveID() / x_size);
  } else {
    Print(Height() - 1, 33, "(%2d, %2d) - %s",
		       info.GetActiveID() % x_size,
		       info.GetActiveID() / x_size,
		       static_cast<const char*>(cpu_gen->GetProperty("name").AsString()));
  }

  if (x_offset == 0 || x_offset == Width()/2 - 1) {
    CenterXCoord();
    Clear();
    Draw(ctx);
    PlaceCursor(ctx);
  }
  else if (y_offset == 0 || y_offset == Height() - 2) {
    CenterYCoord();
    Clear();
    Draw(ctx);
    PlaceCursor(ctx);
  }
  else {
    Move(y_offset, x_offset * 2);
    Refresh();
  }
}

void cMapScreen::Navigate(cAvidaContext& ctx)
{
  // Setup for choosing a cpu...

  CenterActiveCPU();
  Clear();
  Update(ctx);
  Print(Height() - 1, 0, "Choose a CPU and press ENTER");
  Refresh();

  PlaceCursor(ctx);

  cPopulationCell * old_cell = info.GetActiveCell();
  int temp_cell_id;

  bool finished = false;
  int cur_char;
  int iXMove, iYMove;

#ifdef VIEW_ANSI
  MOUSE_INFO * pmiData; // For mouse input (see ANSI.HH)
  // Map dim's run from 0 to iXMax and 0 to iYMax; use this to regulate
  // cursor position

  int iXMax = min(x_size * AVIDA_MAP_X_SPACING, Width()) - 1;
  int iYMax = min(y_size, Height()-1) - 1;
      // extra -1 due to text at bottom

  FlushMouseQueue();        // Make sure queue is clear
  EnableMouseEvents(true);  // and enable mouse event tracking
#endif

  while (finished == false) {
    iXMove = iYMove = 0;	// nothing moves yet
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
      info.SetActiveCell(old_cell);
      finished = true;
      break;
    case '2':
    case KEY_DOWN:
      iYMove = 1;
      break;
    case '8':
    case KEY_UP:
      iYMove = -1;
      break;
    case '4':
    case KEY_LEFT:
      iXMove = -1;
      break;
    case '6':
    case KEY_RIGHT:
      iXMove = 1;
      break;
    case ' ':
    case '\r':
    case '\n':
      finished = true;
      break;

#ifdef VIEW_ANSI
      // Mouse processing code (only works for ANSI right now).  Other view
      //  methods will need a GetMouseInput() stub before this #IFDEF can
      // come out. #defines and MOUSE_INFO struct defined in ansi.hh
    case KEY_MOUSE:
      if (NULL != (pmiData = GetMouseInput()) &&
	  IsCoordInWindow (pmiData->X, pmiData->Y)) {
	// deal with MOUSE_DBLCLICK and MOUSE_CLICK input.  Single click
	// moves cursor.  Dbl click selects cell and runs '\r' code.

	// We already know the click occured within our window
	if (pmiData->iEvent & (MOUSE_DBLCLICK || MOUSE_CLICK)) {
	  // Reposition cursor on single or dbl click. Pin the cursor
	  // movement within the map region.
	  iXMove = min (iXMax, ScreenXToWindow(pmiData->X)) -
	    CursorX();
	  iYMove = min (iYMax, ScreenYToWindow(pmiData->Y)) -
	    CursorY();
	  iXMove /= AVIDA_MAP_X_SPACING; // Correct for spaces in horizontal
	}
	// On dbl click, we are done (act like Enter was hit)
	finished = pmiData->iEvent & MOUSE_DBLCLICK;
      }
      delete (pmiData);	// free up the storage now
      break;
#endif

    default:
      break;
    }	// End of SWITCH

    // Now deal with the cursor movement
    temp_cell_id = info.GetActiveID();
    if (iXMove) {
      int i = temp_cell_id % x_size; // column we are in
      temp_cell_id -= i; // cell id, less column
      i += iXMove;
      while (i >= x_size) i -= x_size;
      while (i < 0) i += x_size;
      temp_cell_id += i; // cell id readjusted to new column
    }
    if (iYMove) {
      temp_cell_id += x_size * iYMove;
      while (temp_cell_id >= population.GetSize())
	temp_cell_id -= population.GetSize();
      while (temp_cell_id < 0) temp_cell_id += population.GetSize();
    }
    if (iXMove || iYMove) {
      info.SetActiveCell( &(population.GetCell(temp_cell_id)) );
      PlaceCursor(ctx);
    }
  } // End of WHILE

#ifdef VIEW_ANSI
  // Turn off mouse events, flush the queue and we are finished
  EnableMouseEvents (false);
  FlushMouseQueue();
#endif

  CenterActiveCPU();	// Set map w/ active CPU in the center
  Clear();
}


