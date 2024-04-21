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
        //Design equations
	    ATT->R2 = ((L - 1) / 2) * sqrt(ATT->Zin * ATT->Zout / L);
	    ATT->R1 = 1 / (((A / ATT->Zin)) - (1 / ATT->R2));
	    ATT->R3 = 1 / (((A / ATT->Zout)) - (1 / ATT->R2));
        //Power dissipation
        ATT->PR1 = ATT->Pin*(ATT->Zin/ATT->R1);//[W]
        ATT->PR2 = ATT->Pin*(ATT->R2*pow(ATT->R1-ATT->Zin,2)/(ATT->R1*ATT->R1*ATT->Zin));//[W]
        ATT->PR3 = ATT->Pin*(pow(ATT->R1*ATT->R2-ATT->Zin*(ATT->R1+ATT->R2),2))/(ATT->R1*ATT->R1*ATT->R3*ATT->Zin);//[W]
	    break;
	  }
	case TEE_TYPE:
	  {
        //Design equations
	    ATT->R2 = (2 * sqrt(L * ATT->Zin * ATT->Zout)) / (L - 1);
	    ATT->R1 = ATT->Zin * A - ATT->R2;
	    ATT->R3 = ATT->Zout * A - ATT->R2;
        //Power dissipation
        ATT->PR1 = ATT->Pin*(ATT->R1/ATT->Zin);//[W]
        ATT->PR2 = ATT->Pin*(pow(ATT->R1-ATT->Zin,2))/(ATT->R2*ATT->Zin);//[W]
        ATT->PR3 = ATT->Pin*(ATT->R3*pow(ATT->R1+ATT->R2-ATT->Zin,2)/(ATT->Zin*ATT->R2*ATT->R2));//[W]
	    break;
	  }
	case BRIDGE_TYPE:
	  {
        //Design equations
	    L = pow(10, ATT->Attenuation / 20);
	    ATT->R1 = ATT->Zin * (L - 1);
	    ATT->R2 = ATT->Zin / (L - 1);
        ATT->R3 = ATT->Zin;//Z01
        ATT->R4 = ATT->Zin;//Z02
        //Power dissipation
        ATT->PR1 = ATT->Pin*((4*ATT->R1*ATT->R2*ATT->R2*ATT->Zin)/(pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2)));//[W]
        ATT->PR2 = ATT->Pin*(4*ATT->R2*ATT->Zin*ATT->Zin*ATT->Zin)/pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2);//[W]
        ATT->PR3 = ATT->Pin*(pow(ATT->R1*ATT->R2+ATT->Zin*ATT->Zin,2)/pow(ATT->R1*ATT->R2+ATT->Zin*(2*ATT->R2+ATT->Zin),2));//[W]
        ATT->PR4 = 0;//Z02 dissipates no power.
	    break;
	  }
      case REFLECTION_TYPE:
      {
        //Design equations
        L = pow(10, ATT->Attenuation / 20);
        if (ATT->minR)
            ATT->R1 = ATT->Zin*(L + 1)/(L - 1);
        else
            ATT->R1 = ATT->Zin*(L - 1)/(L + 1);
        ATT->R2 = ATT->R1;
        //Power dissipation. Both resistor dissipate the same power
        ATT->PR1 = 0.5*ATT->Pin*(1-pow(abs((ATT->Zin-ATT->R1)/(ATT->Zin+ATT->R1)),2));
        ATT->PR2 = ATT->PR1;
         break;
      }
	}
      return 0;
    }
}

//This function creates the schematic. It receives the attenuator resistor values (tagATT * ATT) and bool flag to include a S-parameter box in the schematic
QString* QUCS_Att::createSchematic(tagATT *ATT, bool SP_box)
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
      if (SP_box)
       {
         // S-parameter simulation block
         //-----------------------------
         // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
         // sweep and then the user can modify that in the schematic
         *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

         // Equations
         *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

         // Input term
         *s += QString("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
         *s += "<GND * 1 50 230 0 0 0 0>\n";

         // Output term
         *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
         *s += "<GND * 1 460 230 0 0 0 0>\n";
       }
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<330 130 330 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
           {   // Additional wiring because of the input/output ports

               // Input port
               *s += "<50 130 50 170 \"\" 0 0 0 \"\">\n";
               *s += "<50 130 140 130 \"\" 0 0 0 \"\">\n";

               // Output port
               *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
               *s += "<460 130 350 130 \"\" 0 0 0 \"\">\n";
           }
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 160 60 12 #000000 0 \"%1 dB Pi-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
         {// If the SP simulation box option is activated, then the input and output ports are attached.
          // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QString("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
         }
      *s += "</Paintings>\n";
      break;

    case TEE_TYPE:
      *s += QString("<R R1 1 180 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QString("<R R2 1 270 200 -20 60 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += QString("<R R3 1 350 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R3, 'f', 1));
      *s += "<GND * 1 270 230 0 0 0 0>\n";
      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

        // Equations
        *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 70 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 70 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 460 230 0 0 0 0>\n";
      }
      *s +="</Components>\n";
      *s += "<Wires>\n";
      *s += "<140 130 150 130 \"\" 0 0 0 \"\">\n";
      *s += "<210 130 320 130 \"\" 0 0 0 \"\">\n";
      *s += "<270 130 270 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<70 130 70 170 \"\" 0 0 0 \"\">\n";
          *s += "<70 130 140 130 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
          *s += "<460 130 380 130 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 170 60 12 #000000 0 \"%1 dB Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 390 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

    case BRIDGE_TYPE:
      *s += QString("<R R1 1 260 130 -30 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QString("<R R2 1 180 200 -90 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
      *s += QString("<R R3 1 340 200 11 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zout);
      *s += QString("<R R4 1 260 260 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += "<GND * 1 260 290 0 0 0 0>\n";
      if (SP_box)
      {
        // S-parameter simulation block
        //-----------------------------
        // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
        // sweep and then the user can modify that in the schematic
        *s += "<.SP SP1 1 140 350 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

        // Equations
        *s += "<Eqn Eqn1 1 360 350 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

        // Input term
        *s += QString("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 50 230 0 0 0 0>\n";

        // Output term
        *s += QString("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
        *s += "<GND * 1 460 230 0 0 0 0>\n";
      }
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<130 130 230 130 \"\" 0 0 0 \"\">\n";
      *s += "<290 130 380 130 \"\" 0 0 0 \"\">\n";
      *s += "<180 230 340 230 \"\" 0 0 0 \"\">\n";
      *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
      *s += "<340 130 340 170 \"\" 0 0 0 \"\">\n";
      if (SP_box)
      {   // Additional wiring because of the input/output ports

          // Input port
          *s += "<50 130 50 170 \"\" 0 0 0 \"\">\n";
          *s += "<50 130 140 130 \"\" 0 0 0 \"\">\n";

          // Output port
          *s += "<460 130 460 170 \"\" 0 0 0 \"\">\n";
          *s += "<460 130 380 130 \"\" 0 0 0 \"\">\n";
      }
      *s += "</Wires>\n";
      *s += "<Diagrams>\n";
      *s += "</Diagrams>\n";
      *s += "<Paintings>\n";
      *s += QString("<Text 140 60 12 #000000 0 \"%1 dB Bridged-Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QString("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QString("<Text 400 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

      case REFLECTION_TYPE:
        *s += QString("<R R1 1 130 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
        *s += "<GND * 1 130 330 0 0 0 0>\n";
        *s += QString("<Coupler X1 5 200 200 29 -26 0 1 \"0.7071\" 0 \"90\" 0 \"%1\" 0>\n").arg(ATT->Zin);
        *s += QString("<R R1 1 270 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->R1);
        *s += "<GND * 1 270 330 0 0 0 0>\n";

        if (SP_box)
        {
          // S-parameter simulation block
          //-----------------------------
          // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
          // sweep and then the user can modify that in the schematic
          *s += "<.SP SP1 1 80 400 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n";

          // Equations
          *s += "<Eqn Eqn1 1 300 400 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

          // Input term
          *s += QString("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
          *s += "<GND * 1 50 230 0 0 0 0>\n";

          // Output term
          *s += QString("<Pac P1 1 350 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
          *s += "<GND * 1 350 230 0 0 0 0>\n";
        }
        *s += "</Components>\n";

        *s += "<Wires>\n";
        //First resistor to the coupler
        *s += "<130 270 130 250 \"\" 0 0 0 \"\">\n";
        *s += "<130 250 180 250 \"\" 0 0 0 \"\">\n";
        *s += "<180 230 180 250 \"\" 0 0 0 \"\">\n";

        //Second resistor to the coupler
        *s += "<270 270 270 250 \"\" 0 0 0 \"\">\n";
        *s += "<270 250 220 250 \"\" 0 0 0 \"\">\n";
        *s += "<220 230 220 250 \"\" 0 0 0 \"\">\n";

        *s += "<100 170 180 170 \"\" 0 0 0 \"\">\n";
        *s += "<220 170 300 170 \"\" 0 0 0 \"\">\n";

        if (SP_box)
        {   // Additional wiring because of the input/output ports

            // Input port
            *s += "<50 170 100 170 \"\" 0 0 0 \"\">\n";

            // Output port
            *s += "<300 170 350 170 \"\" 0 0 0 \"\">\n";
        }
        *s += "</Wires>\n";

        *s += "<Diagrams>\n";
        *s += "</Diagrams>\n";
        *s += "<Paintings>\n";
        *s += QString("<Text 100 100 12 #000000 0 \"%1 dB Reflection Attenuator\">\n").arg(ATT->Attenuation);
        if (!SP_box)
        {// If the SP simulation box option is activated, then the input and output ports are attached.
         // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QString("<Text 70 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QString("<Text 270 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zout);
        }
        *s += "</Paintings>\n";

    break;
    }

  return s;
}
