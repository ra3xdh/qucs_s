/*
 * qucs_interface.cpp - m-code interface implementation
 *
 * Copyright (C) 2013 Richard Crozier <richard.crozier@yahoo.co.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "logging.h"
#include "precision.h"
#include "component.h"
#include "components.h"
#include "net.h"
#include "input.h"
#include "dataset.h"
#include "equation.h"
#include "environment.h"
#include "exceptionstack.h"
#include "check_netlist.h"
#include "module.h"
#include "qucs_interface.h"
#include "analysis.h"
#include "e_trsolver.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string>

using namespace qucs;

// constructor
qucsint::qucsint ()
{

    //int listing = 0;
    ret = 0;
    err = 0;

    loginit ();
    ::srand (::time (NULL));

}

qucsint::qucsint (char* infile)
{
    //int listing = 0;
    ret = 0;
    err = 0;

    loginit ();
    ::srand (::time (NULL));

    prepare_netlist (infile);
}

// destructor
qucsint::~qucsint ()
{
    delete subnet;
    delete in;
//    delete out;
    delete root;

    // delete modules
    module::unregisterModules ();

    netlist_destroy_env ();
}

/*!\ todo: replace "root" by / as environment root */
int qucsint::prepare_netlist (char * infile)
{

    // create static modules
    module::registerModules ();

    // create root environment
    root = new qucs::environment (std::string("root"));

    // create netlist object and input
    subnet = new net ("subnet");

    // test if the file actually exists
    FILE * pFile;
    pFile = fopen (infile,"r");
    if (pFile!=NULL)
    {
        // close the file again
        fclose (pFile);
    }
    else
    {
        return NETLIST_FILE_NOT_FOUND;
    }

    in = infile ? new input (infile) : new input ();

    // pass root environment to netlist object and input
    subnet->setEnv (root);
    in->setEnv (root);

    // get input netlist
    if (in->netlist (subnet) != 0)
    {
        if (netlist_check)
        {
            // replace with mex error message
            logprint (LOG_STATUS, "checker notice, netlist check FAILED\n");
        }
        return NETLIST_FAILED_CHECK;
    }

    // attach a ground to the netlist
    gnd = new ground ();
    gnd->setNode (0, "gnd");
    gnd->setName ("GND");
    subnet->insertCircuit (gnd);

    // apply some data to all analyses
    subnet->setActionNetAll(subnet);

    return NETLIST_OK;
}

int qucsint::evaluate ()
{
    // analyse the netlist
    err = 0;
    ret = 0;
    out = subnet->runAnalysis (err);
    ret |= err;

    return ret;
}

int qucsint::output (char * outfile)
{
    // evaluate output dataset
    ret |= root->equationSolver (out);

    if (outfile != NULL)
    {
        out->setFile (outfile);
        out->print ();
    }

    return ret;
}

/*/////////////////////////////////////////////////////////////////////////////

                            trsolver_interface

/////////////////////////////////////////////////////////////////////////////*/

trsolver_interface::trsolver_interface ()
    : qucsint ()
{
    etr = 0;
    isInitialised = false;
}

trsolver_interface::trsolver_interface (char * infile)
    : qucsint (infile)
{
    isInitialised = false;

    int result = prepare_netlist (infile);

    if (result == NETLIST_OK)
    {
        getETR ();
    }
}

//trsolver_interface::~trsolver_interface ()
//{
//
//}

int trsolver_interface::getN (void)
{
    if (etr) return etr->getN ();
    else return -2;
}

int trsolver_interface::getM (void)
{
    if (etr) return etr->getM ();
    else return -2;
}

// gets a pointer to the trsolver_interface if it is present
void trsolver_interface::getETR()
{
    // get a pointer to the external transient solver interface
    // the pointer will be to the supclass of e_trsolver, analysis
    // so we must perform a dynamic cast to convert it to an e_trsolver
    etr = dynamic_cast<e_trsolver *>(subnet->findAnalysis (ANALYSIS_E_TRANSIENT));

    if (etr)
    {
        isInitialised = true;
    }
    else
    {
        isInitialised = false;
    }
}

int trsolver_interface::init (double start, double firstdelta, int mode)
{
    if (etr) return etr->init ((nr_double_t)start, (nr_double_t)firstdelta, mode);
    else return -2;
}


int trsolver_interface::stepsolve_sync (double synctime)
{
    if (etr) return etr->stepsolve_sync ((nr_double_t)synctime);
    else return -2;
}

void trsolver_interface::acceptstep_sync (void)
{
    if (etr) etr->acceptstep_sync ();
}

int trsolver_interface::stepsolve_async (double steptime)
{
    if (etr) return etr->stepsolve_async ((nr_double_t)steptime);
    else return -2;
}

void trsolver_interface::acceptstep_async (void)
{
    if (etr) etr->acceptstep_async ();
}

void trsolver_interface::rejectstep_async (void)
{
    if (etr) etr->rejectstep_async ();
}

void trsolver_interface::getsolution (double * sol)
{
    if (etr) etr->getsolution (sol);
}

int trsolver_interface::setECVSVoltage(char * ecvsname, double V)
{
    if (etr) return etr->setECVSVoltage (ecvsname, (nr_double_t)V);
    else return -2;
}

int trsolver_interface::getJacRows ()
{
    if (etr) return etr->getJacRows ();
    else return -2;
}

int trsolver_interface::getJacCols ()
{
    if (etr) return etr->getJacCols ();
    else return -2;
}

int trsolver_interface::getJacData (int r, int c, double& data)
{
    if (etr)
    {
        nr_double_t tempdata = (nr_double_t)data;
        etr->getJacData (r, c, tempdata);
        data = (double)tempdata;
        return 0;
    }
    else
    {
        return -2;
    }
}

int trsolver_interface::getNodeV (char * label, double& nodeV)
{
    if (etr)
    {
        nr_double_t tempnodeV = (nr_double_t)nodeV;
        int flag = etr->getNodeV (label, tempnodeV);
        nodeV = (double)tempnodeV;
        return flag;
    }
    else
    {
        return -2;
    }
}

int trsolver_interface::getVProbeV (char * probename, double& probeV)
{
    if (etr)
    {
        nr_double_t tempprobeV = (nr_double_t)probeV;
        int flag = etr->getVProbeV (probename, tempprobeV);
        probeV = (double)tempprobeV;
        return flag;
    }
    else
    {
        return -2;
    }
}


int trsolver_interface::getIProbeI (char * probename, double& probeI)
{
    if (etr)
    {
        nr_double_t tempprobeI = (nr_double_t)probeI;
        int flag = etr->getIProbeI (probename, tempprobeI);
        probeI = (double)tempprobeI;
        return flag;
    }
    else
    {
        return -2;
    }
}

//void trsolver_interface::debug (void)
//{
//    if (etr) etr->debug ();
//}
//

void trsolver_interface::printSolution (void)
{
    if (etr) etr->printx ();
}

void trsolver_interface::setMessageFcn(void (*newmessagefcn)(int level, const char * format, ...))
{
    etr->messagefcn = newmessagefcn;
}
