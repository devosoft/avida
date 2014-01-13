/*
 *  cTextViewerAnalyzeDriver.h
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cTextViewerAnalyzeDriver_h
#define cTextViewerAnalyzeDriver_h

#include "cTextViewerDriver_Base.h"

#include "cString.h"

class cAnalyzeView;

class cTextViewerAnalyzeDriver : public cTextViewerDriver_Base
{
private:
  cTextViewerAnalyzeDriver(); // @not_implemented
  cTextViewerAnalyzeDriver(const cTextViewerAnalyzeDriver&); // @not_implemented
  cTextViewerAnalyzeDriver& operator=(const cTextViewerAnalyzeDriver&); // @not_implemented
  
protected:
  bool m_interactive;
  
public:
  cTextViewerAnalyzeDriver(cWorld* world, bool inter = false);
  ~cTextViewerAnalyzeDriver();
  
  void Run();
  
  // Driver Actions
  void Finish() { m_done = true; }
  void Pause() { return; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
};

#endif
