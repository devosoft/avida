/*
 *  cLineage.h
 *  Avida
 *
 *  Called "lineage.hh" prior to 11/30/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cLineage_h
#define cLineage_h

#include <map>
#include "tArray.h"

class cAvidaContext;
class cGenotype;


// struct to compare to genotypes
struct gt_gentype
{
  bool operator()(const cGenotype * g1, const cGenotype * g2) const;
};


class cLineage
{
private:
  friend class cClassificationManager;
  
  int m_id;
  int m_parent_id;
  int m_update_born;
  int m_num_CPUs;
  int m_total_CPUs;
  int m_total_genotypes;
  double m_start_fitness;
  mutable double m_max_fitness;
  mutable double m_ave_fitness;
  double m_generation_born;

  std::map<const cGenotype*, int, gt_gentype> m_genotype_map;
  bool m_threshold;
  mutable bool m_ave_fitness_changed;
  mutable double m_max_fitness_ever;
  double m_lineage_stat1;
  double m_lineage_stat2;

  void CalcCurrentFitness(cAvidaContext& ctx) const;

  /**
    * Creates a new lineage with a given start fitness and parent id.
   * The first genotype has to be added with AddGenotype later.
   *
   * @param start_fitness The initial fitness of the lineage.
   * @param parent_id The id of the parent's lineage.
   * @param id The requested id for this lineage. If negative, a generated
   * value is used. Attention: if you request lineage id's, it is your
   * responsibility not to request two identical ones for different lineages
   * (the class @ref cLineageControl does that correctly).
   **/
  cLineage(double start_fitness, int parent_id, int id, int update, double generation,
           double lineage_stat1 = 0.0, double lineage_stat2 = 0.0);
  
  cLineage(); // @not_implemented
  cLineage(const cLineage&); // @not_implemented
  cLineage& operator=(const cLineage&); // @not_implemented

public:
  ~cLineage() { ; }

  // Adds one instance of the given genotype to the lineage.
  void AddCreature(cAvidaContext& ctx, cGenotype* genotype);

  /**
   * Removes on instance of the given genotype from the lineage.
   *
   * @return True if the removal of this creature potentially creates a new
   * best lineage, otherwise false.
   **/
  bool RemoveCreature(cAvidaContext& ctx, cGenotype* genotype);
  void SetThreshold() { m_threshold = true; }

  int GetID() const { return m_id; }
  int GetParentID() const { return m_parent_id; }
  int GetUpdateBorn() const { return m_update_born; }
  double GetGenerationBorn() const { return m_generation_born; }
  double GetStartFitness() const { return m_start_fitness; }
  double GetMaxFitness() const { return m_max_fitness; }
  double GetAveFitness(cAvidaContext& ctx) const
  {
    if (m_ave_fitness_changed) CalcCurrentFitness(ctx);
    return m_ave_fitness;
  }

  int GetNumGenotypes() const { return m_genotype_map.size(); }
  int GetNumCreatures() const { return m_num_CPUs; }

  /**
   * Counts the number of creatures in the lineage.
   *
   * Should always equal GetNumCreatures(), otherwise something
   * is wrong.
   **/
  int CountNumCreatures() const;

  /**
   * @return The total number of creatures that have ever existed
   * in this lineage.
   **/
  int GetTotalCreatures() const { return m_total_CPUs; }

  /**
   * @return The total number of genotypes that have ever been part
   * of this lineage.
   **/
  int GetTotalGenotypes() const { return m_total_genotypes; }

  bool GetThreshold() const { return m_threshold; }
    
  /**
   * @return The maximum fitness ever attained by this lineage
   **/
  double GetMaxFitnessEver() const { return m_max_fitness_ever; }

  /**
   * @return The value of the first fitness criteria for
   * the creation of this lineage
   **/
  double GetLineageStat1 () const { return m_lineage_stat1; }

  /**
   * @return The value of the second fitness criteria for
   * the creation of this lineage
   **/
  double GetLineageStat2 () const { return m_lineage_stat2; }
	
	tArray<const cGenotype*> GetCurrentGenotypes(cAvidaContext& ctx) const;
};


#ifdef ENABLE_UNIT_TESTS
namespace nLineage {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
