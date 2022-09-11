"""
Module to handle a Qucsator netlist file.
"""

import difflib


def get_net_components(netlist):
    '''
    Search for component types on a netlist file.

    :param netlist: text file containing a netlist
    :return: list of components
    '''
    comps=set()
    with open(netlist) as fp:
        for line in fp:
            if ':' in line:
                # left of :
                element = line.split(':')[0].strip()
                if not '.' in element:
                    comps.add(element)
    return list(comps)


def get_net_simulations(netlist):
    '''
    Search for simulations types on a netlist file.

    :param netlist: text file containing a netlist
    :return: list of simulations
    '''
    sim=set()
    with open(netlist) as fp:
        for line in fp:
            if ':' in line:
                # left of :
                element = line.split(':')[0].strip()
                if '.' in element:
                    # skip subcircuit '.Def' marker
                    if not 'Def' in element:
                        sim.add(element.strip('.'))
    return list(sim)


def check_netlist(ref_netlist, output_netlist, skip=1, verbose=0):
    '''
    Does a diff between a reference netlist and a test netlist.

    :param ref_netlist: text file used as reference netlist
    :param output_netlist: text file used on test
    :param skip: skip lines
    :param verbose: print the difference
    :return: diff equal (True|False), list of lines with mismatches
    '''
    # http://docs.python.org/2.7/library/difflib.html
    net_equal = True
    bad_lines = []
    with open(ref_netlist) as f:
        flines = f.readlines()
    with open(output_netlist) as g:
        glines = g.readlines()

    d = difflib.Differ()

    # skip header lines?
    diff = list(d.compare(flines[skip:], glines[skip:]))

    # show complete diff?
    if verbose:
        print(' '.join(diff))

    for line in diff:
        # look in diff for code [+ - ?]: unique to f, unique to g or not present in f or g
        chars = set('+-?')
        if any((c in chars for c in line[:1])):
            net_equal = False
            #print line
            bad_lines.append(line)
    return net_equal, bad_lines
