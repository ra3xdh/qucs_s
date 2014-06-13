/***************************************************************************
    copyright            : (C) 2008 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

typedef char*   pChar;
typedef char**  ppChar;

// Rules for the list:
// First string in list is the property name.
// Value and material name must be separated by two spaces!

// specific resistance in ohms*meters
pChar List_rho[] = {"rho",
   "2.4e-8  gold", "1.72e-8  copper", "1.62e-8  silver", "12.4e-8  tin",
   "10.5e-8  platinum", "2.62e-8  aluminum", "6.9e-8  nickel",
   "3.9e-8  brass (66Cu 34Zn)", "9.71e-8  iron", "6.0e-8  zinc", 0};

// relative dielectric constant
pChar List_er[] = {"er",
   "9.8  alumina (Al2O3)", "3.78  fused quartz", "3.38  RO4003",
   "2.2  RT/duroid 5880", "10.2  RT/duroid 6010LM", "2.1  teflon (PTFE)",
   "4.0  PVC", "2.3  PE", "4.5  FR4", "6.6  beryllia (BeO)",
   "8.7  aluminum nitride", "11.9  silicon", "12.9  GaAs", 0};

// dielectric loss factor (tangent delta)
pChar List_tand[] = {"tand",
   "2e-4  aluminia (Al2O3) @ 10GHz", "1e-4  fused quartz @ 10GHz",
   "2e-3  RO4003 @ 10GHz", "9e-4  RT/duroid 5880 @ 10GHz",
   "2e-4  teflon (PTFE) @ 1MHz", "5e-2  PVC @ 1MHz", "2e-4  PE @ 1MHz",
   "2e-2  FR4 @ 1GHz", "3e-4  beryllia @ 10GHz",
   "1e-3  aluminum nitride @ 10GHz", "0.015  silicon @ 10GHz",
   "0.002  GaAs @ 10GHz", 0};


ppChar List_Properties[] =
  {(char**)&List_er, (char**)&List_tand, (char**)&List_rho, 0};
