import re
import numpy as np


class QucsDataset:
    def __init__(self, name: str) -> None:
        '''
        A class for parsing and working with QUCS-S simulation results.

        Methods:
            results - returns simulation result for given simulation variable
            variables - prints the list of variables and their types

        Args:
            name (str): Path to the QUCS-S dataset file

        Raises:
            FileNotFoundError: If the specified dataset file is not found
            ValueError: If data does not contain the expected variables
            ValueError: If QUCS-S dataset is invalid

        Attributes:
            _variables (Dict[str, str]):
                A dictionary mapping variable names to either 'indep' or 'dep'.
            __data (Dict[str, np.ndarray]):
                A dictionary mapping variable names to arrays of values.
        '''
        try:
            with open(name, 'r') as f:
                first_line = f.readline()
                if not first_line.startswith("<Qucs Dataset"):
                    raise ValueError(f"Invalid QUCS-S dataset {name}.")
                self.__qucs_dataset = f.readlines()
        except FileNotFoundError:
            raise FileNotFoundError(f"QUCS-S dataset {name} not found.")
        self._variables = {}
        self.__data = self.__parse_qucs_result()

    def __parse_qucs_result(self) -> dict:
        '''
        Parses a *.dat file containing QUCS-S simulation results.

        Returns:
            dict: A dictionary of the variables in the dataset and their values
        '''
        data = {}
        numpoints = 1
        indep_count = 0
        shape = []
        ind = 0

        for line in self.__qucs_dataset:
            if line.startswith('<'):
                if line.startswith('<indep'):
                    matched = re.search(r'<(\w+) (\S+) (\d+)>', line)
                    name = matched.group(2)

                    # work with several independent variables
                    if indep_count >= 1:
                        # keeps the total number of points
                        numpoints = numpoints * int(matched.group(3))
                        shape = int(matched.group(3))
                    else:
                        # only parse the total number of points
                        numpoints = int(matched.group(3))
                        shape = 1

                    # reserve an array for the values
                    data[name] = np.zeros(numpoints)
                    ind = 0

                    # save that this variable is independent
                    self._variables[name] = 'indep'
                    indep_count += 1
                elif line.startswith('<dep'):
                    indep_count = 0
                    matched = re.search(r'<dep (\S+)', line)
                    name = matched.group(1)
                    # reserve a complex matrix to be on the safe side
                    data[name] = np.zeros(numpoints, dtype=np.complex128)
                    ind = 0
                    # store that this variable is dependent
                    self._variables[name] = 'dep'
            else:
                jind = line.find('j')
                if jind == -1:
                    val = float(line)
                else:
                    # complex number -> break into re/im part
                    val_re = line[0:jind-1]
                    sign = line[jind-1]
                    val_im = sign + line[jind+1:-1]
                    # complex number -> break into re/im part
                    val = complex(float(val_re), float(val_im))

                # store the extracted datapoint
                data[name][ind] = val
                ind += 1

        # here comes the clever trick :-)
        # if a dependent variable depends on N > 1 (independent) variables,
        # we reshape the vector we have obtained so far into an N-dimensional
        # matrix
        for key in self._variables:
            if self._variables[key] == 'dep' and shape != 1:
                data[key] = data[key].reshape(shape, int(data[key].size/shape))
                print(f'Simulation results for variable {key} reshaped into an N-dimensional matrix')
        return data

    def results(self, variable_name: str) -> np.ndarray:
        '''
        Returns simulation results for given variable name.

        Args:
            variable_name (str): Name of the simulation variable

        Returns:
            np.ndarray: Array of simulation results for the specified
                variable name
        '''
        if variable_name not in self._variables:
            raise ValueError("Data does not contain the expected variables.")
        return self.__data[variable_name]

    def variables(self) -> None:
        '''
        Prints the variables in the dataset and their types to the console.
        '''
        print('Variables: ')
        for name, vtype in self._variables.items():
            print(f"  {name}: {vtype}")
