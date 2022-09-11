"""
qmat - a function to format arrays of arbitrary dimension for easy copy
and paste to Qucs equation system

Based on code used to paste Numpy into Matlab:
 - http://codepad.org/MVYCM0AJ
 - http://mail.scipy.org/pipermail/numpy-discussion/2009-May/042495.html

"""

import numpy as np
from numpy import ndarray, unravel_index, prod


def qmat(x, format='%.12f'):
    """Display the ndarray 'x' in a format suitable to use with Qucs equation system"""

    def print_row(row, format):
        s = ''
        for i in row:
            s += format % i
            s += ','
        s = s[:-1] #take back last comma
        return s

    # return string
    s =''
    if x.ndim == 1:
        # 1d input
        s += "["
        s += print_row(x, format)
        s += "]"

    if x.ndim == 2:
        s += "["
        s +=print_row(x[0], format)
        if x.shape[0] > 1:
            s += ';'
        for row in x[1:-1]:
            s += print_row(row, format)
            s += ';'
        if x.shape[0] > 1:
            s += print_row(x[-1], format)
        s += "]"

    # vector of matrices ?? not yet supported
    if x.ndim > 2:
        print("MATVEC? Issues with Qucs")
        d_to_loop = x.shape[2:]
        sls = [slice(None,None)]*2
        #s += "reshape([ " # matlab
        s += "["
        # loop over flat index
        for i in range(prod(d_to_loop)):
            # reverse order for matlab
            # tricky double reversal to get first index to vary fastest
            ind_tuple = unravel_index(i,d_to_loop[::-1])[::-1]
            ind = sls + list(ind_tuple)
            s += qmat(x[ind],format)

        s += ']'
        # matlab shape
        #s += '],['
        #for i in x.shape:
        #    s += '%d' % i
        #s += '])'

    return s


#print qmat(np.linspace(0,2,3))
#print qmat(np.eye(2))
#a=np.ones((2,2,2)) # 3d
#print qmat(a) # 3d
