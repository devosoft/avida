# /bin/sh

# read command-line arguments:
TESTSUITE_DIR=$1       # path to top directory for test scripts.
SUBDIR=$2              # subdir of TESTSUITE_DIR containing this script.
AVIDA_PATH=$3          # path to avida executable.

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
cd $SUBDIR

# run avida, and save all output to avida_run.log.
$AVIDA_PATH \
  -genesis $GENESIS_FILE \
  -set INST_SET $INST_SET \
  -set EVENT_FILE $EVENT_FILE \
  -set ANALYZE_FILE $ANALYZE_FILE \
  -set ENVIRONMENT_FILE $ENVIRONMENT_FILE \
  -set START_CREATURE $START_CREATURE \
  2>&1 > avida_run.log

# send the md5sum of detail_pop.100 back to the testdriver for
# verification.
echo "md5sum of output file detail_pop.100:"
md5sum detail_pop.100

# sleeping for one second gives dejagnu/expect time to react.
sleep 1;
