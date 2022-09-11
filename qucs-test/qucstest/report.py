"""
Module to handle reports.
"""

from .colors import *
import os
import pickle


def report_status(collection, savename='', footer=''):
    '''
    Print a table with test resuts. It can also write to file.

    :param collection: data collected during tests.
    :param savename: name used to save the table to a text file.
    :param footer: custom footer appended to the table.
    :return: None
    '''
    header = '%-30s | %-15s ' %('Project', 'Schem. Version')

    for testrun in collection:
        header += ' |   Sim. Runtime     '

    line = '-'*len(header)

    if savename:
        f = open(savename, 'w')
        f.write(line+'\n')
        f.write(header+'\n')
        f.write(line+'\n')

    print(line)
    print(header)
    print(line)

    names = []
    versions = []
    results = []

    # 1- name column
    # 2- version column
    for test in collection[0]:
        names.append(test.name)
        versions.append(test.version)

    results = []
    for run in collection:
        runtime = []
        status = []
        for test in run:
            runtime.append(test.runtime)
            status.append(test.status)
        results.append( (status, runtime) )

    # 3- concatenate name, version, [status]*
    for idx in range(len(names)):
        name = names[idx]
        version = versions[idx]
        proj_stat = '%-30s | %-15s  ' %(name, version)
        for status, runtime in results:
            stat = status[idx]
            time = runtime[idx]

            if 'NUM_FAIL' in stat:
                proj_stat += '| %-10s' %(str(time))
                proj_stat += '%s' %('NUM_FAIL  ')
            elif 'FAIL' in stat:
                proj_stat += '| %-20s' %(stat)
            else:
                proj_stat += '| %-20s' %(str(time))

        # report line
        print(proj_stat)

        if savename:
            f.write(proj_stat+'\n')


    if footer:
        print(line)
        f.write(line+'\n')
        print(footer)
        f.write(footer)

    if savename:
        print(line)
        f.write(line+'\n')
        f.close()
        print(pg("Saved simulation report: %s " %savename))


def report_coverage(collection, datafile, report_name=''):
    '''
    Report component coverage with given tests.

    :param collection: data collected while running testsuite
    :param datafile: file containing data from cpp source files
    :param report_name: name used to save the report
    :return: string containing the report
    '''

    # flatten for component the types of simulations covered
    # tested[component as key] = list of simulations
    tested = {}
    testrun = collection[0]
    for test in testrun:
        for comp in test.comp_types:
            if comp not in tested:
                tested[comp] = list()
            tested[comp] = list(set(tested[comp])|set(test.sim_types))

    # data from source files
    # Load the dictionary back from the pickle file.
    if not os.path.isfile(datafile):
        print(pr('Problem finding: %s' %datafile))
        print(pr('  Run "parse_models.py"'))
        return
    with open( datafile, "rb" ) as handle:
        data = pickle.load( handle )

    reg = [
    '  REGISTER_LUMPED',
    '  REGISTER_SOURCE',
    '  REGISTER_PROBE',
    '  REGISTER_TRANS',
    '  REGISTER_NONLINEAR',
    '  REGISTER_VERILOGA',
    '  REGISTER_DIGITAL',
    '  REGISTER_FILE',
    #'  REGISTER_SIMULATION'
    ]

    # list of simulations
    sims_avail = ['AC_Sim', 'DC_Sim', 'Digi_Sim', 'HB_Sim',
                  'Optimize_Sim', 'Param_Sweep', 'SP_Sim', 'TR_Sim']

    # get suffix type
    regs = [r.strip().split('_')[1] for r in reg]

    cov_report = ''
    line = '\n'+'-'*40
    cov_report += line
    cov_report += '\nReport on Qucs components test coverage'
    # sort by kind
    for reg_kind in regs:
        cov_report += line
        cov_report += '\n### Kind: %s' %(reg_kind)
        cov_report += line
        cov_report += '\n%-18s | %-10s ' %('Component', 'Simulations Covered')
        cov_report += line
        for key in sorted(data):
            model, name, base, kind = data[key]
            if  kind== reg_kind:

                # cross with test coverage
                if model in tested:
                    cov_report += '\n%-18s | %-10s ' %(key, ', '.join(tested[model]))
                else:
                    cov_report += '\n%-18s | %-10s ' %(key, ' --- ')
    cov_report += line
    cov_report += line
    cov_report += '\nAll Available Simulations:'
    cov_report += '\n'+', '.join(sims_avail)
    cov_report += line

    # save to file?
    if report_name:
        print(pg('Saved coverage report: %s' %(report_name) ))
        with open(report_name, 'w') as rep_file:
            rep_file.write(cov_report)

    return cov_report


def report_print_status(collection, savename='', footer=''):
    '''
    Print a table with printing test results. It can also write to file.

    :param collection: data collected during printing tests.
    :param savename: name used to save the table to a text file.
    :param footer: custom footer appended to the table.
    :return: None
    '''
    header = '%-30s | %-15s ' %('Project', 'Schem. Version')

    for testrun in collection:
        header += ' |   Print Runtime    '

    line = '-'*len(header)

    if savename:
        f = open(savename, 'w')
        f.write(line+'\n')
        f.write(header+'\n')
        f.write(line+'\n')

    print(line)
    print(header)
    print(line)

    names = []
    versions = []
    results = []

    for test_idx, test in enumerate(collection[0]): # project names
        if test.files: # project not empty
            proj_stat = '%-30s | %-15s  | %-20s' %(test.name, '', '')
            if (len(collection) > 1): # used --compare
                proj_stat += '| %-20s' %('')
            print(proj_stat)
            for sch_idx, sch in enumerate(test.files):
                proj_stat = '  %-28s | %-15s  ' %(sch.name, sch.version)
                if 'NUM_FAIL' in sch.status:
                    proj_stat += '| %-10s' %(str(sch.runtime))
                    proj_stat += '%s' %('NUM_FAIL  ')
                elif 'FAIL' in sch.status:
                    proj_stat += '| %-20s' %(sch.status)
                else:
                    proj_stat += '| %-20s' %(str(sch.runtime))
                if (len(collection) > 1): # used --compare
                    sch = collection[1][test_idx].files[sch_idx]
                    if 'NUM_FAIL' in sch.status:
                        proj_stat += '| %-10s' %(str(sch.runtime))
                        proj_stat += '%s' %('NUM_FAIL  ')
                    elif 'FAIL' in sch.status:
                        proj_stat += '| %-20s' %(sch.status)
                    else:
                        proj_stat += '| %-20s' %(str(sch.runtime))
                # report line
                print(proj_stat)

                if savename:
                    f.write(proj_stat+'\n')

    if footer:
        print(line)
        f.write(line+'\n')
        print(footer)
        f.write(footer)

    if savename:
        print(line)
        f.write(line+'\n')
        f.close()
        print(pg("Saved simulation report: %s " %savename))
