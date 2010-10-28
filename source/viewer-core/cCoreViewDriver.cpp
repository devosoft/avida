/*
 *  cCoreViewDriver.cpp
 *  Avida
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#include "cCoreViewDriver.h"

#include "cAvidaContext.h"
#include "cDriverManager.h"
#include "cWorld.h"

#include <iostream>


cCoreViewDriver::cCoreViewDriver(cWorld* world) : m_world(world)
{
  cDriverManager::Register(this);

}

cCoreViewDriver::~cCoreViewDriver()
{
  cDriverManager::Unregister(this);
  delete m_world;
}

void cCoreViewDriver::Run()
{

}


void cCoreViewDriver::RaiseException(const cString& in_string)
{
  std::cerr << "Error: " << in_string << std::endl;
}

void cCoreViewDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  std::cerr << "Error: " << in_string << "  Exiting..." << std::endl;
  exit(exit_code);
}

void cCoreViewDriver::NotifyComment(const cString& in_string)
{
  std::cout << in_string << std::endl;
}

void cCoreViewDriver::NotifyWarning(const cString& in_string)
{
  std::cout << "Warning: " << in_string << std::endl;
}
