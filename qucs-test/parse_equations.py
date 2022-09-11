
'''
This script is used to gather information about the Qucs equation systems.
It reads the source code (srs/applications.h) to extract what
operations are supported in different applications (combinations of inputs ouputs).
'''

import re
import sys
import os
from collections import defaultdict
import pickle


def parseApplications(applicationsFile):
    '''
    parse struct in 'qucs-core/src/applications.h'
    search for entries like { "=", TAG_ ... },
    '''

    try:
        with open (applicationsFile) as myfile:
            data=myfile.read()
    except OSError:
        print('File not found: ', applicationsFile)

    parse = re.compile("  \{[^\}]*\}").findall(data);

    # keep index list for operations
    operations = []

    # keep dict by operations containing the list:
    #  - [operation return type, qucs function, number of arguments, [argument type]]

    applications = defaultdict(list)

    # format parsed data, skip NULL entry
    for item in parse[:-1]:
        # drop braces
        item =  item.replace('{','').replace('}','').split(',')

        # cleanup
        item = [it.strip() for it in item]

        # unpack
        op, retType, func, args = item[:4]
        argTypes = item[4:]

        # trim extra "
        op = op[1:-1]

        # store
        if op not in operations: operations.append(op)
        applications[op].append([ retType, func, args, argTypes ])
    return operations, applications


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print('\nPlease provide full path to Qucs repo clone:')
        print('$ python parse_equations.py /path/to/src/qucs')
        sys.exit('Not enough arguments!')

    repo = str(sys.argv[1])

    print('')
    print('Looking for repository: \t %s' %(repo))
    if not os.path.isdir(repo):
        sys.exit("Directory not found: <repo>"+repo)

    # Scan code for available equations/applications
    operations, applications = parseApplications(repo+"qucs-core/src/applications.h")

    # Save list of operations
    opOut = 'qucs_operations.p'
    print('Saving data (as Pickle): \t', opOut)
    with open( opOut, "wb" ) as handle:
        pickle.dump( applications, handle )

    # Save dictionary of applications into a pickle file.
    appOut = 'qucs_applications.p'
    print('Saving data (as Pickle): \t', appOut)
    with open( appOut, "wb" ) as handle:
        pickle.dump( applications, handle )
