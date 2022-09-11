
'''
    Hacked parser to gather info about available components from Qucs C++ sources

    Note:
    * there a few name classes mismatches or inexistent beween qucs / qucs-core
      * Basic_BJT - ?
    * it skips some HDL based components

'''

import glob
import re
import os
import sys
import pickle


def get_class(s):
    # text between parenthesis
    return s[s.find("(")+1:s.find(")")]

def scan_repo(repo):
    '''
    Scan the repository for:
     - registered components
     - component object properties

    input:
     - repo -- path to repository

    return:
     - report -- formatted text table
     - data   -- dictionary with table data
                 data[registered_Name]=[model, name, source file, category]

    '''


    # Get class names and type of model
    # ex. Resistor - Lumped
    #     Diode    - Nonlinear

    mod = os.path.join(repo, 'qucs/qucs/module.cpp')
    print('Scan registred components: \t %s' %(mod))

    regs = [
    '  REGISTER_LUMPED',
    '  REGISTER_SOURCE',
    '  REGISTER_PROBE',
    '  REGISTER_TRANS',
    '  REGISTER_NONLINEAR',
    '  REGISTER_VERILOGA',
    '  REGISTER_DIGITAL',
    '  REGISTER_FILE',
    '  REGISTER_SIMULATION',
    '  REGISTER_EXTERNAL'
    ]
    # Skip '  REGISTER_PAINT'


    mod_reg = {}
    for index in range(len(regs)):
        kind = regs[index].strip().split('_')[1]
        with open(mod) as fm:
            for line in fm:
                if regs[index] in line:
                    model_class = get_class(line)

                    # handle Resistor with multiple parameters
                    if ',' in model_class:
                        model_class = model_class.split(',')[0]

                    mod_reg[model_class] = kind


    # Gather component `data` indo dictionary, saved as Picke later on.
    #  data[registered_Name]=[model, name, source file, category]
    data = {}

    comp_src = 'qucs/qucs/components/'
    comp_dir = os.path.join(repo, comp_src)

    print('Looking for components dir: \t %s%s' %(comp_dir, '*.cpp'))
    if not os.path.isdir(comp_dir):
        sys.exit("Directory not found: <components>"+comp_src)


    # list all component sources
    devices = glob.glob(comp_dir+'*.cpp')

    report  = ''
    report += '\n%20s' %('-'*80)
    report += '\n%-20s | %-20s | %-15s | %-10s' %('Registered Class', 'Model Name', 'Instance Prefix', 'Source')
    report += '\n%20s' %('-'*80)

    # Parse data in component cpp files
    for src in devices:
        constr = False
        nam = False
        verbose = False

        with open(src) as fp:

            # skip non-device files
            skip = ['component.cpp',
                    'componentdialog.cpp',
                    'optimizedialog.cpp',
                    'spicedialog.cpp',
                    'vafile.cpp',
                    'vacomponent.cpp',
                    'verilogfile.cpp',
                    'vhdlfile.cpp',
                    'vfile.cpp']

            # component cpp file
            base = os.path.basename(src)
            if base in skip:
                continue # skip to next src

            # skip moc files
            if 'moc.cpp' in base:
                continue # skip to next src

            if verbose:
                print('\n%s' %src)

            for line in fp:
                if not constr:
                    # look for constructor
                    if '::' in line:
                        constr = True
                        # class name
                        regName = line.split('::')[0].strip()

                        if verbose:
                            print('register Name:', regName)

                if '  Model' in line:
                    # get stuff between " "
                    model = re.findall('"([^"]*)"', line)[0]

                    if verbose:
                        print('model Name:   ', model)

                    # simple bjt has no name, but netlist adds (T) for it
                    if model=='_BJT':
                        nam = True
                        name = 'T'

                    # logic gates have no name, but netlist adds (Y) for them
                    if (model == "AND" or  model == "NAND" or model == "NOR" or
                        model == "OR" or  model == "XNOR" or model == "XOR"):
                        nam = True
                        name = 'Y'

                    # simple MOSFET has no name, but netlist adds (T) for it
                    if model=='_MOSFET':
                        nam = True
                        name = 'T'

                    if nam:
                        if verbose:
                            print('instance Name:', name)
                        continue

                if not nam:
                    if 'Name' in line:
                        if '=' in line:
                            nam = True
                            name = re.findall('"([^"]*)"', line)[0]
                            if verbose:
                                print('instance Name:', name)
                            continue

        report += '\n%-20s | %-20s | %-15s | %-10s' %(regName, model, name, base)


        # try to skip unknown classes from Qucs into Qucs-core
        # ignore unregistered
        if regName in mod_reg:
            data[regName]=[model, name, base, mod_reg[regName]]
        else:
            print('  Not registered class: ', regName)

    report += '\n%20s' %('-'*80)
    report += '\nTotal: %i' %(len(list(data.keys())))
    report += '\n%20s' %('-'*80)

    return report, data


if __name__ == "__main__":


    if len(sys.argv) != 2:
        print('\nPlease provide full path to Qucs repo clone:')
        print('$ python parse_mode.py /path/to/src/qucs')
        sys.exit('Not enough arguments!')

    repo = str(sys.argv[1])


    print('')
    print('Looking for repository: \t %s' %(repo))
    if not os.path.isdir(repo):
        sys.exit("Directory not found: <repo>"+repo)

    # Scan code for registered components and their properties
    report, data = scan_repo(repo)

    # Save table
    report_out = 'qucs_components.txt'
    print('Saving components table: \t', report_out)
    with open(report_out, 'w') as myFile:
        myFile.write(report)

    # Save data, to be used on the test reports
    # Save a dictionary into a pickle file.
    datafile = 'qucs_components_data.p'
    print('Saving data (as Pickle): \t', datafile)
    with open( datafile, "wb" ) as handle:
        pickle.dump( data, handle )

    # parse type from registered



