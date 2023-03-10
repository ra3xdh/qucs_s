import numpy as np
import matplotlib.pyplot as plt
import warnings
from parse_result import QucsDataset

warnings.simplefilter("ignore", np.ComplexWarning)


# to create the dat.ngspice file to load with:
# load the file rc_tran_ac.sch into QUCS-S and run the simulation

data = QucsDataset('rc_tran_ac.dat.ngspice')
data.variables()

fig,  axs = plt.subplots(nrows=1, ncols=2, figsize=(15, 5))

axs[0].semilogx(data.results('frequency'), data.results('ac.v(vn3)'), '-ro')
axs[1].plot(data.results('time'), data.results('tran.v(vn3)'), '--b')
axs[0].set_xlabel('Time (s)')
axs[0].set_ylabel('Vn3 (V)')
axs[1].set_xlabel('frequency (Hz)')
axs[1].set_ylabel('Vn3 (V)')
axs[0].grid()
axs[1].grid()

plt.show()
