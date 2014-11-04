#!/usr/bin/env python
# encoding: utf-8

import os
from subprocess import Popen, PIPE, call
import re


makedir = "/home/yodalee/git/qucs/qucs/qucs"
errorfilepath = "/tmp/undefined"
cmd = ['make', '-k']
editcmd = ['vim', '-o']
classReg = re.compile("'([^']*)'")

files = {}


def parse(line):
    return line.split(':')[0], classReg.search(line).group(1)

cwd = os.getcwd()
try:
    os.chdir(makedir)
    p = Popen(cmd, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()

    # parse error
    errormsg = str(stderr)
    for line in errormsg.split('\\n'):
        if "unknown type" in line or "undefined template" in line or "incomplete type" in line:
            filename = line.split(':')[0]
            classname = line.split("'")[1]

            if filename in files:
                if classname not in files[filename]:
                    files[filename].append(classname)
            else:
                files[filename] = []
                files[filename].append(classname)

    # write to file
    try:
        writefile = open(errorfilepath, "w")
        for f in iter(files):
            if not f.endswith(".moc.cpp"):
                writefile.write(f + "\n")
                if f.endswith(".h"):
                    for c in files[f]:
                        writefile.write("class " + c + ";\n")
                    for c in files[f]:
                        writefile.write("#include <" + c + ">\n")
                elif f.endswith(".cpp"):
                    for c in files[f]:
                        writefile.write("#include <" + c + ">\n")
                writefile.write("\n")

    finally:
        writefile.close()

    # open vim for edit 
    for f in iter(files):
        filename = os.path.basename(f)
        for root, dirs, filelist in os.walk(makedir):
            for editfile in filelist:
                if editfile == filename:
                    call(['vim', '-O', os.path.join(root, editfile), errorfilepath])

finally:
    os.chdir(cwd)
