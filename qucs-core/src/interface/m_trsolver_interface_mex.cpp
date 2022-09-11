#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <typeinfo>
#include "mex.h"
#include "class_handle.hpp"
#include "mextrsolver.h"

using namespace std;

// Value-Defintions of the different class methods available
enum ClassMethods { evNotDefined,
                    prepare_netlist,
                    init_sync,
                    init_async,
                    stepsolve_sync,
                    acceptstep_sync,
                    stepsolve_async,
                    acceptstep_async,
                    rejectstep_async,
                    getsolution,
                    debug,
                    printx,
                    getN,
                    getM,
                    getJac,
                    setecvs,
                    getnodev,
                    getvprobe,
                    getiprobe
                  };

// Map to associate the command strings with the class
// method enum values
std::map<std::string, ClassMethods> s_mapClassMethodStrs;

void Initialize()
{
    // Set up the class methods map
    s_mapClassMethodStrs["prepare_netlist"]     = prepare_netlist;
    s_mapClassMethodStrs["init_sync"]           = init_sync;
    s_mapClassMethodStrs["init_async"]          = init_async;
    s_mapClassMethodStrs["stepsolve_sync"]      = stepsolve_sync;
    s_mapClassMethodStrs["acceptstep_sync"]     = acceptstep_sync;
    s_mapClassMethodStrs["stepsolve_async"]     = stepsolve_async;
    s_mapClassMethodStrs["acceptstep_async"]    = acceptstep_async;
    s_mapClassMethodStrs["rejectstep_async"]    = rejectstep_async;
    s_mapClassMethodStrs["getsolution"]         = getsolution;
    s_mapClassMethodStrs["debug"]               = debug;
    s_mapClassMethodStrs["printx"]              = printx;
    s_mapClassMethodStrs["getN"]                = getN;
    s_mapClassMethodStrs["getM"]                = getM;
    s_mapClassMethodStrs["getJac"]              = getJac;
    s_mapClassMethodStrs["setecvs"]             = setecvs;
    s_mapClassMethodStrs["getnodev"]            = getnodev;
    s_mapClassMethodStrs["getvprobe"]           = getvprobe;
    s_mapClassMethodStrs["getiprobe"]           = getiprobe;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Get the command string
    char cmd[128];

    Initialize();

    if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
    {
        mexErrMsgTxt("First input should be a command string less than 128 characters long.");
    }

    // New
    if (!strcmp("new", cmd))
    {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("New: One output expected.");
        // Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<mextrsolver>(new mextrsolver);
        return;
    }

    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
        mexErrMsgTxt("Second input should be a class instance handle.");

    // Delete
    if (!strcmp("delete", cmd))
    {
        // Destroy the C++ object
        destroyObject<mextrsolver>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return;
    }

    // Get the class instance pointer from the second input
    mextrsolver *mextrsolver_instance = convertMat2Ptr<mextrsolver>(prhs[1]);

    // Call the various class methods
    // Switch on the value
    switch(s_mapClassMethodStrs[cmd])
    {
    case prepare_netlist:
        mextrsolver_instance->prepare_netlist(nlhs, plhs, nrhs, prhs);
        return;
	case init_sync:
        mextrsolver_instance->init_sync(nlhs, plhs, nrhs, prhs);
        return;
	case init_async:
        mextrsolver_instance->init_async(nlhs, plhs, nrhs, prhs);
        return;
    case stepsolve_sync:
        mextrsolver_instance->stepsolve_sync(nlhs, plhs, nrhs, prhs);
        return;
    case acceptstep_sync:
        mextrsolver_instance->acceptstep_sync(nlhs, plhs, nrhs, prhs);
        return;
    case stepsolve_async:
        mextrsolver_instance->stepsolve_async(nlhs, plhs, nrhs, prhs);
        return;
	case acceptstep_async:
        mextrsolver_instance->acceptstep_async(nlhs, plhs, nrhs, prhs);
        return;
    case rejectstep_async:
        mextrsolver_instance->rejectstep_async(nlhs, plhs, nrhs, prhs);
        return;
    case getsolution:
        mextrsolver_instance->getsolution(nlhs, plhs, nrhs, prhs);
        return;
//     case debug:
//         mextrsolver_instance->debug();
//         return;
    case printx:
        mextrsolver_instance->printx();
        return;
    case getN:
        mextrsolver_instance->getN(nlhs, plhs, nrhs, prhs);
        return;
    case getM:
        mextrsolver_instance->getM(nlhs, plhs, nrhs, prhs);
        return;
    case getJac:
        mextrsolver_instance->getJac(nlhs, plhs, nrhs, prhs);
        return;
    case setecvs:
        mextrsolver_instance->setecvs(nlhs, plhs, nrhs, prhs);
        return;
    case getnodev:
        mextrsolver_instance->getnodev(nlhs, plhs, nrhs, prhs);
        return;
    case getvprobe:
        mextrsolver_instance->getvprobe(nlhs, plhs, nrhs, prhs);
        return;
    case getiprobe:
        mextrsolver_instance->getiprobe(nlhs, plhs, nrhs, prhs);
        return;
    default:
        mexErrMsgTxt("Unrecognised class command string.");
        break;
    }

    // Got here, so command not recognized
    //mexErrMsgTxt("Command not recognized.");
}
