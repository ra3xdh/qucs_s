#!/usr/bin/python
# QUCS->Python+pylab converter ver.1.0
# Public domain code, written by Wojciech M. Zabolotny
# ( wzab<at>ise.pw.edu.pl ) 20.01.2009
import sys
import pylab
class qucs_dep_var:
  def __init__(this,vals,ind_vars):
    this.val=vals
    this.ind_vars=ind_vars

class qucs_data:
  def __init__(this,fname=""):
    this.indeps={}
    this.deps={}
    if fname != "":
        f=open(fname,"rb")
        l=f.readline().strip()
        # In the first line check whether we have a qucs data
        if l != "<Qucs Dataset 0.0.18>":
           raise("This is not a qucs data file!")
        # Now we should start reading dependent vars, and independent vars
        # The next line should be either dependent variable or independent variable
        while True:
          l=f.readline().strip()
          if l=="":
             break
          if l[0:6]=="<indep":
            #This is declaration of the independent variable
            this.create_indep(l[6:-1],f)
          elif l[0:4]=="<dep":
            #This is declaration of the dependent variable
            this.create_dep(l[4:-1],f)
        f.close()

  def conv_dta(this,line):
    nline=line.replace("j","")
    if len(line)!=len(nline):
       nline=nline+"j"
    return complex(nline)
  def create_dep(this,ldef,infile):
    #Create the dependent variable with the name defined in the first field
    vnames=ldef.split()
    #Calculate the dimensions
    dims=[]
    vsize=1
    for iname in vnames[1:]:
       vs=len(this.indeps[iname])
       dims.append(vs)
       vsize*=vs
    #Reserve the data buffer
    dta = pylab.zeros(vsize,complex)
    #Read the data
    for i in xrange(0,vsize):
        l=infile.readline().strip()
        dta[i]=this.conv_dta(l)
    #Now make sure, that the last line is "<indep>"
    l=infile.readline().strip()
    if l != "</dep>":
       raise("Wrong syntax in line: "+l)
    #Reshape the data buffer into the multi-dimensional array
    dta=pylab.reshape(dta,dims,'FORTRAN')
    this.deps[vnames[0]]=qucs_dep_var(dta,vnames[1:])

  def create_indep(this,ldef, infile):
    #Create the independent variable with the name defined in the first field
    #In the first line we should find the variable name and its length
    [vname, vsize]=ldef.split()
    vsize=int(vsize)
    #Create the empty data
    dta = pylab.zeros(vsize,complex)
    #Read the data
    for i in xrange(0,vsize):
        l=infile.readline().strip()
        dta[i]=this.conv_dta(l)
    #Now make sure, that the last line is "<indep>"
    l=infile.readline().strip()
    if l != "</indep>":
       raise("Wrong syntax in line: "+l)
    this.indeps[vname]=dta


