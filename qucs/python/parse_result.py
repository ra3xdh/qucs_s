import re
import numpy as np


def parse_file(name):

    file = open(name)

    # the dict this function returns
    data = {}
    numpoints = 1
    ind = 0
    shape = []
    variables = {}

    for line in file:
        #print line
        if line.startswith('<'):
            if line.startswith('<indep'):
                #print line
                r = re.match(r'\<(\w+) (\S+) (\d+)\>', line)
                g = r.groups()
                # there can be several independent variables -> numpoints keeps
                # the total number of points
                numpoints = numpoints * int(g[2])
                name = g[1]
                # reserve an array for the values
                data[name] = np.zeros(int(g[2]))
                ind = 0
                # save the simulation points in an array
                shape = np.append(shape, int(g[2]))
                # save that this variable is independent
                variables[name] = 'indep'

            if line.startswith('<dep'):
                #print line
                r = re.match(r'\<dep (\S+)', line)
                g = r.groups()
                name = g[0]
                # reserve a complex matrix to be on the safe side
                data[name] = np.zeros(int(numpoints), np.complex128)
                ind = 0
                # store that this is a dependent variable
                variables[name] = 'dep'

        else:
            jind = line.find('j')

            if(jind == -1):
                # real number -> just parse it
                val = float(line)
            else:
                # complex number -> break into re/im part
                val_re = line[0:jind-1]
                sign = line[jind-1]
                val_im = sign + line[jind+1:-1]
                # and convert it into a complex number
                val = complex(float(val_re), float(val_im))

            # store the extracted datapoint
            data[name][ind] = val
            ind = ind + 1


    data['variables'] = variables
    # reverse the shape variable in order to get the reshape operation (see below)
    # correct
    shape = shape[::-1]

    # here comes the clever trick :-)
    # if a dependent variable depends on N > 1 (independent) variables,
    # we reshape the vector we have obtained so far into an N-dimensional
    # matrix
    for key in data['variables']:
        temp = data['variables'][key]
        if temp == 'dep':
            temp_data = data[key]
            data[key] = np.reshape(temp_data, shape)

    return data
