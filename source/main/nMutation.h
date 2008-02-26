/*
 *  nGeometry.h
 *  Avida
 *
 *  Created by David on 10/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef nMutation_h
#define nMutation_h

namespace nMutation {
  enum {
    TRIGGER_NONE = 0,
    TRIGGER_UPDATE,
    TRIGGER_DIVIDE,
    TRIGGER_PARENT,
    TRIGGER_WRITE,
    TRIGGER_READ,
    TRIGGER_EXEC,
    NUM_TRIGGERS
  };

  enum {
    SCOPE_GENOME = 0,
    SCOPE_LOCAL,
    SCOPE_GLOBAL,
    SCOPE_PROP,
    SCOPE_SPREAD
  };

  enum {
    TYPE_POINT = 0,
    TYPE_INSERT,
    TYPE_DELETE,
    TYPE_HEAD_INC,
    TYPE_HEAD_DEC,
    TYPE_TEMP,
    TYPE_KILL
  };
}

#endif
