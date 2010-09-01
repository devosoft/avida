/*
 *  ASAvidaNativeObjects.h
 *  Avida
 *
 *  Created by David Bryson on 10/21/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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
