/*
 *  ASAvidaNativeObjects.h
 *  Avida
 *
 *  Created by David Bryson on 10/21/08.
 *  Copyright 2008-2011 Michigan State University. All rights reserved.
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

#ifndef ASAvidaNativeObjects_h
#define ASAvidaNativeObjects_h

#include "AvidaScript.h"

#define AS_DECLARE_NATIVE_OBJECT(NAME, CLASS) \
  class CLASS; \
  namespace AvidaScript { template<> inline sASTypeInfo TypeOf<CLASS*>() { return sASTypeInfo(AS_TYPE_OBJECT_REF, NAME); } }

AS_DECLARE_NATIVE_OBJECT("Config",          cAvidaConfig);
AS_DECLARE_NATIVE_OBJECT("Driver",          cDefaultRunDriver);
AS_DECLARE_NATIVE_OBJECT("Genotype",        cAnalyzeGenotype);
AS_DECLARE_NATIVE_OBJECT("GenotypeBatch",   cGenotypeBatch);
AS_DECLARE_NATIVE_OBJECT("ResourceHistory", cResourceHistory);
AS_DECLARE_NATIVE_OBJECT("World",           cWorld);

#undef AS_DECLARE_NATIVE_OBJECT

#endif
