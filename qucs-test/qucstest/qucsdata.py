"""
Module to handle a Qucs Dataset result file.
"""

import re
import numpy as np
import pprint as pp

class QucsData:
    """
    Read an store a result file writen by Qucsator.
    """
    def __init__(self, filename):
        # source dataset
        self.filename = filename
        # qucsator version that generated the data
        self.version = ''
        # list of all variable names
        self.names = []
        # data[name] = array
        self.data = {}
        # data dependent[name]=list(names)
        self.dependent = {}

        # parse
        self.read(self.filename)

    def __str__(self):
        """
        Print self.
        """
        pr  = ''
        pr += 'File name: %s\n' %self.filename
        pr += 'names:\n'
        for name in self.names:
            pr += '    \'%s\': shape %s\n' %(name, self.data[name].shape)
        pr += 'dependent: \n'
        pr += pp.pformat(self.dependent, indent=4)
        pr += '\n'
        return pr

    def read(self,fileName):
        """
        Qucs Dataset parser.
        """
        variable = ''
        with open(fileName) as qucsdata:
            for line in qucsdata:
                # header
                r = re.match(r'\<Qucs Dataset (\d+\.\d+\.\d+)\>', line)
                if r:
                    self.version = r.group(1)

                if '<indep' in line:
                    # independent variables
                    r = re.match(r'(\S+) (\S+) (\d+)', line.translate({ord(c):'' for c in "<>"}))
                    kind, identifier, size =  r.groups()
                    # collect identifiers
                    self.names.append(identifier)
                    variable = identifier
                    continue # next line

                if '<dep' in line:
                    # dependent variables
                    # one or more independent, y=f(v,w,...)
                    r = re.findall('(\S+)', line.translate({ord(c):'' for c in "<>"}))
                    identifier = r[1]
                    self.dependent[identifier]= r[2:]
                    self.names.append(identifier)
                    variable = identifier
                    continue # next line

                if '</indep' in line:
                    # reached end of independent variable
                    # cast from list to numpy array
                    values = np.array(self.data[variable])
                    self.data[variable] = values
                    variable = ''
                    continue #  next line

                if '</dep' in line:
                    # reached end of dependent variable
                    # dependencies are stacked on a single column
                    # cast from list to 2D array
                    # TODO check for multidimensional ndim > 2
                    # <dep Variable axis1 axis2 axis3 ...>
                    shape = []
                    for dep in self.dependent[variable]:
                        size = len(self.data[dep])
                        shape.append (size)
                    values = np.array(self.data[variable])
                    self.data[variable] = values.reshape(tuple(shape),order='F')
                    variable = ''
                    continue #  next line

                if variable:
                    # collect data from line into temporary list
                    if variable not in list(self.data.keys()):
                        self.data[variable] = []
                    value = line.strip()
                    if ('j' in value):
                        # put imaginary to the end
                        value = value.replace("j","")
                        value = value+'j'
                        value = complex(value)
                    else:
                        value = float(value)
                    self.data[variable].append(value)




