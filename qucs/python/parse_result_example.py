import numpy as np
import matplotlib.pyplot as plt
import warnings
from parse_result import QucsDataset

warnings.simplefilter("ignore", np.ComplexWarning)


# create the dat file to load with:
# qucsator < rc_ac_sweep.net > rc_ac_sweep.dat

data = QucsDataset('rc_ac_sweep.dat')
data.variables()

x = data.results('acfrequency')
y = np.abs(data.results('out.v'))
c = data.results('Cx')

plt.loglog(x, y[0, :], '-rx')
plt.loglog(x, y[1, :], '-b.')
plt.loglog(x, y[4, :], '-go')

plt.legend(['Cx=' + str(c[0]), 'Cx=' + str(c[1]), 'Cx=' + str(c[4])])

plt.xlabel('acfrequency')
plt.ylabel('abs(out.v)')
plt.grid()
plt.show()
