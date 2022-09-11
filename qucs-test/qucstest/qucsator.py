"""
Module to handle calls to qucsator.
"""

import os
import subprocess


def get_qucsator_version(prefix):
    '''
    Run Qucsator and return its version string.

    :param prefix: path to qucsator executable
    :return: the version tag of qucsator
    '''
    ext = '' if os.name != 'nt' else '.exe'
    cmd = [os.path.join(prefix,"qucsator"+ext), "-v"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    version = p.stdout.readlines()[0].strip()
    return version


def get_registed_models(prefix):
    '''
    Query qucsator for defined components. Option -l (debug mode only?).
    Definitions also listed in qucsdefs.h

    :param prefix: path containing qucsator executable
    :return: list of registered components
    '''
    ext = '' if os.name != 'nt' else '.exe'
    cmd = [os.path.join(prefix,"qucsator"+ext), "-l"]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    listing = p.stdout.readlines()

    registered=[]
    defs = False
    for line in listing:
        if 'struct define_t qucs_definition_available' in line:
            defs = True
        if defs:
            if 'def_' in line:
                model=line.strip()
                model=model.strip(',')
                model=model[4:]
                registered.append(model)
    registered.sort()
    return registered
