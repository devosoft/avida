# /bin/sh

# set TESTSUITE_DIR to top directory for test scripts.
pushd $srcdir; TESTSUITE_DIR=$PWD; popd
SUBDIR=default.tail           # subdir of TESTSUITE_DIR containing this script.
AVIDA_PATH=$PWD/../../main/primitive  # path to avida executable.

# specify paths to config files.
CFG_DIR=$TESTSUITE_DIR/$SUBDIR
GENESIS_FILE=$CFG_DIR/genesis
INST_SET=$CFG_DIR/inst_set.default
EVENT_FILE=$CFG_DIR/events.cfg
ANALYZE_FILE=$CFG_DIR/analyze.cfg
ENVIRONMENT_FILE=$CFG_DIR/environment.cfg
START_CREATURE=$CFG_DIR/organism.default

# make subdirectory for storing avida's output files.
[ -d $SUBDIR ] || mkdir -p $SUBDIR
cd $SUBDIR || exit  # exit with error status if can't cd into subdir for
                    # storing avida's output files.

# run avida, and save all output to avida_run.log.
$AVIDA_PATH \
  -genesis $GENESIS_FILE \
  -set INST_SET $INST_SET \
  -set EVENT_FILE $EVENT_FILE \
  -set ANALYZE_FILE $ANALYZE_FILE \
  -set ENVIRONMENT_FILE $ENVIRONMENT_FILE \
  -set START_CREATURE $START_CREATURE \
  2>&1 > avida_run.log \
  || exit  # if avida run fails, exit with avida's error status.

# verify last line of detail_pop.100 matches contents of file
# detail_pop.100.last_line.expected.
tail -n 1 detail_pop.100 > detail_pop.100.last_line
cmp detail_pop.100.last_line $CFG_DIR/detail_pop.100.last_line.expected \
  && echo "Last line of detail_pop.100 is as expected." \
  || exit  # if last line doesn't match, exit with error status.



  # Uncomment section below for demo of test failure.

### run avida, and save all output to avida_run.log.
##$AVIDA_PATH \
##  -genesis $GENESIS_FILE \
##  -seed 0 \
##  -set INST_SET $INST_SET \
##  -set EVENT_FILE $EVENT_FILE \
##  -set ANALYZE_FILE $ANALYZE_FILE \
##  -set ENVIRONMENT_FILE $ENVIRONMENT_FILE \
##  -set START_CREATURE $START_CREATURE \
##  2>&1 > avida_run.log \
##  || exit  # if avida run fails, exit with avida's error status.
##
### verify last line of detail_pop.100 matches contents of file
### detail_pop.100.last_line.expected.
##tail -n 1 detail_pop.100 > detail_pop.100.last_line
##cmp detail_pop.100.last_line $CFG_DIR/detail_pop.100.last_line.expected \
##  && echo "Last line of detail_pop.100 is as expected." \
##  || exit  # if last line doesn't match, exit with error status.
