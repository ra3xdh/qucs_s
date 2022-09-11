#!/usr/bin/env python

'''
  Notes:
  * it skips subcircut marker '.Def'
  * there is no indication that a Spice file is translated into the netlist.
    The only hint is the '_cir' appended tot he definition .DEF, which is skiped

  TODO
  * mark versions when comparing for NUM_FAIL

'''

import argparse
import logging
import datetime
import numpy as np
import os
import platform
import pprint
import subprocess
if (os.name != 'nt') or (platform.system() != 'Darwin'): # multiprocessing is tricky to use
    import multiprocessing
import shutil
import sys
import threading
import time
from distutils.version import LooseVersion


from qucstest.colors import pb, pg, pr, py
from qucstest.schematic import *
from qucstest.netlist import *
from qucstest.report import *
from qucstest.qucsdata import QucsData
from qucstest.qucsator import *
from qucstest.qucsgui import *
from qucstest.figures import *
from qucstest.misc import *


class Test:
   '''
   Object used to store information related to a test.
   '''
   def __init__(self, name):
       # test name (the project directory name)
       self.name = name
       # full path to the project
       self.path = ''
       # name of the source schematic
       self.schematic = ''
       # version of the schematic
       self.version= ''
       # default dataset on schematic
       self.dataset = ''
       # reference netlist (typically the basename as the schematic)
       self.netlist = ''
       # list of components being simulated
       self.comp_types = []
       # list of simulations being performed
       self.sim_types = []
       # test status [PASS, FAIL, NUM_FAIL, TIME_FAIL]
       self.status = ''
       # time it took to run the test
       self.runtime = ''
       # message related to status
       self.message = ''
       # list of traces that resulted in NUM_FAIL
       self.failed_traces = []
       # next variables are used for: abs(test - ref) <= (atol + rtol * abs(ref) )
       # relative error
       self.rtol = 0.0
       # absolute error
       self.atol = 0.0

   def debug(self):
       print('name         :', self.name)
       print('schematic    :', self.schematic)
       print('version      :', self.version)
       print('dataset      :', self.dataset)
       print('netlist      :', self.netlist)
       print('comp_types   :', self.comp_types)
       print('sim_types    :', self.sim_types)
       print('status       :', self.status)
       print('runtime      :', self.runtime)
       print('message      :', self.message)
       print('failed_traces:', self.failed_traces)
       print('test rtol    :', self.rtol)
       print('test atol    :', self.atol)

   def getSchematic(self):
       if not self.schematic:
           # get schematic name from direcitory name
           # trim the simulation types
           sim_types= ['DC_', 'AC_', 'TR_', 'SP_', 'SW_']
           name = self.name
           for sim in sim_types:
               if sim in name:
                   name=name[3:]
           self.schematic = name[:-4]+'.sch'
       return self.schematic


class qucsfile:
   '''
   Hold commonly used data about a Qucs schematic or data display file
   '''
   def __init__(self, name):
      # schematic name
      self.name = name
      # schematic type
      self.type = ''
      # schematic version
      self.version = ''
      # test status [PASS, FAIL]
      self.status = ''
      # time it took to print
      self.runtime = ''
      # message related to status
      self.message = ''

class Print_test:
   '''
   Object used to store information related to a GUI print test.
   '''
   def __init__(self, name, prj_dir):
       # test name (the project directory name)
       self.name = name
       # full path to the project
       self.path = os.path.join(prj_dir, name)
       # project files (schematics and data displays)
       self.files = []

   def debug(self):
       print('name            :', self.name)
       print('path            :', self.path)
       for sch in self.files:
          print('  file          :', sch.name)
          print('  type          :', sch.type)
          print('  version       :', sch.version)
          print('  status        :', sch.status)
          print('  runtime       :', sch.runtime)
          print('  message       :', sch.message)

   def add_all_files(self, suffix):
      all_sch = [qucsfile(f) for f in sorted(os.listdir(self.path))
                 if f.endswith("."+suffix)
                 if os.path.isfile(os.path.join(self.path, f))]
      for sch in all_sch:
         # get_sch_version() works for both .sch and .dpl
         sch.version = get_sch_version(os.path.join(self.path, sch.name))
         sch.type = suffix
      self.files += all_sch
      return all_sch


#http://stackoverflow.com/questions/1191374/subprocess-with-timeout
class Command(object):
    '''
    Class used to run a subprocess call with timeout.
    '''
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None
        self.timeout = False
        self.retcode = 0

    def run(self, timeout):
        def target():
            logger.debug( pb('Thread started') )
            self.process = subprocess.Popen(self.cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = self.process.communicate()
            # keep the stdout and stderr
            self.out = out
            self.err = err
            logger.debug( pb('Thread finished') )

        thread = threading.Thread(target=target)
        thread.start()

        thread.join(timeout)
        if thread.is_alive():
            self.timeout = True
            print(pr('Terminating process, timed out %i s' %timeout))
            self.process.terminate()
            thread.join()
        self.retcode =  self.process.returncode
        # NOTE: a negative returncode -N indicates that the child
        #   was terminated by signal N (Unix only)
        if self.retcode:
            logger.warn( pr('Process return code: %i' %self.retcode) )
        else:
            logger.info( pb('Process return code: %i' %self.retcode) )


def get_subdirs(dir):
    '''
    Return a list of names of subdirectories.

    :param dir: dir to look up for subdirs.
    :return: list of subdirec
    '''
    return [name for name in sorted(os.listdir(dir))
            if os.path.isdir(os.path.join(dir, name))]



def compare_datasets(ref_dataset, test_dataset, rtol, atol):
    '''
    Compare two datasets for numerical differences.

    :param ref_dataset : reference dataset
    :param test_dataset: test dataset
    :param rtol: relative tolerance
    :param atol: absolute tolerance
    :return failed: list of traces that failed numerical check
    '''

    from qucstest.colors import pb, pr, pg

    if not os.path.isfile(ref_dataset):
        sys.exit('No reference dataset: %s' %ref_dataset)
    if not os.path.isfile(test_dataset):
        sys.exit('No test dataset: %s' %test_dataset)

    # TODO failed also catches if the solver didn't run, output_dataset will be empty,
    # it will fail the comparison

    # let's compare results

    # list of failed variable comparisons
    failed=[]

    logger.info( pb('load data %s' %(ref_dataset)) )
    ref = QucsData(ref_dataset)

    logger.info( pb('load data %s' %(test_dataset)) )
    test = QucsData(test_dataset)

    logger.info( pb('Comparing dependent variables [rtol=%s, atol=%s]' %(rtol, atol)) )

    for name in list(ref.dependent.keys()):
        ref_trace  = ref.data[name]
        test_trace = test.data[name]

        # check: abs(test - ref) <= (atol + rtol * abs(ref) )
        if not np.allclose(test_trace, ref_trace, rtol=rtol, atol=atol):
            logger.warning( pr('  Failed %s' %(name)) )
            failed.append(name)
        else:
            logger.info(pg('  Passed %s' %(name)) )

    return failed


def run_simulation(test, qucspath, plot_interactive=False):
    '''
    Run simulation from reference netlist and compare outputs (dat, log)

    :param test: test object containing the test info
    :param qucspath: path containing qucsator
    :param plot_interactive: plot graphs as data is compared
    '''

    name = test.getSchematic()


    test_dir = os.getcwd()

    proj_dir = os.path.join(test_dir, 'testsuite', test.name)
    test.path = proj_dir
    print('\nProject : ', proj_dir)


    input_net = os.path.join(proj_dir, "netlist.txt")
    if not os.path.isfile(input_net):
        sys.exit('Input netlist not found')

    # fetch types of simulation an types of components
    comps = get_net_components(input_net)
    sim = get_net_simulations(input_net)

    test.comp_types = comps
    test.sim_types = sim

    # get the Qucs Schematic version from the schematic
    schematic = os.path.join(proj_dir, test.schematic)

    test.version = get_sch_version(schematic)
    test.dataset =  get_sch_dataset(schematic)

    output_dataset = os.path.join(proj_dir, "test_"+test.dataset)

    ext = '' if os.name != 'nt' else '.exe'
    cmd = [os.path.join(qucspath, "qucsator"+ext), "-i", input_net, "-o", output_dataset]
    print('Running : ', ' '.join(cmd))

    # TODO run a few times, record average/best of 3
    # call the solver in a subprocess, set the timeout
    tic = time.time()
    command = Command(cmd)
    command.run(timeout=maxTime)
    toc = time.time()
    runtime = toc - tic

    # If return code, ignore time
    if command.retcode:
        test.status = 'FAIL'
        test.message = 'FAIL CODE %i' %command.retcode
    elif command.timeout:
        test.status = 'TIME_FAIL'
        test.message = 'TIMEOUT'
    else:
        test.status = 'PASS'
        test.runtime = '%f' %runtime

    logger.info( pb('Runtime: %f' %runtime) )

    if (command.timeout):

        errout =  os.path.join(proj_dir, 'error_timeout.txt')
        print(pr('Failed with timeout, saving: \n   %s' % errout))
        with open(errout, 'w') as myFile:
            myFile.write(command.err.decode("iso-8859-1"))

    if (command.retcode):
        errout = os.path.join(proj_dir, 'error_code.txt')
        print(pr('Failed with error code, saving: \n   %s' % errout))
        with open(errout, 'w') as myFile:
            myFile.write(command.err.decode("iso-8859-1"))

    # perform result comparison
    if (not command.timeout) and (command.retcode==0):
        ref_dataset = os.path.join(proj_dir, get_sch_dataset(schematic))

        numerical_diff = compare_datasets(ref_dataset, output_dataset, rtol=test.rtol, atol=test.atol)
        if numerical_diff:
            test.failed_traces = numerical_diff
            test.status = 'NUM_FAIL'

        # show all traces
        if plot_interactive:
            plot_error(ref_dataset, output_dataset, list(QucsData(ref_dataset).dependent.keys()), show=plot_interactive)

        # quiet save of fail numerical check
        if numerical_diff:
            plot_error(ref_dataset, output_dataset, test.failed_traces)

    return test

def print_project(print_test, qucspath, what):
    '''
    Print project content (.sch and/or .dpl files)

    :param print_test: print_test object containing the project
    :param qucspath: path containing qucsator
    :param what: type of files to print, 'sch', 'dpl' or 'all'
    '''
    for f in print_test.files     :
       # schematic/data display name, without suffix
       f_basename = os.path.splitext(os.path.basename(f.name))[0]
       in_f = os.path.join(print_test.path, f.name)
       if (f.type == 'sch'):
          out_print = os.path.join(print_test.path, f_basename+".pdf")
       else: # 'dpl'
          out_print = os.path.join(print_test.path, f_basename+"_dpl.pdf")
       ext = '' if os.name != 'nt' else '.exe'
       cmd = [os.path.join(qucspath, "qucs"+ext), "-p", "-i", in_f, "-o", out_print]
       print('Running : ', ' '.join(cmd))

       tic = time.time()
       command = Command(cmd)
       command.run(timeout=maxTime)
       toc = time.time()
       runtime = toc - tic

       # If return code, ignore time
       if command.retcode:
          f.status = 'FAIL (%i)'% command.retcode
          f.message = 'FAIL CODE %i' % command.retcode
       elif command.timeout:
          f.status = 'TIME_FAIL'
          f.message = 'TIMEOUT'
       else:
          f.status = 'PASS'
          f.runtime = '%f' % runtime

    return print_test


def add_test_project(sch):
    '''
    Add a schematic file as a test on the testsuite.

    - create directory (start with simulation types, ex. DC_TR_myCircuit_prj)
    - search and copy related subcircuits
    - TODO search and copy included SPICE files
    - initialize reference netlis
    - initialize reference data file
    - TODO initialize SPICE, run qucsconv

    :param sch: path to a schematic file (.sch)
    :return: destination directory
    '''

    print(pb('Adding new project to test-suite.'))
    print('Adding schematic: %s' %(sch))

    # get schematic basename
    sch_name = os.path.splitext(os.path.basename(sch))[0]

    # scan schematic for types of simulation [.DC, .AC, .TR, .SP, .SW]
    # create dir, concatenate simulation type(s), schematic name, append '_prj'
    # ex. TR_myCircuit_prj, DC_AC_TR_complexCircuit_prj
    sim_used = get_sch_simulations(sch)
    sim_found = ''
    for sim in sim_used:
        #skip dot, prepend simulation types
        sim_found+=sim[1:]+'_'
    if not sim_found:
        sys.exit( pr('This schematic performs no simulation, is it a subcircuit?'))
    dest = sim_found + sch_name + '_prj'

    # scan for subcircuits, to be copied over to destination
    sub_files = get_sch_subcircuits(sch)

    dest_dir = os.path.join(os.getcwd(),'testsuite', dest)
    if not os.path.exists(dest_dir):
        print('Creating directory:', dest_dir)
        os.makedirs(dest_dir)
    else:
        print('Use existing directory:', dest_dir)

    # copy schematic
    shutil.copy2(sch, dest_dir)

    # copy listed subcircuit (recursive)
    for sub in sub_files:
        print('Copying sub-circuit', sub)
        src = os.path.join(os.path.dirname(sch),sub)
        if os.path.isfile(src):
            shutil.copy2(src, dest_dir)
        else:
            sys.exit(pr('Oops, subcircuit not found: ', src))

    return dest_dir


def parse_options():
    '''
    Helper to handle the command line option parsing.

    :return: parsed command line options
    '''

    parser = argparse.ArgumentParser(description='Qucs testing script.')

    parser.add_argument('--prefix', type=str,
                       help='prefix of installed Qucs (default: /usr/local/bin/)')

    parser.add_argument('--qucs',
                       action='store_true',
                       help='run qucs tests')

    parser.add_argument('--qucsator',
                       action='store_true',
                       help='run qucsator tests')

    parser.add_argument('-p', '--print', type=str,
                       choices=['sch', 'dpl', 'all'],
                       nargs='?',
                       const='all',
                       help='run qucs and prints schematics and/or data displays to file',
                       dest='qprint') # as args.print will choke...

    parser.add_argument('--add-test', type=str, metavar='FILE',
                       help='add schematic file to the testsuite')

    parser.add_argument('--exclude', type=str, metavar='FILE',
                       help='file listing projects excluded from test')

    parser.add_argument('--include', type=str, metavar='FILE',
                       help='file listing projects selected for test')

    parser.add_argument('--project', type=str,
                       help='path to a test project')

    parser.add_argument('--compare', nargs='+', type=str, metavar='PATH',
                       help='two full paths to directories containing '
                             'qucs or qucsator binaries for comparison test')

    parser.add_argument("-v", "--verbose", const=1, default=0,
                        type=int, nargs="?", metavar='LEVEL',
                        help="increase verbosity: 0 = only warnings, 1 = info, 2 = debug. No number means info. Default is no verbosity.")

    parser.add_argument('--reset',
                       action='store_true',
                       help='Reset (overwrite) data and log files of test projects.'
                            'Run qucsator given with --prefix.')

    parser.add_argument('--timeout', type=int, default=90,
                       help='Abort test if longer that timeout (default: 90 s).')

    parser.add_argument('--rtol', type=float, default=1e-1,
                       help='Set the element-wise relative tolerace (default 1e-1).\n'
                            'See: Numpy allclose function.')

    parser.add_argument('--atol', type=float, default=1e-5,
                       help='Set the element-wise absolute tolerace (default 1e-5).\n'
                            'See: Numpy allclose function.')

    parser.add_argument('--plot-interactive', action='store_true',
                       help='Plot and show error graphs interactively.\n'
                            'Hardcopy PNG saved by default.')
    if (os.name != 'nt'): # on windows multiprocessing is tricky to use
        parser.add_argument('-mp', '--processes', nargs="?", default=1, type=int, metavar='NUM',
                        help='Use %(metavar)s processes to run the simulations (default: number of CPU cores).') # if no value is specified the default 'None' will be used

    args = parser.parse_args()
    return args



if __name__ == '__main__':

    args = parse_options()
    #print(args)


    # set global values, default or overrides
    maxTime = args.timeout
    rtol = args.rtol
    atol = args.atol


    print(pb('Using Max. time: %s' %str(maxTime)))
    print(pb('Using rtol: %s' %str(rtol)))
    print(pb('Using atol: %s' %str(atol)))


    # setup logger
    logger = logging.getLogger()
    logging.basicConfig(format='%(levelname)s:%(message)s')
    if args.verbose == 0:
        logger.setLevel(logging.WARN)
    elif args.verbose == 1:
        logger.setLevel(logging.INFO)
    elif args.verbose == 2:
        logger.setLevel(logging.DEBUG)


    # TODO improve the discovery of qucs, qucator
    prefix_given = False
    if args.prefix:
        #prefix = os.path.join(args.prefix, 'bin', os.sep)
        prefix = args.prefix
        prefix_given = True
    else:
        # use default paths, build location, system locations
        prefix = os.path.join('')

    if args.compare:
        prefixes = args.compare
        if (args.qucsator):
           logger.info( pb('Comparing the following qucsators:') )
           for qp in prefixes:
              ext = '' if os.name != 'nt' else '.exe'
              if os.path.isfile(os.path.join(qp, 'qucsator'+ext)):
                 print(pb('%s' %(qp)))
              else:
                 sys.exit(pr("No qucsator binary found in: %s" %(qp)))
        else:
           for qp in prefixes:
              ext = '' if os.name != 'nt' else '.exe'
              if os.path.isfile(os.path.join(qp, 'qucs'+ext)):
                 print(pb('%s' %(qp)))
              else:
                 sys.exit(pr("No qucs binary found in: %s" %(qp)))
    elif not prefix_given:
        pass
    else:
       prefixes = [prefix]

       if (args.qucs or args.qprint):
          ext = '' if os.name != 'nt' else '.exe'
          if os.path.isfile(os.path.join(prefix, 'qucs'+ext)):
             print(pb('Found Qucs in: %s' %(prefix)))
          else:
             sys.exit(pr('Oh dear, Qucs not found in: %s' %(prefix)))

       if (args.qucsator or args.reset):
          ext = '' if os.name != 'nt' else '.exe'
          if os.path.isfile(os.path.join(prefix, 'qucsator'+ext)):
             print(pb('Found Qucsator in: %s' %(prefix)))
          else:
             sys.exit(pr('Oh dear, Qucsator not found in: %s' %(prefix)))


    # get single project or list of test-projects
    if args.project:
        testsuite =  [os.path.join(args.project)]
    else:
        testsuite = get_subdirs('./testsuite/')

    # TODO read list of: skip, testshort, testlong

    if args.exclude:
        skip = args.exclude
        with open(skip) as fp:
            for line in fp:
                skip_proj = line.split(',')[0]
                if skip_proj in testsuite:
                    print(py('Skipping %s' %skip_proj))
                    testsuite.remove(skip_proj)

    if args.include:
        add = args.include
        include = []
        with open(add) as fp:
            for line in fp:
                proj = line.split(',')[0]
                if proj in testsuite:
                    print(pg('Including %s' %proj))
                    include.append(proj)
        if include:
            testsuite = include

    # Toggle if any test fail
    returnStatus = 0

    if args.qucs or args.qucsator or args.project:
        print('\n')
        print(pb('******************************************'))
        print(pb('** Test suite - Selected Test Projects  **'))
        print(pb('******************************************'))

        # Print list of selected tests
        pprint.pprint(testsuite)

    #
    # Run Qucs GUI
    #
    if args.qucs:
        print('\n')
        print(pb('** Test schematic to netlist conversion **'))

        # loop over testsuite
        # messages are added to the dict, project as key
        net_report = {}
        for test in testsuite:

            dest_dir = os.path.join('testsuite', test)

            projName = test.strip(os.sep)
            # get schematic name from direcitory name
            # trim the simulation types
            sim_types= ['DC_', 'AC_', 'TR_', 'SP_', 'SW_']
            for sim in sim_types:
                if sim in projName:
                    projName=projName[3:]
            projName = projName[:-4]

            # generate test_ netlist
            input_sch = os.path.join(dest_dir, projName+'.sch')

            # skip future versions of schematic
            sch_version = get_sch_version(input_sch)
            qucs_version = get_qucs_version(prefix).decode().split(' ')[1]

            if LooseVersion(sch_version) > LooseVersion(qucs_version):
                print(pb("Warning: skipping future version of schematic"))
                print(pb("  Using qucs %s with schematic version %s"
                         %(qucs_version, sch_version)))
                continue

            # go on to create a fresh test_netlist.txt
            test_net  = os.path.join(dest_dir, 'test_'+projName+'.txt')
            sch2net(input_sch, test_net, prefix)

            ref_netlist = os.path.join(dest_dir, 'netlist.txt')

            # diff netlists: reference and test_
            print('Comparing : diff %s %s' %(ref_netlist, test_net))
            net_equal, bad_lines = check_netlist(ref_netlist, test_net)

            if net_equal:
                print(pg('Diff netlist    : PASS'))
            else:
                print(pr('Diff netlist    : FAIL'))
                net_report[test] = bad_lines
                returnStatus = -1

        print('\n')
        print(pb('############################################'))
        print(pb('#  Report schematic to netlist conversion  #'))

        if list(net_report.keys()):
            print(pr('--> Found differences (!)'))
            pprint.pprint(net_report)
        else:
            print(pg('--> No differences found.'))

    #
    # Run Qucs simulator
    #
    if args.qucsator:
        print('\n')
        print(pb('********************************'))
        print(pb('** Test simulation and output **'))
        print(pb('********************************'))

        # collect all reports, sim_collect will be a list of dicts,
        # one list for each qpath. Each dict contains the report output
        # for each simulation performed
        #sim_collect = []
        # fail will be a list of lists, one for each qpath. Each sub-list
        # contains information on failed tests
        #fail = []

        show_plot = args.plot_interactive

        collect_tests = []
        # loop over prefixes (possibly just one)
        for qucspath in prefixes:
            if (os.name == 'nt') or (platform.system() == 'Darwin'): # multiprocessing is tricky to use
                tests = []
                # loop over testsuite
                for project in testsuite:
                    test = Test(project)
                    test.rtol = rtol
                    test.atol = atol
                    run_simulation(test, qucspath, show_plot)
                    tests.append(test)
            else: # on Linux multiprocessing works fine
                nprocs = args.processes
                pool = multiprocessing.Pool(nprocs) # when np=None uses cpu_count() processes
                # prepare list of Test object to simulate
                alltests = [Test(project) for project in testsuite]
                # set default test tolerances
                for t in alltests:
                    t.rtol = rtol
                    t.atol = atol
                testsp = [pool.apply_async(run_simulation, args=(t, qucspath, show_plot,)) for t in alltests]
                pool.close() # this and the following line might not be needed...
                pool.join() # wait for all simulations to finish
                tests = [p.get() for p in testsp] # get results
            collect_tests.append(tests)

        print('\n')
        print(pb('############################################'))
        print(pb('#  Report simulation result comparison     #'))

        for indx, qucspath in enumerate (prefixes):
            print(pb('--> Simulator location: %s' %(qucspath)))

            for test in collect_tests[indx]:
                if test.status == "NUM_FAIL":
                    logger.warn(pr(' Numerical differences! Project [%s], traces %s' %(test.name, test.failed_traces)))
                    returnStatus = -1

                if "FAIL" in test.status:
                    logger.warn( 'Test: %s, Status: %s, Message %s' %(test.name, test.status, test.message))
                    returnStatus = -1


        if not returnStatus:
            print(pg('--> No significant numerical differences found.'))

        print(pb('#                                          #'))
        print(pb('############################################'))


        print(pg('************************'))
        print(pg('* Qucsator test report *'))
        print(pg('************************'))

        if args.compare:
            table_name = 'qucsator_comparison_' + timestamp() + '_sim_results.txt'
        else:
            import string
            table_name = 'report_simulation'+'_'+ get_qucsator_version(prefix).decode('iso-8859-1').replace(' ','_')+'.txt'

        if len (prefixes) > 1:
            footer  = 'Qucsator versions:   '
            for qp in prefixes:
                footer += get_qucsator_version(qp).decode('iso-8859-1') + ' : '
            footer += '\n\nBinary Locations:'
            for qp in prefixes:
                footer += '\n' + qp
            footer += '\n'
        else:
            footer  = 'Qucsator version:   '  + get_qucsator_version(qucspath).decode('iso-8859-1') + ' '

        footer += '\n'
        footer += 'Report produced on: ' + timestamp("%Y-%m-%d %H:%M:%S") + '\n'

        # Print simulation report to stdout and save to table_name
        report_status(collect_tests, table_name, footer)

        # Report tested/untested devices
        # data from simulation
        datafile = 'qucs_components_data.p'
        report_name = 'report_coverage_%s.txt' %(timestamp())
        report_coverage(collect_tests, datafile, report_name)

    #
    # Add schematic as test-project and initialize its netlist, result and log files
    #
    if args.add_test:
        if not args.prefix:
          sys.exit("Prefix required to add new test projects")

        sch = args.add_test
        if os.path.exists(sch):

            # copy stuff into place
            dest_dir = add_test_project(sch)

            # create reference netlist.txt
            input_sch  = os.path.join(dest_dir, sch)
            output_net = os.path.join(dest_dir,"netlist.txt")
            sch2net(input_sch, output_net, prefix)

            # create reference .dat, log.txt
            print(pb("Creating reference data and log files."))
            output_dataset = get_sch_dataset(input_sch)
            output_dataset = os.path.join(dest_dir, output_dataset)
            cmd = [os.path.join(prefix,"qucsator"), "-i", output_net, "-o", output_dataset]
            print('Running [qucsator]: ', ' '.join(cmd))

            # call the solver in a subprocess, set the timeout
            tic = time.time()
            command = Command(cmd)
            command.run(timeout=maxTime)
            toc = time.time()
            runtime = toc - tic

            # save log.txt
            # FIXME note that Qucs-gui adds a timestamp to the the log
            #       running Qucsator it does not the same header/footer
            logout = os.path.join(dest_dir,'log.txt')
            #print pb('Initializing %s saving: \n   %s' %(sch, logout))
            with open(logout, 'w') as myFile:
                myFile.write(command.out)

            ## ready to test-fire, run.py and check --qucs, --qucsator
            ## reminder to add to repository
            #sys.exit(0)
        else:
            sys.exit("File not found: %s" %sch)

    #
    # Reset the netlist, data and log files of test projects
    # acording version found on the given prefix.
    # FIXME this is similar to adding the test project again...
    # can we refactor the args.add_test?
    #
    if args.reset:
        for test in testsuite:
            dest_dir = os.path.join('testsuite', test)

            projName = test.strip(os.sep)
            # get schematic name from direcitory name
            # trim the simulation types
            sim_types= ['DC_', 'AC_', 'TR_', 'SP_', 'SW_']
            for sim in sim_types:
                if sim in projName:
                    projName=projName[3:]
            projName = projName[:-4]

            # do not reset netlist,
            # 0.0.17 has no command line interface, it launches...

            input_sch = os.path.join(dest_dir, projName+'.sch')
            output_dataset = get_sch_dataset(input_sch)
            output_dataset = os.path.join(dest_dir, output_dataset)

            output_net = os.path.join(dest_dir, 'netlist.txt')

            # OVERWRITE reference .dat, log.txt
            print(pb("Creating reference data and log files."))
            cmd = [os.path.join(prefix,"qucsator"), "-i", output_net, "-o", output_dataset]
            print('Running [qucsator]: ', ' '.join(cmd))

            tic = time.time()
            # call the solver in a subprocess, set the timeout
            command = Command(cmd)
            command.run(timeout=maxTime)
            toc = time.time()
            runtime = toc - tic

            # save log.txt
            # FIXME log reports different details if release/debug mode
            logout = os.path.join(dest_dir,'log.txt')
            #print pb('Initializing %s saving: \n   %s' %(sch, logout))
            with open(logout, 'wb') as myFile:
                myFile.write(command.out)

    # Print schematics contained in all (or selected) projects
    #
    if args.qprint:
        print('\n')
        print(py('********************************'))
        if (args.qprint == 'sch'):
           print('printing schematic(s): %s' %(testsuite))
        elif(args.qprint == 'dpl'):
           print('printing data display(s): %s' %(testsuite))
        else: # print all
           print('printing schematic(s) and data display(s): %s' %(testsuite))

        # prepare list of Print_test object to print

        test_dir = os.getcwd()
        prj_dir = os.path.join(test_dir, 'testsuite')
        collect_tests = []
        # loop over prefixes (possibly just one)
        for qucspath in prefixes:
            # prepare list of Test object to simulate
            allprint = [Print_test(project, prj_dir) for project in testsuite]
            if (args.qprint == 'sch'):
               for t in allprint:
                  t.add_all_files('sch')
            elif(args.qprint == 'dpl'):
               for t in allprint:
                  t.add_all_files('dpl')
            else: # print all
               for t in allprint:
                  t.add_all_files('sch')
                  t.add_all_files('dpl')
            if (os.name == 'nt') or (platform.system() == 'Darwin'): # multiprocessing is tricky to use
              results = []
              for ptest in allprint:
                 #ptest.debug()
                 print_project(ptest, qucspath, args.qprint)
                 results.append(ptest)
            else: # on Linux multiprocessing works fine
              nprocs = args.processes
              pool = multiprocessing.Pool(nprocs) # when np=None uses cpu_count() processes
              testsp = [pool.apply_async(print_project, args=(t, qucspath, args.qprint)) for t in allprint]
              pool.close() # this and the following line might not be needed...
              pool.join() # wait for all simulations to finish
              results = [p.get() for p in testsp] # get results

            collect_tests.append(results)


        print(pg('*****************************'))
        print(pg('* Qucs printing test report *'))
        print(pg('*****************************'))

        if args.compare:
            table_name = 'qucs_comparison_' + timestamp() + '_print_results.txt'
        else:
            table_name = 'report_printing'+'_'+ get_qucs_version(prefix).decode().replace(' ','_')+'.txt'

        if len (prefixes) > 1:
            footer  = 'Qucs versions:   '
            for qp in prefixes:
                footer += get_qucs_version(qp).decode() + ' : '
            footer += '\n\nBinary Locations:'
            for qp in prefixes:
                footer += '\n' + qp
            footer += '\n'
        else:
            footer  = 'Qucs version:   '  + get_qucs_version(qucspath).decode() + ' '

        footer += '\n'
        footer += 'Report produced on: ' + timestamp("%Y-%m-%d %H:%M:%S") + '\n'
        # Print simulation report to stdout and save to table_name
        report_print_status(collect_tests, table_name, footer)


    if returnStatus:
        status = 'FAIL'
    else:
        status = 'PASS'

    print('\n')
    print(pb('###############  Done. Return status: %s ###############' %status ))

    sys.exit(returnStatus)

# vim:ts=8:sw=4:et
