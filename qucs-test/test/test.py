
import sys
sys.path.append("..")

from qucstest.qucsdata import QucsData

data = "single_balanced.dat"
d = QucsData(data)
print(d)

