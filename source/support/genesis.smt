#############################################################################
# This file includes all the basic run-time defines for avida.
# For more information, see doc/genesis.html
#############################################################################

VERSION_ID 2.3.1		# Do not change this value!

### Architecture Variables ###
MAX_UPDATES  -1         # Maximum updates to run simulation (-1 = no limit)
MAX_GENERATIONS -1      # Maximum generations to run simulation (-1 = no limit)
END_CONDITION_MODE 0    # End run when ...
                        # 0 = MAX_UPDATES _OR_ MAX_GENERATIONS is reached
                        # 1 = MAX_UPDATES _AND_ MAX_GENERATIONS is reached
WORLD-X 30              # Width of the world in Avida mode.
WORLD-Y 30              # Height of the world in Avida mode.
RANDOM_SEED 0           # Random number seed. (0 for based on time)
HARDWARE_TYPE 2         # 0 = Original CPUs
                        # 1 = New, Stack-based CPUs
                        # 2 = Newer -- Stacks, Memory, Threading
MAX_CPU_THREADS 1       # Number of Threads CPUs can spawn

### Configuration Files ###
DEFAULT_DIR ../Avida2.work/              # Directory in which config files are found
INST_SET inst_set.smt             # File containing instruction set
EVENT_FILE events.cfg             # File containing list of events during run
ANALYZE_FILE analyze.cfg          # File used for analysis mode
ENVIRONMENT_FILE environment.cfg  # File that describes the environment
START_CREATURE organism.smt       # Organism to seed the soup

### Reproduction ###
BIRTH_METHOD 4   # 0 = Replace random organism in neighborhood
		  # 1 = Replace oldest organism in neighborhood
		  # 2 = Replace largest Age/Merit in neighborhood
		  # 3 = Place only in empty cells in neighborhood
		  # 4 = Replace random from entire population (Mass Action)
		  # 5 = Replace oldest in entire population (like Tierra)
DEATH_METHOD 0    # 0 = Never die of old age.
		  # 1 = Die when inst executed = AGE_LIMIT (with deviation)
		  # 2 = Die when inst executed = length * AGE_LIMIT (+ dev.)
AGE_LIMIT 5000    # Modifies DEATH_METHOD
AGE_DEVIATION 0   # Modified DEATH_METHOD
ALLOC_METHOD 0    # 0 = Allocated space is set to default instruction.
                  # 1 = Set to section of dead genome (Necrophilia)
                  # 2 = Allocated space is set to random instruction.
DIVIDE_METHOD 1   # 0 = Divide leaves state of mother untouched.
                  # 1 = Divide resets state of mother
                  #     (after the divide, we have 2 children)
		  # 2 = Divide resets only the current thread of the mother
		  #     (useful in 4-stack CPU w/ parasites)

GENERATION_INC_METHOD 1 # 0 = Only the generation of the child is
                        #     increased on divide.
			# 1 = Both the generation of the mother and child are
			#     increased on divide (good with DIVIDE_METHOD 1).

### Divide Restrictions ####
CHILD_SIZE_RANGE 2.0	# Maximal differential between child and parent sizes.
MIN_COPIED_LINES 0.5    # Code fraction which must be copied before divide.
MIN_EXE_LINES    0.5    # Code fraction which must be executed before divide.
REQUIRE_ALLOCATE   1    # Is a an allocate required before a divide? (0/1)
REQUIRED_TASK -1  # Number of task required for successful divide.

### Mutations ###

# mutations that occur during execution..
POINT_MUT_PROB  0.0     # Mutation rate (per-location per update)
COPY_MUT_PROB   0.01  # Mutation rate (per copy).

# mutations that occur on divide...
INS_MUT_PROB    0.0     # Insertion rate (per site, applied on divide).
DEL_MUT_PROB    0.0     # Deletion rate (per site, applied on divide).
DIV_MUT_PROB    0.0     # Mutation rate (per site, applied on divide).
DIVIDE_MUT_PROB 0.0     # Mutation rate (per divide).
DIVIDE_INS_PROB 0.05    # Insertion rate (per divide).
DIVIDE_DEL_PROB 0.05    # Deletion rate (per divide).
PARENT_MUT_PROB 0.0     # Per-site, in parent, on divide

# heads based mutations
# READ_SHIFT_PROB   0.0
# READ INS_PROB     0.0
# READ_DEL_PROB     0.0
# WRITE_SHIFT_PROB  0.0
# WRITE_INS_PROB    0.0
# WRITE_DEL_PROB    0.0


### Mutation reversions ###
# these slow down avida a lot, and should be set to 0 normally.
REVERT_FATAL       0.0  # Should any mutations be reverted on birth?
REVERT_DETRIMENTAL 0.0  #   0.0 to 1.0; Probability of reversion.
REVERT_NEUTRAL     0.0
REVERT_BENEFICIAL  0.0

STERILIZE_FATAL       0.0  # Should any mutations clear (kill) the organism?
STERILIZE_DETRIMENTAL 0.0  #   0.0 to 1.0; Probability of reset.
STERILIZE_NEUTRAL     0.0
STERILIZE_BENEFICIAL  0.0

FAIL_IMPLICIT     0	# Should copies that failed *not* due to mutations
			# be eliminated?

### Time Slicing ###
AVE_TIME_SLICE 30
SLICING_METHOD 2	# 0 = CONSTANT: all organisms get default...
			# 1 = PROBABILISTIC: Run _prob_ proportional to merit.
			# 2 = INTEGRATED: Perfectly integrated deterministic.
SIZE_MERIT_METHOD 4	# 0 = off (merit is independent of size)
			# 1 = Merit proportional to copied size
			# 2 = Merit prop. to executed size
			# 3 = Merit prop. to full size
			# 4 = Merit prop. to min of executed or copied size
			# 5 = Merit prop. to sqrt of the minimum size
TASK_MERIT_METHOD 1	# 0 = No task bonuses
			# 1 = Bonus just equals the task bonus
THREAD_SLICING_METHOD 1 # 0 = One thread executed per time slice.
			# 1 = All threads executed each time slice.
 			# Formula for an organism's thread slicing: 
			# 1 + (num_organism_threads-1) * THREAD_SLICING_METHOD

MAX_LABEL_EXE_SIZE 1	# Max nops marked as executed when labels are used
MERIT_TIME 1            # 0 = Merit Calculated when task completed
		        # 1 = Merit Calculated on Divide
MAX_NUM_TASKS_REWARDED -1  # -1 = Unlimited

### Genotype Info ###
THRESHOLD 3		# Number of organisms in a genotype needed for it
			#   to be considered viable.
GENOTYPE_PRINT 0	# 0/1 (off/on) Print out all threshold genotypes?
GENOTYPE_PRINT_DOM 0	# Print out a genotype if it stays dominant for
                        #   this many updates. (0 = off)
SPECIES_THRESHOLD 2     # max failure count for organisms to be same species
SPECIES_RECORDING 0	# 1 = full, 2 = limited search (parent only)
SPECIES_PRINT 0		# 0/1 (off/on) Print out all species?
TEST_CPU_TIME_MOD 20    # Time allocated in test CPUs (multiple of length)
TRACK_MAIN_LINEAGE 1    # Track primary lineage leading to final population?

### Log Files ###
LOG_CREATURES 0		# 0/1 (off/on) toggle to print file.
LOG_GENOTYPES 0		# 0 = off, 1 = print ALL, 2 = print threshold ONLY.
LOG_THRESHOLD 0		# 0/1 (off/on) toggle to print file.
LOG_SPECIES 0		# 0/1 (off/on) toggle to print file.
LOG_LANDSCAPE 0		# 0/1 (off/on) toggle to print file.

LOG_LINEAGES 0          # 0/1 (off/on) to log advantageous mutations
# This one can slow down avida a lot. It is used to get an idea of how
# often an advantageous mutation arises, and where it goes afterwards.
# See also LINEAGE_CREATION_METHOD.

LINEAGE_CREATION_METHOD 0
# Lineage creation options are.  Works only when LOG_LINEAGES is set to 1.
#   0 = manual creation (on inject, use successive integers as lineage labels).
#   1 = when a child's (potential) fitness is higher than that of its parent.
#   2 = when a child's (potential) fitness is higher than max in population.
#   3 = when a child's (potential) fitness is higher than max in dom. lineage
#	*and* the child is in the dominant lineage, or (2)
#   4 = when a child's (potential) fitness is higher than max in dom. lineage
#	(and that of its own lineage)
#   5 = same as child's (potential) fitness is higher than that of the
#       currently dominant organism, and also than that of any organism
#       currently in the same lineage.
#   6 = when a child's (potential) fitness is higher than any organism
#       currently in the same lineage.
#   7 = when a child's (potential) fitness is higher than that of any
#       organism in its line of descent

### END ###


