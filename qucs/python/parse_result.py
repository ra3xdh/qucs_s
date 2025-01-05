from dataclasses import dataclass, field
from pathlib import Path
from typing import Literal, TypeAlias
import re

import numpy as np


VariableType: TypeAlias = Literal["indep", "dep"]
ComplexArray: TypeAlias = np.typing.NDArray[np.complex128]
FloatArray: TypeAlias = np.typing.NDArray[np.float64]
DataDict: TypeAlias = dict[str, ComplexArray | FloatArray]
VariableDict: TypeAlias = dict[str, VariableType]


@dataclass
class QucsDataset:
    """A class for parsing and working with QUCS-S simulation results."""

    file_path: str | Path
    _variables: VariableDict = field(default_factory=dict, init=False)
    _data: DataDict = field(default_factory=dict, init=False)
    _qucs_dataset: list[str] = field(default_factory=list, init=False)

    def __post_init__(self) -> None:
        """Initialize the dataset by reading and parsing the file."""
        try:
            with open(self.file_path, "r", encoding="utf-8") as f:
                first_line = f.readline()
                if not first_line.startswith("<Qucs Dataset"):
                    raise ValueError(f"Invalid QUCS-S dataset {self.file_path}.")
                self._qucs_dataset = f.readlines()
        except FileNotFoundError:
            raise FileNotFoundError(f"QUCS-S dataset {self.file_path} not found.")

        self._parse_qucs_result()

    def _parse_qucs_result(self) -> None:
        """Parse a *.dat file containing QUCS-S simulation results."""
        numpoints: int = 1
        indep_count: int = 0
        shape: int = 1
        ind: int = 0
        current_var: str = ""

        for line in self._qucs_dataset:
            if line.startswith("<"):
                if line.startswith("<indep"):
                    matched = re.search(r"<(?P<tag>\w+) (?P<var>\S+) (?P<points>\d+)>", line)
                    if not matched:
                        continue

                    current_var = matched.group('var')
                    points = int(matched.group('points'))

                    if indep_count >= 1:
                        numpoints *= points
                        shape = points
                    else:
                        numpoints = points

                    self._data[current_var] = np.zeros(numpoints)
                    self._variables[current_var] = "indep"
                    indep_count += 1
                    ind = 0

                elif line.startswith("<dep"):
                    indep_count = 0
                    matched = re.search(r"<dep (?P<var>\S+)", line)
                    if not matched:
                        continue

                    current_var = matched.group('var')
                    self._data[current_var] = np.zeros(numpoints, dtype=np.complex128)
                    self._variables[current_var] = "dep"
                    ind = 0

            elif (
                line.strip() and current_var
            ):  # Проверяем что строка не пустая и переменная определена
                val = self._parse_value(line.strip())
                self._data[current_var][ind] = val
                ind += 1

        # Reshape dependent variables for N > 1 independent variables
        if shape > 1:
            for key, var_type in self._variables.items():
                if var_type == "dep":
                    rows = shape
                    cols = int(self._data[key].size / shape)
                    self._data[key] = self._data[key].reshape(rows, cols)
                    print(
                        f"Simulation results for variable {key} reshaped into an N-dimensional matrix"
                    )

    @staticmethod
    def _parse_value(line: str) -> complex | float:
        """Parse a string value into either a complex number or float."""
        jind = line.find("j")
        if jind == -1:
            return float(line)

        val_re = line[0:jind - 1]
        sign = line[jind - 1]
        val_im = sign + line[jind + 1:]
        return complex(float(val_re), float(val_im))

    def results(self, variable_name: str) -> ComplexArray | FloatArray:
        """Return simulation results for given variable name."""
        if variable_name not in self._variables:
            raise ValueError("Data does not contain the expected variables.")
        return self._data[variable_name]

    def variables(self) -> None:
        """Print the variables in the dataset and their types to the console."""
        print("Variables: ")
        for name, vtype in self._variables.items():
            print(f"  {name}: {vtype}")

