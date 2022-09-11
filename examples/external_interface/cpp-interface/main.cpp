#include <cstdio>
#include <cmath>
#include "qucs-core/qucs_interface.h"

using namespace qucs;

// message function used to print messages from the solver
void testmessage(int level, const char * message, ...);

// example
void example_probe_and_subcircuit_access_async (void);
void example_ecvs_async (void);

int main (int argc, char ** argv)
{

    example_probe_and_subcircuit_access_async ();

    example_ecvs_async ();

    return 0;

}

void testmessage(int level, const char * message, ...)
{
    printf("%s\n", message);
}

void example_probe_and_subcircuit_access_async (void)
{
    char infile[] = "../../../example_probe_and_subcircuit_access.net";

    printf ("\nRunning example_probe_and_subcircuit_access_async ... \n");
    printf ("Demonstrates methods of extracting data during simulation.\n");
    printf ("Evaluates netlist file %s.\n\n", infile);

    char * wirelabel = "test_label";
    char * vprobename = "VProbe1";
    char * iprobename = "IProbe1";
    char * iprobename2 = "IProbe2";
    char * subcktvprobe = "SUB1.SUB1.VProbeS1";

    double node_voltage = 0;
    double probe_voltage;
    double probe_current;

    int exists = -1;

    trsolver_interface qtr;

    qtr.prepare_netlist (infile);

    qtr.setMessageFcn (&testmessage);

    double tend = 1e-3;
    double start = 0.0;
    double delta = (tend - start) / 113.0;

    qtr.init (start, delta/100, ETR_MODE_ASYNC);

    qtr.printSolution ();

    for (double t = start; t <= tend; t += delta)
    {
        qtr.stepsolve_async (t);

        // prints the whole solution, all node voltages and branch currents
        qtr.printSolution ();

        exists = qtr.getNodeV (wirelabel, node_voltage);

        if (exists == 0)
        {
            printf("%s node voltage: %f\n", wirelabel, node_voltage);
        }

        exists = qtr.getVProbeV (vprobename, probe_voltage);

        if (exists == 0)
        {
            printf ("%s probe voltage: %f\n", vprobename, probe_voltage);
        }

        exists = qtr.getIProbeI (iprobename, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename, probe_current);
        }

        exists = qtr.getIProbeI (iprobename2, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename2, probe_current);
        }


        exists = qtr.getVProbeV (subcktvprobe, probe_voltage);

        if (exists == 0)
        {
            printf ("%s probe voltage: %f\n", subcktvprobe, probe_voltage);
        }

        qtr.acceptstep_async ();

    }
}


void example_ecvs_async (void)
{
    char infile[] = "../../../example_ecvs.net";

    printf ("\nRunning example_ecvs_async function\n");
    printf ("Demonstrates setting a voltage source from external code\nduring simulation.\n");
    printf ("Evaluates netlist file %s.\n\n", infile);

    char * ecvsname = "ECVS1";
    char * iprobename = "Pr1";

    double probe_current;

    int exists = -1;

    trsolver_interface qtr;

    qtr.prepare_netlist (infile);

    qtr.setMessageFcn (&testmessage);

    double tend = 1;
    double start = 0.0;
    double delta = (tend - start) / 10;

    qtr.init (start, delta, ETR_MODE_ASYNC);

    exists = qtr.setECVSVoltage(ecvsname, 0);

    qtr.printSolution ();

    for (double t = start; t <= tend; t += delta)
    {
        exists = qtr.setECVSVoltage (ecvsname, std::sin(2 * 3.142 * 1.0 * t));

        qtr.stepsolve_async (t);

        // prints the whole solution, all node voltages and branch currents
        qtr.printSolution ();

        exists = qtr.getIProbeI (iprobename, probe_current);

        if (exists == 0)
        {
            printf ("%s probe current: %f\n", iprobename, probe_current);
        }

        // the solution must be explicitly accepted for it to become a
        // part of the circuit solution history
        qtr.acceptstep_async ();
    }
}

