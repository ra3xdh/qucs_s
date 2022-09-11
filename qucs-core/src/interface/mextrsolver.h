#include "mex.h"
#include <qucs-core/qucs_interface.h>


#ifndef MEXTRSOLVER_H
#define MEXTRSOLVER_H

// mex interface class
class mextrsolver
{
    public:
        mextrsolver();
        virtual ~mextrsolver();

        // test method
        //void debug(void);
        void printx(void);

        // the interface methods
        int prepare_netlist(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void init_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void init_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int stepsolve_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void acceptstep_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int stepsolve_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void acceptstep_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void rejectstep_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getsolution(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getN(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void getJac(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void setecvs(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void getiprobe(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void getvprobe(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void getnodev(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

    private:
        // the one and only trsolver_interface object, interface to the
        // qucs transient solver
        qucs::trsolver_interface qtr;
};

// function to display messages
void mextrsolvermessage(int, const char*, ...);

#endif // MEXTRSOLVER_H
