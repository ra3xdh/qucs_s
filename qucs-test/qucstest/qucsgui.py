"""
Module to handle calls to qucs-gui.
"""

import os
import sys
import subprocess
from .colors import pb

def get_qucs_version(prefix):
    '''
    Run Qucs-GUI and return its version string.

    :param prefix: path to qucsator executable
    :return: the version tag of qucsator
    '''
    cmd = [os.path.join(prefix,"qucs"), "-v"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    p_out = p.stdout.readlines()
    # need to look for a line starting with 'Qucs'
    #   as there might be some GTK+ junk output before (?)
    #   like "Gtk-WARNING **: Locale not supported by C library."
    version = '(unknown)'
    for line in p_out:
        if line.decode().startswith('Qucs'):
            version = line.strip()
    return version


def sch2net (input_sch, output_net, prefix):
    '''
    Convert sch to netlist.
    Run Qucs-GUI, convert schematic into netlist.

    :param input_sch: path to input schematic
    :param output_net: path to generated netlist
    :param prefix: previx where qucs can be found
    '''
    print(pb("Converting schematic to netlist."))
    cmd = [os.path.join(prefix,"qucs"), "-n", "-i", input_sch, "-o", output_net]
    print('Running [qucs]: ', ' '.join(cmd))
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    retval = p.wait()
    # report issues if any
    if retval:
        print('Return code: ', retval)
        for line in p.stdout.readlines():
            print(line, end=' ')
        sys.exit('Error on sch2net.')
