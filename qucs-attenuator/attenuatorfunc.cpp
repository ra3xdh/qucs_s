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
      *s += QString("<R R1 1 180 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QString("<R R2 1 255 130 -35 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += QString("<R R3 1 330 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R3, 'f', 1));
      *s += "<GND * 1 180 230 0 0 0 0>\n";
      *s += "<GND * 1 330 230 0 0 0 0>\n";
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<330 130 330 170 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 160 60 12 #000000 0 \"%1 dB Pi-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;

    case TEE_TYPE:
      *s += QString("<R R1 1 180 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QString("<R R2 1 270 200 -20 60 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += QString("<R R3 1 350 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R3, 'f', 1));
      *s += "<GND * 1 270 230 0 0 0 0>\n";
      *s +="</Components>\n";
      *s += "<Wires>\n";
      *s += "<140 130 150 130 \"\" 0 0 0 \"\">\n";
      *s += "<210 130 320 130 \"\" 0 0 0 \"\">\n";
      *s += "<270 130 270 170 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 170 60 12 #000000 0 \"%1 dB Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 390 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;

    case BRIDGE_TYPE:
      *s += QString("<R R1 1 260 130 -30 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QString("<R R2 1 180 200 -90 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
      *s += QString("<R R3 1 340 200 11 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zout);
      *s += QString("<R R4 1 260 260 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += "<GND * 1 260 290 0 0 0 0>\n";
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<130 130 230 130 \"\" 0 0 0 \"\">\n";
      *s += "<290 130 380 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 230 340 230 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<340 130 340 170 \"\" 0 0 0 \"\">\n";
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 140 60 12 #000000 0 \"%1 dB Bridged-Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      *s += QString("<Text 400 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
      *s += "</Paintings>\n";
      break;
    }

  return s;
}
