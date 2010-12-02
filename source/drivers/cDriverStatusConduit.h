/*
 *  cDriverStatusConduit.h
 *  Avida
 *
 *  Created by David on 10/14/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#ifndef cDriverStatusConduit_h
#define cDriverStatusConduit_h

class cString;


class cDriverStatusConduit
{
private:
  cDriverStatusConduit(const cDriverStatusConduit&); // @not_implemented
  cDriverStatusConduit& operator=(const cDriverStatusConduit&); // @not_implemented
  
public:
  cDriverStatusConduit() { ; }
  virtual ~cDriverStatusConduit() { ; }
  
  // Signals
  virtual void SignalError(const cString& msg, int exit_code = 0);
  
  // Notifications
  virtual void NotifyWarning(const cString& msg);
  virtual void NotifyComment(const cString& msg);
};

#endif
