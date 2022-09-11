/****************************************************************************
**     Qucs Attenuator Synthesis
**     attenuatorfunc.cpp
**
**     since 2006/6/14
**
**
**
**
**
*****************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "attenuatorfunc.h"

#include <QString>

QUCS_Att::QUCS_Att(){}
QUCS_Att::~QUCS_Att(){}

int QUCS_Att::Calc(tagATT *ATT)
{
  double Lmin, L, A;
  L = pow(10, ATT->Attenuation / 10);

  A = (L + 1) / (L - 1);

  if(ATT->Zin > ATT->Zout)
    {
      Lmin = (2 * ATT->Zin / ATT->Zout) - 1 + 2 * 
	sqrt(ATT->Zin / ATT->Zout * (ATT->Zin / ATT->Zout - 1));
    }
  else
    {
      Lmin = (2 * ATT->Zout / ATT->Zin) - 1 + 2 * 
	sqrt(ATT->Zout / ATT->Zin * (ATT->Zout / ATT->Zin - 1));
    }
  ATT->MinimumATT = 10 * log10(Lmin);  

  if(ATT->MinimumATT > ATT->Attenuation)
    {
      return -1;
    }
  else
    {
      switch(ATT->Topology)
	{
	case PI_TYPE:
	  {
	    ATT->R2 = ((L - 1) / 2) * sqrt(ATT->Zin * ATT->Zout / L);
	    ATT->R1 = 1 / (((A / ATT->Zin)) - (1 / ATT->R2));
	    ATT->R3 = 1 / (((A / ATT->Zout)) - (1 / ATT->R2));
	    break;
	  }
	case TEE_TYPE:
	  {
	    ATT->R2 = (2 * sqrt(L * ATT->Zin * ATT->Zout)) / (L - 1);
	    ATT->R1 = ATT->Zin * A - ATT->R2;
	    ATT->R3 = ATT->Zout * A - ATT->R2;
	    break;
	  }
	case BRIDGE_TYPE:
	  {
	    L = pow(10, ATT->Attenuation / 20);
	    ATT->R1 = ATT->Zin * (L - 1);
	    ATT->R2 = ATT->Zin / (L - 1);
	    break;
	  }
	}
      return 0;
    }
}


QString* QUCS_Att::createSchematic(tagATT *ATT)
{
  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";
  *s += "<Components>\n";
  
  switch(ATT->Topology)
    {
    case PI_TYPE:
      *s += QString("<R R1 1 180 160 -30 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 210 130 -19 -42 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += QString("<R R3 1 240 160 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R3);
      *s += "<GND * 1 180 190 0 0 0 0>\n";
      *s += "<GND * 1 240 190 0 0 0 0>\n";
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<240 130 280 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 180 130 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 125 60 12 #000000 0 \"%1 dB Pi-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 290 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;

    case TEE_TYPE:
      *s += QString("<R R1 1 180 130 -25 -42 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 210 160 10 -4 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += QString("<R R3 1 240 130 -25 -42 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R3);
      *s += "<GND * 1 210 190 0 0 0 0>\n";
      *s +="</Components>\n";
      *s += "<Wires>\n";
      *s += "<140 130 150 130 \"\" 0 0 0 \"\">\n";
      *s += "<270 130 280 130 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 115 60 12 #000000 0 \"%1 dB Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 290 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;

    case BRIDGE_TYPE:
      *s += QString("<R R1 1 210 130 -19 -42 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
      *s += QString("<R R2 1 180 160 -30 -14 0 1 \"%1 Ohm\" 0 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
      *s += QString("<R R3 1 240 160 11 -14 0 1 \"%1 Ohm\" 0 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zout);
      *s += QString("<R R4 1 210 220 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R2);
      *s += "<GND * 1 210 250 0 0 0 0>\n";
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<240 130 280 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 180 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 190 240 190 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 100 60 12 #000000 0 \"%1 dB Bridged-Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 290 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;
    }

  return s;
}
