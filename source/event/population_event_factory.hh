//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_EVENT_FACTORY_HH
#define POPULATION_EVENT_FACTORY_HH

#ifndef EVENT_FACTORY_HH
#include "event_factory.hh"
#endif

class cEvent;
class cPopulation;
class cString;

class cPopulationEventFactory : public cEventFactory {
private:
  cPopulation *m_population;
public:
  // event enums
  enum eEvent { EVENT_undefined = 0,
    EVENT_exit,
    EVENT_exit_if_generation_greater_than,
    EVENT_exit_if_update_greater_than,
    EVENT_exit_if_ave_lineage_label_smaller,
    EVENT_exit_if_ave_lineage_label_larger,
    EVENT_echo,
    EVENT_print_data,
    EVENT_print_average_data,
    EVENT_print_error_data,
    EVENT_print_variance_data,
    EVENT_print_dominant_data,
    EVENT_print_stats_data,
    EVENT_print_count_data,
    EVENT_print_totals_data,
    EVENT_print_tasks_data,
    EVENT_print_tasks_exe_data,
    EVENT_print_resource_data,
    EVENT_print_time_data,
    EVENT_print_mutation_data,
    EVENT_print_mutation_rate_data,
    EVENT_print_divide_mut_data,
    EVENT_print_dom_parasite_data,
    EVENT_print_instruction_data,
    EVENT_print_instruction_abundance_histogram,
    EVENT_print_depth_histogram,
    EVENT_print_genotype_abundance_histogram,
    EVENT_print_species_abundance_histogram,
    EVENT_print_lineage_totals,
    EVENT_print_lineage_counts,
    EVENT_print_dom,
    EVENT_parasite_debug,
    EVENT_print_dom_parasite,
    EVENT_print_genotype_map,
    EVENT_print_number_phenotypes,
    EVENT_print_phenotype_status,
    EVENT_save_population,
    EVENT_load_population,
    EVENT_save_clone,
    EVENT_load_clone,
    EVENT_load_dump_file,
    EVENT_dump_pop,
    EVENT_print_genotypes,
    EVENT_detail_pop,
    EVENT_detail_sex_pop,
    EVENT_detail_parasite_pop,
    EVENT_dump_historic_pop,
    EVENT_dump_historic_sex_pop,
    EVENT_dump_memory,
    EVENT_inject,
    EVENT_inject_all,
    EVENT_inject_range,
    EVENT_inject_sequence,
    EVENT_inject_random,
    EVENT_inject_range_parasite,
    EVENT_inject_range_pair,
    EVENT_zero_muts,
    EVENT_mod_copy_mut,
    EVENT_mod_div_mut,
    EVENT_set_copy_mut,
    EVENT_mod_point_mut,
    EVENT_set_point_mut,
    EVENT_calc_landscape,
    EVENT_predict_w_landscape,
    EVENT_predict_nu_landscape,
    EVENT_sample_landscape,
    EVENT_random_landscape,
    EVENT_analyze_landscape,
    EVENT_pairtest_landscape,
    EVENT_test_dom,
    EVENT_analyze_population,
    EVENT_print_detailed_fitness_data,
    EVENT_print_genetic_distance_data,
    EVENT_genetic_distance_pop_dump,
    EVENT_task_snapshot,
    EVENT_print_viable_tasks_data,
    EVENT_apocalypse,
    EVENT_kill_rectangle,
    EVENT_rate_kill,
    EVENT_serial_transfer,
    EVENT_hillclimb,
    EVENT_hillclimb_neut,
    EVENT_hillclimb_rand,
    EVENT_compete_demes,
    EVENT_reset_demes,
    EVENT_print_deme_stats,
    EVENT_copy_deme,
    EVENT_calc_consensus,
    EVENT_test_size_change_robustness,
    EVENT_test_threads,
    EVENT_print_threads,
    EVENT_dump_fitness_grid,
    EVENT_dump_genotype_grid,
    EVENT_dump_task_grid,
    EVENT_dump_donor_grid,
    EVENT_dump_receiver_grid,
    EVENT_print_tree_depths,
    EVENT_sever_grid_col,
    EVENT_sever_grid_row,
    EVENT_join_grid_col,
    EVENT_join_grid_row,
    EVENT_connect_cells,
    EVENT_disconnect_cells,
    EVENT_inject_resource,
    EVENT_set_resource,
    EVENT_inject_scaled_resource,
    EVENT_outflow_scaled_resource,
    EVENT_set_reaction_value,
    EVENT_set_reaction_value_mult,
    EVENT_NO_EVENT };
  
  cPopulationEventFactory( cPopulation *pop );
  ~cPopulationEventFactory();

  int EventNameToEnum(const cString & name) const;
  cEvent * ConstructEvent(int event_enum, const cString & args );
};

#endif
