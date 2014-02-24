/*
 *  environment/Reaction.h
 *  avida-core
 *
 *  Created by David on 6/24/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaEnvironmentReaction_h
#define AvidaEnvironmentReaction_h

#include "avida/environment/Types.h"

#include <limits>


namespace Avida {
  namespace Environment {
    
    // Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class ReactionProcess;
    class ReactionRequisite;
    
    
    // Environment::Reaction
    // --------------------------------------------------------------------------------------------------------------
    
    class Reaction
    {
    private:
      const ReactionID m_id;
      
    public:
      Reaction(const ReactionID& reaction_id);
      
      const ReactionID& GetID() const { return m_id; }
    };
    
    
    
    // Environment::ReactionProcess
    // --------------------------------------------------------------------------------------------------------------
    
    class ReactionProcess
    {
    protected:
      Util::Args* m_args;
      
    public:
      LIB_EXPORT inline ReactionProcess(Util::Args* args) : m_args(args) { ; }
      LIB_EXPORT virtual ~ReactionProcess() = 0;
      
      LIB_EXPORT virtual void DoProcess(/* arguments */) = 0;
    };
    
    
    // Environment::ReactionRequisite
    // --------------------------------------------------------------------------------------------------------------
    
    class ReactionRequisite
    {
    private:
      Apto::List<Reaction*> m_prior_reaction_list;
      Apto::List<Reaction*> m_prior_noreaction_list;
      int m_min_task_count;
      int m_max_task_count;
      int m_min_reaction_count;
      int m_max_reaction_count;
      int m_min_tot_reaction_count;
      int m_max_tot_reaction_count;
      int m_divide_only;
      
    public:
      ReactionRequisite()
        : m_min_task_count(0)
        , m_max_task_count(std::numeric_limits<int>::max())
        , m_min_reaction_count(0)
        , m_max_reaction_count(std::numeric_limits<int>::max())
        , m_min_tot_reaction_count(0)
        , m_max_tot_reaction_count(std::numeric_limits<int>::max())
        , m_divide_only(0)
      {
      }
      ~ReactionRequisite() { ; }
      
      const Apto::List<Reaction*>& Reactions() const { return m_prior_reaction_list; }
      const Apto::List<Reaction*>& NoReactions() const { return m_prior_noreaction_list; }
      int MinTaskCount() const { return m_min_task_count; }
      int MaxTaskCount() const { return m_max_task_count; }
      int MinReactionCount() const { return m_min_reaction_count; }
      int MaxReactionCount() const { return m_max_reaction_count; }
      int MinTotReactionCount() const { return m_min_tot_reaction_count; }
      int MaxTotReactionCount() const { return m_max_tot_reaction_count; }
      int DivideOnly() const { return m_divide_only; }
      
      void AddReaction(Reaction* in_reaction) { m_prior_reaction_list.PushRear(in_reaction); }
      void AddNoReaction(Reaction* in_reaction) { m_prior_noreaction_list.PushRear(in_reaction); }
      
      void SetMinTaskCount(int min) { m_min_task_count = min; }
      void SetMaxTaskCount(int max) { m_max_task_count = max; }
      void SetMinReactionCount(int min) { m_min_reaction_count = min; }
      void SetMaxReactionCount(int max) { m_max_reaction_count = max; }
      void SetMinTotReactionCount(int min) { m_min_tot_reaction_count = min; }
      void SetMaxTotReactionCount(int max) { m_max_tot_reaction_count = max; }
      void SetDivideOnly(int div) { m_divide_only = div; }
    };
    
  };
};

#endif
