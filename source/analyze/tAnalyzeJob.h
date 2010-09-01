/*
 *  tAnalyzeJob.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef tAnalyzeJob_h
#define tAnalyzeJob_h

#ifndef cAnalyzeJob_h
#include "cAnalyzeJob.h"
#endif

template <class T> class tAnalyzeJob : public cAnalyzeJob
{
protected:
  T* m_target;
  void (T::*JobTask)(cAvidaContext&);

public:
  tAnalyzeJob(T* target, void (T::*funJ)(cAvidaContext&)) : cAnalyzeJob(), m_target(target), JobTask(funJ) { ; }
  
  void Run(cAvidaContext& ctx)
  {
    (m_target->*JobTask)(ctx);
  }
};

#endif
