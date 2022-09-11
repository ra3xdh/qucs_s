#!/usr/bin/env python


'''
This is a simple random constrained test generator for all functions in src/applications.h.
The purpose is is to test (and collect coverage of) the Qucs equation system and everything below it.

In short, it works as follows:
- For each entry in src/application.h.
- Generate random (or constrained) arguments (double, complex, matrix...)
- Create a test equation and computes the expected result.
- Save test equation into a netlist
- Run the netlist and read the simulator output
- Compare simulator output to the expected result.


TODO:
- At the moment it does not raise an error in Travis, only used for coverage.

'''

import argparse
import re
import os
import sys
import pickle
from collections import defaultdict

import numpy as np

# ease the use of eval, pull everything into the namespace
from numpy import *

# format Numpy to Qucs equation vector/matrix formats
from qmat import qmat

from random import randint

import subprocess

from qucstest.qucsdata import QucsData


def argTypeToMath(op, argc, argsTypes, prec='%.12f'):
    '''
    map argument types to numbers that can be computed

    input:
     - qucs operation
     - number of arguments
     - type of arguments
     - precision

    return:
     - qucs Equation content
     - expected numerical result

    TODO
    - some sort of random constrained number generation
    - how to handle speciall cases, such as
      branch cuts on hyperbolic functions...
    '''

    import collections

    # bypass random generation
    # FIXME these functions need further attention have
    constr = collections.defaultdict(dict)
    constr['arcsin']['TAG_DOUBLE']  = -1.0
    constr['arcsin']['TAG_COMPLEX'] = complex(3,4)
    constr['arcsin']['TAG_VECTOR']  = -1 * np.ones(3)
    constr['arccos']['TAG_DOUBLE']  = -1.0
    constr['arccos']['TAG_COMPLEX'] = complex(3,4)
    constr['arccos']['TAG_VECTOR']  = -1 * np.ones(3)
    constr['tanh']['TAG_DOUBLE']  = -1.0
    constr['tanh']['TAG_COMPLEX'] = complex(3,4)
    constr['tanh']['TAG_VECTOR']  = -1 * np.ones(3)
    constr['artanh']['TAG_DOUBLE']  = 0
    constr['artanh']['TAG_COMPLEX'] = complex(3,4)
    constr['artanh']['TAG_VECTOR']  = np.zeros(3)

    # Create list of arguments based on possible argument types
    Eqn = []
    pyEqn = []

    for arg in argsTypes:
        if   arg == 'TAG_DOUBLE':

            # value as string
            #value = 0.3
            value = np.random.rand()+1

            # inject constraint
            if op in list(constr.keys()):
                value = constr[op]['TAG_DOUBLE']

            Eqn.append(repr( float(prec % value) ));
            pyEqn.append(repr(value));

        elif arg == 'TAG_COMPLEX':

            # value as string
            #value = 1.2+2.1j
            a = np.random.rand()
            b = np.random.rand()
            value = complex(a,b)

            # inject constraint
            if op in list(constr.keys()):
                value = constr[op][arg]

            formatComplex = '{0:.%s} {1} {2:.%s}j' %(prec[2:], prec[2:])

            sign = '+' if value.imag >= 0 else '-'
            valueC = formatComplex.format(value.real, sign , abs(value.imag))

            Eqn.append( '(%s)' %valueC.replace('j','*j') )
            pyEqn.append(repr(value));

        elif arg == 'TAG_VECTOR':

            value = np.linspace(1.,2.,3)
            # inject constraint
            if op in list(constr.keys()):
                value = constr[op][arg]

            # format qucs equation string
            qucsString = qmat(value, format=prec)

            Eqn.append( qucsString );
            pyEqn.append(repr(value));

        elif arg == 'TAG_MATRIX':

            #pyValue = np.ones((2,2))
            value = np.random.rand(2,2) #random matrix

            # format qucs equation string
            qucsString = qmat(value, format=prec)

            Eqn.append( qucsString );
            pyEqn.append(repr(value));

        elif arg == 'TAG_UNKNOWN':
            # no argument?
            value = ''
            Eqn.append(repr(value));
            pyEqn.append(repr(value));

        elif arg == 'TAG_BOOLEAN':

            # 0 or 1
            value = random.randint(0,2)
            Eqn.append(repr(value));
            pyEqn.append(repr(value));

    # TAG_RANGE, an slice of a vector?

    # map qucs operator to python operators
    # to compute the expected result

    #print pyEqn

    opMap={}
    opMap['avg'] = 'average'
    opMap['length'] = 'len'
    opMap['random'] = 'random.rand'


    opMap['arsinh'] = 'arcsinh'
    opMap['arcosh'] = 'arccosh'
    opMap['artanh'] = 'arctanh'

    opMap['>'] = 'greater'
    opMap['<'] = 'less'
    opMap['>='] = 'greater_equal'
    opMap['<='] = 'less_equal'

    opMap['arg']   = 'angle' # Qucs alias
    opMap['angle'] = 'angle'

    opMap['mag'] = 'abs'

    # local helper to set angle in degree
    def angleDeg(arg):
        return np.angle(arg, deg=True)

    def ceilHelp(arg):
        if (type(arg) == complex) :
            return complex(ceil(real(arg)), ceil(imag(arg)))
        else:
            return ceil(arg)

    def fixHelp(arg):
        if (type(arg) == complex) :
            return complex(fix(real(arg)), fix(imag(arg)))
        else:
            return fix(arg)

    def hypotHelp(x1, x2):
        if (type(x1) == complex) :
            x1 = abs(x1)
        if (type(x2) == complex) :
            x2 = abs(x2)
        return hypot(x1, x2)

    def dB_Help(x):
        if type(x) == float:
            return 10.0*log10(abs(x))
        else:
            return 10.0*log10(abs(x)**2)

    def sqr_Help(x):
        if type(x) == ndarray and x.ndim > 1:
            return linalg.matrix_power(a,2)
        else:
            return square(x)

    def mult_Help(x):
        if type(a) == ndarray and a.ndim > 1:
            if type(b) == ndarray and b.ndim > 1:
                return dot(a, b)
        else:
            return a*b

    def norm_Help(x):
        return linalg.norm(x)**2

    def sinc_Help(x):
        return sinc(x/pi)

    def modulus_Help(x,y):
        if type(x) == complex or type(y) == complex:
            return x - y * floor (x / y);
        else:
            return x%y

    def hat_Help(x,y):
        if type(x) == ndarray and x.ndim >= 1:
            return linalg.matrix_power(x,y)
        else:
            return x**y


    opMap['phase'] = 'angleDeg'
    opMap['ceil'] = 'ceilHelp'
    opMap['fix'] = 'fixHelp'
    opMap['hypot'] = 'hypotHelp'
    opMap['dB'] = 'dB_Help'
    opMap['sqr'] = 'sqr_help'
    opMap['*'] = 'mult_help'
    opMap['norm'] ='norm_help'
    opMap['sinc'] ='sinc_help'
    opMap['%'] ='modulus_help'
    opMap['^'] ='hat_help'

    # check for operator mapping
    pyOp = op
    if op in opMap:
        pyOp = opMap[op]

    #
    # Build expression for Qucs
    # Build expression for Python
    #

    # No argument ex. random()
    if argc == 0:
        expression = 'check="%s()"' %(op)
        pyCode = '%s()' %(pyOp)

    # Unary operation, ie op(arg)
    # ex. +y , real(value)
    if argc == 1:

        expression = 'check="%s(%s)"' %(op, Eqn[0])
        pyCode = '%s(%s)' %(pyOp, pyEqn[0])
        #print op
        #print pyCode

    # Binary operation ex. 1+1
    # or function(arg, arg)
    # Assumptions for functions:
    # - name is longer than 1 character
    # - one (see above) or more arguments

    # what about '>', need to map to np.greater( a , b)
    if argc == 2:
        if len(op) == 1 : #unary
            expression = 'check="(%s)"' %(op.join(Eqn))
            pyCode = '%s' %(pyOp.join(pyEqn))

        else: #function
            expression = 'check="%s(%s)"' %(op, ','.join(Eqn))
            pyCode = '%s(%s)' %(pyOp, ','.join(pyEqn))

        # handle special case
        # a > b --> greater(a,b)
        # a < b --> less(a,b)

        # if complex:
        # a > b --> greater(abs(a), abs(b))
        # a < b --> less(abs(a), abs(b))
        if op == '>' or op == '<' or op == '>=' or op == '<=':
            expression = 'check="(%s)"' %(op.join(Eqn))
            pyCode = '%s(%s)' %(pyOp, ','.join(['abs(%s)' %arg for arg in pyEqn]))

        if op == '==' or op == '!=' :
            expression = 'check="(%s)"' %(op.join(Eqn))
            pyCode = '%s' %(pyOp.join(pyEqn))

    # ?: ifthenelse
    # a?b:c
    # BUG FIXME
    if argc == 3:

         expression = 'check="%s?%s:%s"' %(Eqn[0], Eqn[1], Eqn[2])
         pyCode = '%s if %s else %s' %(Eqn[1], Eqn[0], Eqn[2])


    #print pyCode

    # Try compute the result with Python
    pyResult = 0
    try :
        pyResult = eval(pyCode)
    except :
        pyResult = "  >> MISSING Python operation: %s %s" %(op, argsTypes)

    return expression, pyCode, pyResult



def testNetlist(qucsExpression):
    '''
    returns a netlist including the test expression.
    '''
    #net  = '# Qucs 0.0.19  Generated by Qucs-Test scripts.\n'
    net = '.DC:DC1 Temp="26.85" reltol="0.001" abstol="1 pA" vntol="1 uV"' \
          'saveOPs="no" MaxIter="150" saveAll="no" convHelper="none" Solver="CroutLU"\n'
    net += 'Eqn:Eqn1 %s Export="yes"\n' %qucsExpression
    return net




#=============================

#
# Operations missing Python verification
# Skip them for now
#
skip =[
  'arctan',  # can have one or two arguments?
  'length',  # don't know how to handle matvec input
  'array',   # ??
  '?:',      # ifthenesle, needs work
  'random',  # no reaso to check, only run
  'srandom', # same as above
  'unwrap',
  'coth',
  'sech',
  'Rollet',
  'Mu',
  'Mu2',
  'StabFactor',
  'StabMeasure',
  'linspace',
  'logspace',
  'NoiseCircle',
  'NoiseCircle',
  'NoiseCircle',
  'NoiseCircle',
  'NoiseCircle',
  'NoiseCircle',
  'StabCircleL',
  'StabCircleS',
  'GaCircle',
  'GpCircle',
  'GpCircle',
  'GpCircle',
  'GpCircle',
  'PlotVs',
  'PlotVs',
  'PlotVs',
  'interpolate',
  'receiver',
  'diff',
  'stos',
  'stoy',
  'stoz',
  'ytos',
  'ztos',
  'ytoz',
  'ztoy',
  'ztor',
  'rtoz',
  'ytor',
  'rtoy',
  'rtoswr',
  'twoport', #twoport(X, from, to) Y, Z, H, G, A, S, T
  'det',
  '!',
  '!=',
  '==',     # partially working, issue with check2:  check="[1.0,1.5,2.0]==1.5" check2="1.5==[1.0,1.5,2.0]"
  'vector',
  'matrix',
  'assert',
  'bugon',
  'floor',
  'limexp',
  'ln',
  'cot',
  'arccot',
  'sec',
  'arcsec',
  'cosec',
  'arccosec',
  'arcoth',
  'sech,',
  'arsech',
  'cosech',
  'arcosech',
  'dbm',
  'integrate',
  'dbw2w',
  'w2dbm',
  'dbm2w',
  'w2dbm',
  'polar',
  'bessely',
  'besseli0',
  'besselj',
  'stddev',
  'variance',
  'cumavg',
  'rms',
  'round',
  'erf',
  'erfc',
  'erfinv',
  'erfcinv',
  'runavg',
  'vt',
  'kbd',
  'step',
  'fft',
  'ifft',
  'dft',
  'idft',
  'Time2Freq',
  'Freq2Time',
  'fftshift',
  'xvalue',
  'yvalue',
  'adjoint',
  'signum',
  'inverse',

]

# implemented test support by return type
implemented = [
 'TAG_BOOLEAN',
 'TAG_DOUBLE',
 'TAG_COMPLEX',
 'TAG_VECTOR',
 #'TAG_MATRIX',  # parser hacked to support it, need work
 #'TAG_MATVEC', # don't know how to handle this
 #'TAG_CHAR',  # see 'array'
 #'TAG_STRING',  # see '+'
 #
]

# FIXME
# RANGE?
# [ PASS ]  TEST 335  [ avg ]  ['TAG_VECTOR', 'TAG_RANGE']

# =============================================================================

# Let us do some work...

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Qucs testing script.')

    parser.add_argument('--prefix', type=str,
                       help='prefix of installed Qucs (default: /usr/local/bin/)')

    parser.add_argument('--operation', type=str,
                       help='test one particular operation, ex. \'+\')')
    args = parser.parse_args()
    #print args

    # prefix = ''

    if args.prefix:
        if os.path.isfile(os.path.join(args.prefix, 'qucsator')):
            prefix = args.prefix
            print('\nFound Qucsator in: %s\n' %(prefix))
        else:
            sys.exit('Oh dear, Qucsator not found in: %s' %(prefix))
    else:
       sys.exit('\n Error, missing --prefix')


    # Get list of Qucs operations and their details

    # Picke/unpickle data application.h data
    # operations, applications = parseApplications("/Users/guitorri/git/qucs/qucs-core/src/applications.h")
    with open( 'qucs_operations.p', "rb" ) as f1:
        operations   = pickle.load( f1 )
    with open( 'qucs_applications.p', "rb" ) as f2:
        applications = pickle.load( f2 )


    if args.operation in operations:
        if args.operation in operations:
            operations = [args.operation]
        else:
            sys.exit('Operation not supported: %s' %args.operation)



    # Well, these are counters
    testCount =0
    skipCount = 0
    passCount = 0
    failCount = 0

    # Toggle if any test fail
    returnStatus = 0

    # Process every operation
    for op in operations:

        # cound skipped operations
        if op in skip:
            skipped = len(applications[op])
            print(' >> SKIP [%s] %s tests' %(op, skipped))
            skipCount +=  skipped
            continue

        for app in applications[op]:

            testCount +=1

            # unpack data
            retType = app[0]
            func = app[1]
            argc = int(app[2])
            argTypes = app[3]

            # skip if test not yet supported
            if retType not in implemented:
                #print ' >> MISSING [ %s ] return type %s' %(op, retType)
                skipCount +=  1
                continue

            # get Quscs string and Python result
            try:
                expression, pyExpression, pyResult = argTypeToMath(op, argc, argTypes, prec='%.12f')
            except:
                print(' >> ISSUES with %s' %op)
                print(expression, pyResult)

            # if pyResult is a string, report and move to next
            if isinstance(pyResult, str):
                #print ' >> PYTHON ERROR    [ %s ] ==> [%s] = %s | PyCode  >> %s' %(op, retType, argTypes, pyExpression)
                print(' >> PYTHON ERROR    [ %s ] ==> [%s] = %s' %(op, retType, argTypes))
                skipCount +=  1
                continue

            # save netlist    /temp/qucstest/test###.txt
            # run, save  data /temp/qucstest/test###.dat, handle crash
            # read dat
            # compare with expected
            # PASS, FAILL
            tmpDir = '/tmp/qucstest/'
            inNet  = tmpDir + 'test%03i.txt' %testCount
            outDat = tmpDir + 'test%03i.dat' %testCount

            if not os.path.exists(tmpDir):
                os.makedirs(tmpDir)

            net = testNetlist(expression)
            with open(inNet, 'w') as myTest:
                myTest.write( net )

            cmd = [os.path.join(prefix, "qucsator"), "-i", inNet, "-o", outDat]
            print('Running [qucsator]: ', ' '.join(cmd))
            retval = subprocess.call(cmd, stdout=open(os.devnull, "w"), stderr=subprocess.STDOUT)

            sim = 'PASS'
            if retval:
                sim = 'FAIL'
                print("\n +++ Qucsator ERROR +++ \n")
                print('Return code: ', retval)
                print('netlist: ', inNet)
                sys.exit('Error on qucsator.')


            # Qucsator data parser (hacked to support matrix)
            #import parse_result as parse
            if os.path.exists(outDat):
               test_data = QucsData(outDat)
            else:
               print(" \n\n +++  no result +++ \n\n:", outDat)

            test =  test_data.data['check']


            stat = ''
            if np.allclose(pyResult, test, rtol=1e-5, atol=1e-8):
                stat = 'PASS'
                passCount +=1
            else:
                stat = ' >FAIL'

            print("[ %s ]  TEST %03i  [ %s ] ==> [%s] = %s" %(stat, testCount, op, retType, argTypes))

            if stat != 'PASS' or sim =='FAIL':

                # fail due do numerical error or qucsator fail
                returnStatus -1

                failCount +=1
                print('---')
                print(net)
                print("Expected:")
                print(pyExpression)
                print('   ', pyResult)
                print("Got:")
                with open(outDat, 'r') as myDat:
                    for line in myDat:
                        print('   ', line, end=' ')
                print('---\n')

    total = 0
    for key in operations:
        total += len(applications[key])

    print('')
    print('-'*15)
    print('-- PASS  = ', passCount)
    print('-- FAIL  = ', failCount)
    print('-- SKIP  = ', skipCount)
    print('='*15)
    print('-- TOTAL = ', total)
    print('-'*15)

    if returnStatus:
        status = 'FAIL'
    else:
        status = 'PASS'

    print('\n')
    print(('###############  Done. Return status: %s ###############' %status ))

    sys.exit(returnStatus)

