"""Module to color messages sent to the terminal (TTY).

It can print messages in blue, green, red, yellow.

"""

import sys

# Add colors to the print messages
class bcolors:
    HEADER = '\033[95m'    # magenta
    OKBLUE = '\033[94m'    # blue
    OKGREEN = '\033[92m'   # green
    WARNING = '\033[93m'   # yellow
    FAIL = '\033[91m'      # red
    ENDC = '\033[0m'       # end-color

# Avoid color codes if output piped to file
def pb(message):
    '''
    Add blue color if message used on terminal.
    '''
    if sys.stdout.isatty() and not sys.platform=='win32':
        return bcolors.OKBLUE + message + bcolors.ENDC
    else:
        return message

def pg(message):
    '''
    Add green color if message used on terminal.
    '''
    if sys.stdout.isatty() and not sys.platform=='win32':
        return bcolors.OKGREEN + message + bcolors.ENDC
    else:
        return message

def pr(message):
    '''
    Add red color if message used on terminal.
    '''
    if sys.stdout.isatty() and not sys.platform=='win32':
        return bcolors.FAIL + message + bcolors.ENDC
    else:
        return message

def py(message):
    '''
    Add yellow color if message used on terminal.
    '''
    if sys.stdout.isatty() and not sys.platform=='win32':
        return bcolors.WARNING + message + bcolors.ENDC
    else:
        return message
