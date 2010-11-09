/*
 *  cPhenPlastUtil.h
 *  Avida
 *
 *  Created by David on 6/18/10.
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

#ifndef cPhenPlastUtil_h
#define cPhenPlastUtil_h

class cAvidaContext;
class cBioGroup;
class cGenome;
class cPhenPlastSummary;
class cWorld;

template<class T> class tArray;


class cPhenPlastUtil
{
private:
  cPhenPlastUtil(); // @not_implemented
  
public:
  static int GetNumPhenotypes(cAvidaContext& ctx, cWorld* world, cBioGroup* bg);
  static double GetPhenotypicEntropy(cAvidaContext& ctx, cWorld* world, cBioGroup* bg);
  static double GetTaskProbability(cAvidaContext& ctx, cWorld* world, cBioGroup* bg, int task_id);
  static const tArray<double>& GetTaskProbabilities(cAvidaContext& ctx, cWorld* world, cBioGroup* bg);
  static cPhenPlastSummary* TestPlasticity(cAvidaContext& ctx, cWorld* world, const cGenome& mg);
};  


#endif
