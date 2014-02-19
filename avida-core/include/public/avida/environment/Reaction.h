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
    public:
      enum Type {
        PROCTYPE_ADD = 0,
        PROCTYPE_MULT,
        PROCTYPE_POW,
        PROCTYPE_LIN,
        PROCTYPE_ENZYME,
        PROCTYPE_EXP
      };
      
    private:
      cResourceDef* resource;  // Resource consumed.
      
      double m_value;          // Efficiency.
      int m_type;              // Method of applying bonuses.
      double m_max_number;     // Max quantity of resource usable.
      double m_min_number;     // Minimum quantity of resource needed (otherwise 0)
      double m_max_fraction;   // Max fraction of avaiable resources useable.
      double m_k_sub_m;        // k sub m variable needed for enzyme reaction
      cResourceDef* m_product;    // Output resource.
      double m_conversion;     // Conversion factor.
      double m_lethal;		 // Lethality of reaction
      bool m_sterilize; //!< Whether performance of this reaction sterilizes the organism.
      bool m_depletable;       // Does completing consume resource?
      
      // Resource detection
      cResourceDef* detect;    // Resource Measured
      double m_detection_threshold; // Min quantity of resource to register present
      double m_detection_error; // Var of Detection Event (as % of resource present)
      
      
    public:
      ReactionProcess()
        : resource(NULL)
        , m_value(1.0)
        , m_type(PROCTYPE_ADD)
        , m_max_number(1.0)
        , m_min_number(0.0)
        , m_max_fraction(1.0)
        , m_k_sub_m(0.0)
        , m_product(NULL)
        , m_conversion(1.0)
        , m_lethal(0)
        , m_sterilize(false)
        , m_depletable(true)
        , m_detect(NULL)
        , m_detection_threshold(0.0)
        , m_detection_error(0.0)
      {
      }
      ~ReactionProcess() { ; }
      
      cResourceDef* GetResDef() const { return m_resource; }
      double Value() const { return m_value; }
      Type ProcessType() const { return m_type; }
      double MaxNumber() const { return m_max_number; }
      double MinNumber() const { return m_min_number; }
      double MaxFraction() const { return m_max_fraction; }
      double KsubM() const { return m_k_sub_m; }
      cResourceDef* Product() const { return m_product; }
      double Conversion() const { return m_conversion; }
      bool Depletable() const { return m_depletable; }
      double Lethal() const { return m_lethal; }
      bool Sterilize() const { return m_sterilize; }
      cResourceDef* Detect() const { return m_detect; }
      double DetectionThreshold() const { return m_detection_threshold; }
      double DetectionError() const { return m_detection_error; }
      
      void SetResourceDef(cResourceDef* _in) { m_resource = _in; }
      void SetValue(double _in) { m_value = _in; }
      void SetType(Type _in) { m_type = _in; }
      void SetMaxNumber(double _in) { m_max_number = _in; }
      void SetMinNumber(double _in) { m_min_number = _in; }
      void SetMaxFraction(double _in) { m_max_fraction = _in; }
      void SetKsubM(double _in) { m_k_sub_m = _in; }
      void SetProductDef(cResourceDef* _in) { m_product = _in; }
      void SetConversion(double _in) { m_conversion = _in; }
      void SetDepletable(bool _in) { m_depletable = _in; }
      void SetLethal(double _in) { m_lethal = _in; }
      void SetSterile(int _in) { m_sterilize = _in; }
      void SetDetect(cResourceDef* _in) { m_detect = _in; }
      void SetDetectionThreshold(double _in) { m_detection_threshold = _in; }
      void SetDetectionError(double _in) { m_detection_error = _in; }
    };
    
    
    // Environment::ReactionRequisite
    // --------------------------------------------------------------------------------------------------------------
    
    class ReactionRequisite
    {
    private:
      Apto::List<cReaction*> m_prior_reaction_list;
      Apto::List<cReaction*> m_prior_noreaction_list;
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
      
      const Apto::List<cReaction*>& Reactions() const { return m_prior_reaction_list; }
      const Apto::List<cReaction*>& NoReactions() const { return m_prior_noreaction_list; }
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
