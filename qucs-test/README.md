# Qucs testing scripts.


Objective: test the Qucs (GUI) and Qucsator (simulator).

The test projects under the `testsuite` directory are all run by default.

To skip failing projects use the `--exclude` option with a list of projects to be ignored.


## Options: `python3 run.py -h`

```
usage: run.py [-h] [--prefix PREFIX] [--qucs] [--qucsator]
              [-p [{sch,dpl,all}]] [--add-test FILE] [--exclude FILE]
              [--include FILE] [--project PROJECT] [--compare PATH [PATH ...]]
              [-v [LEVEL]] [--reset] [--timeout TIMEOUT] [--rtol RTOL]
              [--atol ATOL] [--plot-interactive] [-mp [NUM]]

Qucs testing script.

optional arguments:
  -h, --help            show this help message and exit
  --prefix PREFIX       prefix of installed Qucs (default: /usr/local/bin/)
  --qucs                run qucs tests
  --qucsator            run qucsator tests
  -p [{sch,dpl,all}], --print [{sch,dpl,all}]
                        run qucs and prints schematics and/or data displays to
                        file
  --add-test FILE       add schematic file to the testsuite
  --exclude FILE        file listing projects excluded from test
  --include FILE        file listing projects selected for test
  --project PROJECT     path to a test project
  --compare PATH [PATH ...]
                        two full paths to directories containing qucs or
                        qucsator binaries for comparison test
  -v [LEVEL], --verbose [LEVEL]
                        increase verbosity: 0 = only warnings, 1 = info, 2 =
                        debug. No number means info. Default is no verbosity.
  --reset               Reset (overwrite) data and log files of test
                        projects.Run qucsator given with --prefix.
  --timeout TIMEOUT     Abort test if longer that timeout (default: 90 s).
  --rtol RTOL           Set the element-wise relative tolerace (default 1e-1).
                        See: Numpy allclose function.
  --atol ATOL           Set the element-wise absolute tolerace (default 1e-5).
                        See: Numpy allclose function.
  --plot-interactive    Plot and show error graphs interactively. Hardcopy PNG
                        saved by default.
  -mp [NUM], --processes [NUM]
                        Use NUM processes to run the simulations (default:
                        number of CPU cores).
                        This option is disabled on Windows and macOS.
```

## Outputs

 * The script is rather verbose and the progress is printed to the terminal (with colors, yeah!).

 * A table with the run project, Qucs schematic version and simulation runtim is saved automatically and time-stamped.

 * A report table with all components available to Qucs and the coverage with respect to the types of simulations.

## Running the test suite

Example of running the test-suite for Qucsator while skipping a few test projects:

```
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --exclude skip.txt --qucsator
```

The test can be run using several processes in parallel, which will shorthen the overall simulation time if the machine has a multi-core processor, using the `-mp` option; this option not supported  on Windows and macOS.

```
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --exclude skip.txt --qucsator -mp
```

Two different `qucsator` versions can be compared with the command:

```
$ python3 run.py --compare /home/user/bin1/ /home/user/bin2/ --qucsator -mp
```

where `/home/user/bin1/` and `/home/user/bin2/` are the two directories containing the `qucsator` binaries to be tested.

## Printing the schematic to PDF files (devel)

See below examples of printing schematics to file (pdf):

 * printing all
 * printing all data display (`.dpl`) files only
 * skipping a list of projects
 * a single project from the testsuite

```
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --print
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --print dpl
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --exclude skip_print.txt -print
$python3 run.py --prefix /home/user/local/qucs-master/bin/ --print --project AC_SW_resonance_prj
```

As for the `qucsator` test, the `-mp` option can be added to run multiple (printing) processes in parallel (this option not supported on Windows and macOS).

The script will generate a table showing, for every file, the time needed to print the schematics to file or the `qucs` error code in case the file generation was not succesful.

Two different `qucs` versions can be used to generate the print files and the results for every binary will be shown in the report table, to allow for a quick comparison. Use a command like:

```
$ python3 run.py --compare /home/user/bin1/ /home/user/bin2/ --print -mp
```
where `/home/user/bin1/` and `/home/user/bin2/` are the two directories containing the `qucs` binaries to be tested.


## Running the Qucs equations tests

Qucs is featured with an equation system available in the schematic area.

The script below tests several of the built-in functions.

In short, it works as follows:

- For each entry in src/application.h.
- Generate random (or constrained) arguments (double, complex, matrix...)
- Create a test equation and computes the expected result (with Python and Numpy).
- Save test equation into a netlist
- Run the netlist and read the simulator output.
- Compare simulator output to the expected result.


### Options: `python run_equations.py -h`
````
usage: run_equations.py [-h] [--prefix PREFIX] [--operation OPERATION]

Qucs testing script.

optional arguments:
  -h, --help            show this help message and exit
  --prefix PREFIX       prefix of installed Qucs (default: /usr/local/bin/)
  --operation OPERATION
                        test one particular operation, ex. '+')
````

To run all the tests, use:

```
$ python run_equations.py --prefix /home/user/git/qucs-clone/
```


## Tested with

Centos 3.10.0-1062.4.1.el7.x86_64, Python 3.6.8, numpy 1.18.1

Mac OSX not tested yet

Windows not tested yet

You might need to install `argparse` which is not included with Python 2.6. Something like:

```
$sudo apt-get update
$sudo apt-get install python-setuptools
$sudo easy_install pip
$sudo pip install setuptools --no-use-wheel --upgrade
$sudo pip install argparse
```


## Testsuite results, Qucs 0.0.18

  * [List of Qucs components](qucs_components.txt)

  * [Testsuite simulation report](report_simulation_Qucsator_0.0.18.txt)

  * [Testsuite component coverage report](report_coverage_Qucsator_0.0.18.txt)

  * [Currently failing projects](skip.txt)


## Error Output Examples

  * [Output without errors](output_normal.txt)

  * [Output with errors](output_errors.txt)

---

_under construction_

## How it works

It considers every directory under `testsuite` as a project to be run. Then,

 * It run tests to check the schematic to netlist conversion, using the Qucs-GUI code.

 * It compares (diff) a reference netlist with the generated netlist on the step above.

 * A simulation is run for each project

 * Results are for the dependent variables are compared with reference results

 * It collects : failed netlist diffs, simulation runtime, failed variable comparison, output and warnings (not used currently).


## Running Qucs on the command line:

Running Qucs and Qucsator from the command line makes it easy to automated testing.

### Schematic to Netlist:

It is possible to use Qucs GUI from the command line to read a schematic and create the netlist.

```
$qucs -n -i file.sch -o netlist.txt
```

### Schematic to file (print):

The Qucs GUI can be used from the command line to read a schematic and crete a printout (feature in development).

```
$qucs -p -i file.sch -o netlist.pdf
```

### Netlist to Results:

Having a netlist file it is possible to run the Qucsator backed to read the netlist and create the results (and log messages).

```
$qucsator -i netlist.txt -o result.dat
```

## Notes:

 * Tests should not take too much time to run.
 * Tests should focus into a particular feature which can be tested in isolation
 * Tests could be categorised, ex. DC, AC analysis, a component...
 * Tests should exercise corners that are crucial for stable releases.
 * Before releasing, the test set should be run with no regressions.
 * Git can be used to version test result (timing history?)
 * Use `git diff -G '^([^#])'` to ignore the changes on the header line of the netlist files.


# Test-case

A test case is stored on a directory, similar to a Qucs project

Suggested contents:

 * test.txt
   * test config file
 * Schematic (.sch)
   * Test schematic (name convention)
   * Subcircuits dependencies
 * Netlist (.txt)
   * reference netlist
 * Result (.dat)
   * reference results
 * Optional
   * reference log file (.txt)
   * PDF dump of schematic/data display

_What about asco, verilog, vhdl?_

# Run a test-case

The test-case is run depending on the available inputs. The main steps are listed below

 * convert schematic into test netlist
 * compare reference netlist with test netlist
 * run netlist (or test netlist) store test result
 * compare reference result with test result

```
Schematic ==> Netlist

input:  [name].sch
output: netlist_test.txt

Netlist ==> Result

input:  netlist.txt | netlist_test.txt
output: result.dat | result_test.txt
        log.txt
        [asco.log]
```

```
compare:
  - netlist X netlist_test
  - result  X result_test
```

## Compare netlist

Simple diff, skip first line. Retun different lines.

## Check netlist

Qucsator has the option to check a netlist. ?? what for ??

## Compare results

It uses the numpy [`allclose`](http://docs.scipy.org/doc/numpy/reference/generated/numpy.allclose.html) function to check how close together are all the dependent variables (arrays) on the reference and test data files.

# Test Suite:

Tests should be similar to projects. So making it easy to open on Qucs an use as examples.

## Set structure:

```
qucs-test/
  run.py

  testsuite
    TR_collpits_prj/
      collpits.sch


    AC_bandpass_prj/
      bandpass.sch
      opa.sch
      bandpass.dat
      netlist.txt
      log.txt

    ASCO_AC_fobar_prj/

    VERILOG_

    VHDL_
```

Notes:

 * Need to identify the test schematic among the subcircuits
   * directory name, and the name of the schematic to be netlisted/simulated.

Whish-list

 * what about verilog, vhdl, asco?
 * pack/unpack tarballs/examples?


# Helper Scripts

* Create/update list of test/prj:
  - check consistency of the test-suite.
  - it looks what tests are available.
  - it adds new tests as they are found.
  - it preserves the ones that are commented out #, give warning


* Test behavior depends on the provided input/reference files
  - if [sch] provided -> netlist, check
  - if [sch, dat] provided -> nelist, [check], simulate, check
  - if [sch, dat, log] provided -> do netlist, [check], simulate, check, statistics

* Give option to create reference files when missing.

* Table Report pass, fail, missing resource
* Catch errors, time out, kill process if taking more than expected
* Report checks, time improvement/regression

* Html report?
