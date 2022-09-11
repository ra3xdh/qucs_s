#!/bin/bash
# run_tests.sh -- run this script to execute python tests
#

NO_CORES=$(nproc --all)
if [ -z "${QUCS_HOME}" ]; then
  QUCS_HOME=~/local/qucs
fi
if [ -z "${QUCS_HOME2}" ]; then
  QUCS_HOME2=~/tmp/qucs
fi

if [ ! -d "${QUCS_HOME}" ]; then
    echo "QUCS_HOME not set properly!"
    exit -1
fi

#python3 run.py --prefix ${QUCS_HOME}/bin/ --exclude skip.txt --qucs -mp ${NO_CORES}
python3 run.py --prefix ${QUCS_HOME}/bin/ --exclude skip.txt --qucsator -mp ${NO_CORES}
#python3 run.py --prefix ${QUCS_HOME}/bin/ --exclude skip.txt -p all #print all
#python3 run.py --prefix ${QUCS_HOME}/bin/ --compare ${QUCS_HOME}/bin ${QUCS_HOME2}/bin
#python3 run.py --prefix ${QUCS_HOME}/bin/ --exclude skip.txt --qucs --plot-interactive

#python3 run_equations.py --prefix ${QUCS_HOME}/bin/

# see .gitignore
#find . -type f -name "test_*" -exec rm {} \;
#find . -type f -name "*.pyc" -exec rm {} \;

