#include <string>
#include <qucs-core/qucs_interface.h>
#include "mextrsolver.h"

using namespace qucs;

// function to display messages
void mextrsolvermessage(int level, const char* warningmsg, ...)
{
    // we don't use level, it is just there to match the prototype
    // for the function in e_trsolver
    (void) level;

    //mexWarnMsgIdAndTxt("MATLAB:trsolver", warningmsg);

    mexPrintf("%s\n", warningmsg);
}

mextrsolver::mextrsolver()
{
    //ctor
    //thetrsolver = NULL;
}

mextrsolver::~mextrsolver()
{
    //dtor
}

void mextrsolver::printx()
{
    qtr.printSolution ();
}

int mextrsolver::prepare_netlist(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char *input_buf;
    int result;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "Three inputs required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if (mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotString",
                            "Input must be a string containing the file name.");

    /* input must be a row vector */
    if (mxGetM(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotVector",
                            "Input must be a row vector.");

    /* copy the string data from prhs[2] into a C string input_ buf.    */
    input_buf = mxArrayToString (prhs[2]);

    // call the prepare_netlist method with the input
    result = qtr.prepare_netlist (input_buf);

    if (result == NETLIST_OK)
    {
        // get the pointer to the e_trsolver analysis
        qtr.getETR ();
    }
    else if (result == NETLIST_FAILED_CHECK)
    {
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:netlistcheckfailure",
                            "The netlist file failed the netlist check.");
    }
    else if (result == NETLIST_FILE_NOT_FOUND)
    {
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:netlistcheckfailure",
                            "The netlist was not found or could not be opened.");
    }
    else
    {
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:unknownfailure",
                            "The qucs interface returned an unknown error code.");
    }

    if (!qtr.getIsInitialised ())
    {
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:noetr",
                            "The transient solver could not be initialised (Is there an ETR sim in the netlist?).");
    }
    else
    {
        // make the message function mextrsolvermessage
        qtr.setMessageFcn (&mextrsolvermessage);
    }

    // free the char array as we are done with it
    mxFree (input_buf);

    return result;
}

// solves the circuit at a specified time point
int mextrsolver::stepsolve_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");

    /* input must be a scalar */
    if ((mxGetM(prhs[2])!=1) | (mxGetN(prhs[2])!=1))
        mexErrMsgIdAndTxt( "MATLAB:trsolver:inputNotVector",
                           "Input must be a scalar.");


    double synctime = mxGetScalar(prhs[2]);

    qtr.stepsolve_sync(synctime);

    return 0;
}


void mextrsolver::acceptstep_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");


    qtr.acceptstep_sync();
}


// solves the circuit at a specified time point
int mextrsolver::stepsolve_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");

    /* input must be a scalar */
    if ((mxGetM(prhs[2])!=1) | (mxGetN(prhs[2])!=1))
        mexErrMsgIdAndTxt( "MATLAB:trsolver:inputNotVector",
                           "Input must be a scalar.");


    double synctime = mxGetScalar(prhs[2]);

    qtr.stepsolve_async(synctime);

    return 0;
}


void mextrsolver::acceptstep_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");


    qtr.acceptstep_async();
}

void mextrsolver::rejectstep_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");


    qtr.rejectstep_async();
}

// gets the last solution calculated by the solver
int mextrsolver::getsolution(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double * outpointer;
    int xN, xM, status = 0;

    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 2)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");


    xN = qtr.getN();
    xM = qtr.getM();

    //mexPrintf("%d %d\n", xN, xM);

    if (xN <= 0)
    {
        mexErrMsgIdAndTxt( "MATLAB:trsolver:nosolution",
                           "There zero nodes in the circuit.");
    }

    // copy the solution
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(xN+xM), (mwSize)(1), mxREAL);

    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    // copy the data into the array
    qtr.getsolution(outpointer);

    // copy the number of nodal voltages and branch currents
    plhs[1] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(2), mxREAL);

    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[1]);

    // copy the data into the array
    outpointer[0] = (double)xN;
    outpointer[1] = (double)xM;

    return status;
}

void mextrsolver::init_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double start;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");

    /* input must be a scalar */
    if ((mxGetM(prhs[2])!=1) | (mxGetN(prhs[2])!=1))
        mexErrMsgIdAndTxt( "MATLAB:trsolver:inputNotVector",
                           "Input must be a scalar.");


    start = mxGetScalar(prhs[2]);

    qtr.init(start, 1e-6, ETR_MODE_SYNC);
}

void mextrsolver::init_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double start, firstdelta;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:trsolver:maxlhs",
                           "Too many output arguments.");

    /* input must be a scalar */
    if ((mxGetM(prhs[2])!=1) | (mxGetN(prhs[2])!=1))
        mexErrMsgIdAndTxt( "MATLAB:trsolver:inputNotVector",
                           "Input 1 must be a scalar.");


    start = mxGetScalar(prhs[2]);

    /* input must be a scalar */
    if ((mxGetM(prhs[3])!=1) | (mxGetN(prhs[3])!=1))
        mexErrMsgIdAndTxt( "MATLAB:trsolver:inputNotVector",
                           "Input 2 must be a scalar.");


    firstdelta = mxGetScalar(prhs[3]);

    qtr.init(start, firstdelta, ETR_MODE_ASYNC);
}


// gets the last solution calculated by the solver
int mextrsolver::getN(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double * outpointer;

    /* check for proper number of arguments */
    if (nrhs!=2)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "No input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");


    int xN = qtr.getN ();

    // copy the solution
    plhs[0] = mxCreateDoubleMatrix ( (mwSize)(1), (mwSize)(1), mxREAL);

    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr (plhs[0]);

    // copy the data into the array
    outpointer[0] = (double)xN;
}


int mextrsolver::getM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double * outpointer;

    /* check for proper number of arguments */
    if (nrhs!=2)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "No input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");


    int xM = qtr.getM ();

    // copy the solution
    plhs[0] = mxCreateDoubleMatrix ( (mwSize)(1), (mwSize)(1), mxREAL);

    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr (plhs[0]);

    // copy the data into the array
    outpointer[0] = (double)xM;

}

// Get the jacobian matriz for the circuit
void mextrsolver::getJac(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double * outpointer;

    /* check for proper number of arguments */
    if (nrhs != 2)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "No input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    int jrows = qtr.getJacRows ();
    int jcols = qtr.getJacCols ();

    // copy the solution
    plhs[0] = mxCreateDoubleMatrix ( (mwSize)(jrows), (mwSize)(jcols), mxREAL);

    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr (plhs[0]);

    // copy the jacobian matrix data into the matlab matrix
    for(int c = 0; c < jcols; c++)
    {
        for(int r = 0; r < jrows; r++)
        {
            qtr.getJacData(r, c, outpointer[(c*jrows)+r]);
        }
    }

}

// sets the value of an externally controlled voltage source
void mextrsolver::setecvs(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char *ecvsname;
    double newvoltage = 0;
    int result;

    /* check for proper number of arguments */
    if (nrhs != 4)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "One input required.");
    else if (nlhs > 0)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if (mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotString",
                            "Input must be a string containing the file name.");

    /* 4th input must be a scalar */
    if (mxIsNumeric (prhs[3]) & ((mxGetM (prhs[3]) != 1) | (mxGetN (prhs[3]) != 1)))
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotVector",
                            "Voltage input must be a scalar.");


    /* copy the string data from prhs[2] into a C string ecvsname.    */
    ecvsname = mxArrayToString (prhs[2]);

    newvoltage = mxGetScalar (prhs[3]);

    result = qtr.setECVSVoltage (ecvsname, newvoltage);

    if (result != 0)
    {
        // Throw an error if the voltage source was not found
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:ecvsnotfound",
                            "The ECVS component with name %s was not found.",
                            ecvsname );
    }
}


// gets the value of a current probe
void mextrsolver::getiprobe(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char* probename;
    double* outpointer;
    double current = 0;
    int result;

    /* check for proper number of arguments */
    if (nrhs != 3)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "One input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if (mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotString",
                            "Input must be a string containing the file name.");


    /* copy the string data from prhs[2] into a C string probename.    */
    probename = mxArrayToString (prhs[2]);

    result = qtr.getIProbeI (probename, current);

    if (result != 0)
    {
        // Throw an error if the current probe was not found
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:vprobenotfound",
                            "The current probe with name %s was not found.",
                            probename );
    }
    else
    {
        // copy the solution
        plhs[0] = mxCreateDoubleMatrix ( (mwSize)(1), (mwSize)(1), mxREAL);

        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr (plhs[0]);

        // copy the data into the array
        outpointer[0] = (double)current;
    }
}

// gets the value of a voltage probe
void mextrsolver::getvprobe(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char* probename;
    double* outpointer;
    double voltage = 0;
    int result;

    /* check for proper number of arguments */
    if (nrhs != 3)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "One input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if (mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotString",
                            "Input must be a string containing the file name.");


    /* copy the string data from prhs[2] into a C string probename.    */
    probename = mxArrayToString (prhs[2]);

    result = qtr.getVProbeV (probename, voltage);

    if (result != 0)
    {
        // Throw an error if the voltage probe was not found
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:vprobenotfound",
                            "The voltage probe with name %s was not found.",
                            probename );
    }
    else
    {
        // copy the solution
        plhs[0] = mxCreateDoubleMatrix ( (mwSize)(1), (mwSize)(1), mxREAL);

        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr (plhs[0]);

        // copy the data into the array
        outpointer[0] = (double)voltage;
    }
}


// gets the value of the voltage at a named node
void mextrsolver::getnodev(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char* nodename;
    double* outpointer;
    double voltage = 0;
    int result;

    /* check for proper number of arguments */
    if (nrhs != 3)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:invalidNumInputs",
                            "One input required.");
    else if (nlhs > 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:maxlhs",
                            "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if (mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:inputNotString",
                            "Input must be a string containing the file name.");


    /* copy the string data from prhs[2] into a C string nodename. */
    nodename = mxArrayToString (prhs[2]);
    
    result = qtr.getNodeV (nodename, voltage);

    if (result != 0)
    {
        // Throw an error if the node was not found
        mexErrMsgIdAndTxt ( "MATLAB:trsolver:vprobenotfound",
                            "The voltage probe with name %s was not found.",
                            nodename );
    }
    else
    {
        // copy the solution
        plhs[0] = mxCreateDoubleMatrix ( (mwSize)(1), (mwSize)(1), mxREAL);

        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr (plhs[0]);

        // copy the data into the array
        outpointer[0] = (double)voltage;
    }
}
