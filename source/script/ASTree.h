/*
 *  ASTree.h
 *  Avida
 *
 *  Created by David on 4/7/07.
 *  Copyright 2007 Michigan State University. All rights reserved.
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

#ifndef ASTree_h
#define ASTree_h

enum eASTNodeType
{
  N_STMT_LIST,
  N_EXPR,
  N_VAR_DECL,
  N_IF_BLOCK,
  N_WHILE_BLOCK,
  N_FOREACH_BLOCK,
  N_FUNC,
  N_RETURN,
  N_BLOCK,
  
  N_ERROR
};

class cASTVisitor;

class cASTNode
{
public:
  eASTNodeType type;
  
  cASTNode() { ; }
  virtual ~cASTNode() { ; }
  
  virtual void Accept(cASTVisitor& visitor) = 0;
};





#endif
