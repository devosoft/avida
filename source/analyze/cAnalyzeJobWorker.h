/*
 *  cAnalyzeJobWorker.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeJobWorker_h
#define cAnalyzeJobWorker_h

#ifndef cThread_h
#include "cThread.h"
#endif

class cAnalyzeJobQueue;


class cAnalyzeJobWorker : public cThread
{
private:
  cAnalyzeJobQueue* m_queue;
  
  void Run();

public:
  cAnalyzeJobWorker(cAnalyzeJobQueue* queue) : m_queue(queue) { ; }  
};

#endif
