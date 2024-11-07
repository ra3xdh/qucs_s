/****************************************************************************
**     Qucs Attenuator Synthesis
**     attenuatorfunc.cpp
**
**     since 2006/6/14
**     updated on 2024/05/17
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

// References:
// [1] RF design guide. Systems, circuits, and equations. Peter Vizmuller. Artech House, 1995
// [2] The PIN diode circuit designer's handbook. W.E. Doherty, Jr., R.D. Joos, Microsemi Corp., 1998

QUCS_Att::QUCS_Att(){}
QUCS_Att::~QUCS_Att(){}

int QUCS_Att::Calc(tagATT *ATT)
{
  double Lmin, L, A;
  L = pow(10, ATT->Attenuation / 10);

  A = (L + 1) / (L - 1);
  //Check minumum attenuation for Pi and Tee type attenuators
   if ((ATT->Topology == PI_TYPE) || (ATT->Topology == TEE_TYPE))
   {
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
    }
    switch(ATT->Topology)
	{
	case PI_TYPE:
	  {
        //Design equations [1]
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
        //Design equations [1]
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
        //Design equations [1]
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
        //Design equations [2]
        L = pow(10, ATT->Attenuation / 20);
        if (ATT->minR)
            ATT->R1 = ATT->Zin*(L + 1)/(L - 1);
        else
            ATT->R1 = ATT->Zin*(L - 1)/(L + 1);
        ATT->R2 = ATT->R1;
        //Power dissipation. Both resistors dissipate the same power
        ATT->PR1 = 0.5*ATT->Pin*(1-pow(abs((ATT->Zin-ATT->R1)/(ATT->Zin+ATT->R1)),2));
        ATT->PR2 = ATT->PR1;
         break;
      }
    case QW_SERIES_TYPE:
      {
         //Design equations [2]
         L = pow(10, 0.05*ATT->Attenuation);
         ATT->R1  = ATT->Zin/(L-1);
         ATT->R2 = ATT->Zin;
         ATT->R3 = ATT->R1;
         ATT->R4 = (ATT->R1*ATT->R1*ATT->Zin + 2*ATT->R1*ATT->Zin*ATT->Zin)/(ATT->R1*ATT->R1 + 2*ATT->R1*ATT->Zin+2*ATT->Zin*ATT->Zin); // Zout
         ATT->L = 0.25*C0/ATT->freq;//lambda/4
         //Power dissipation.
         ATT->PR1 = ATT->Pin*ATT->R1*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR2 = ATT->Pin*ATT->Zin*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR3 = ATT->PR1;
         break;
      }
    case QW_SHUNT_TYPE:
      {
         //Design equations [2]
         L = pow(10, 0.05*ATT->Attenuation);
         ATT->R1  = ATT->Zin*(L-1);
         ATT->R2 = ATT->Zin;
         ATT->R3 = ATT->R1;
         ATT->R4 = ATT->R1 + ATT->Zin*(ATT->R1+ATT->Zin)/(2*ATT->R1+ATT->Zin); // Zout
         ATT->L = 0.25*C0/ATT->freq;//lambda/4
         //Power dissipation.
         ATT->PR1 = ATT->Pin*ATT->R1*ATT->Zin/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR2 = ATT->Pin*ATT->R1*ATT->R1/pow(ATT->R1 + ATT->Zin,2);
         ATT->PR3 = ATT->PR1;
         break;
       }
     case L_PAD_1ST_SERIES:
       {
        L = pow(10, -ATT->Attenuation / 10);
        // Design equations
        ATT->R1 = -ATT->Zin*(L-1)/(sqrt(L)+1);// Series resistor
        ATT->R2 = -ATT->Zin*(L + sqrt(L))/(L - 1); // Shunt resistor
        ATT->R3 = ATT->R2*(ATT->R1+ATT->Zin)/(ATT->R1+ATT->R2+ATT->Zin);//Output impedance
        // Power dissipation
        ATT->PR1 = ATT->Pin*(1-sqrt(L));// Series resistor
        ATT->PR2 = ATT->Pin*L*(1-L)/(L+sqrt(L));// Shunt resistor
        break;
       }
    case L_PAD_1ST_SHUNT:
      {
       L = pow(10, -ATT->Attenuation / 10);
       // Design equations
       ATT->R1 = ATT->Zin/(1-sqrt(L));// Shunt resistor
       ATT->R2 = ATT->Zin*(1-sqrt(L))/(sqrt(L)); // Series resistor
       ATT->R3 = -ATT->Zin*(L-2*sqrt(L)+2)/(L-2*sqrt(L));//Output impedance
       // Power dissipation
       ATT->PR1 = ATT->Pin*sqrt(L)*(1-2*sqrt(L)+L)/(1-sqrt(L));// Shunt resistor
       ATT->PR2 = ATT->Pin*(1-sqrt(L));// Series resistor
       break;
      }
    case R_SERIES:
      {
       L = pow(10, -ATT->Attenuation / 10);
       // Design equations
       ATT->R1 = -((ATT->Zout + ATT->Zin)*L - 2*sqrt(ATT->Zout*ATT->Zin*L))/L;// Series resistor
       ATT->R2 = ATT->Zout + ATT->R1; // Input impedance
       ATT->R3 = ATT->Zin + ATT->R1;//Output impedance
       // Power dissipation
       ATT->PR1 = ATT->Pin*(1-L);// Shunt resistor
       break;
      }
    case R_SHUNT:
      {
      L = pow(10, -ATT->Attenuation / 10);
      // Design equations
      ATT->R1 = (2*sqrt(ATT->Zout*ATT->Zin*L)*ATT->Zout*ATT->Zin + (ATT->Zout*ATT->Zout*ATT->Zin + ATT->Zout*ATT->Zin*ATT->Zin)*L)/(4*ATT->Zout*ATT->Zin - (ATT->Zout*ATT->Zout + 2*ATT->Zout*ATT->Zin + ATT->Zin*ATT->Zin)*L);// Shunt resistor
      ATT->R2 = (ATT->Zout*ATT->R1)/(ATT->Zout+ATT->R1); // Input impedance
      ATT->R3 = (ATT->Zin*ATT->R1)/(ATT->Zin+ATT->R1);//Output impedance
      // Power dissipation
      ATT->PR1 = ATT->Pin*(1-L);// Shunt resistor
      break;
      }

	}
   return 0;

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
      *s += QStringLiteral("<R R1 1 180 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QStringLiteral("<R R2 1 255 130 -35 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += QStringLiteral("<R R3 1 330 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R3, 'f', 1));
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
         *s += QStringLiteral("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
         *s += "<GND * 1 50 230 0 0 0 0>\n";

         // Output term
         *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
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
      *s += QStringLiteral("<Text 160 60 12 #000000 0 \"%1 dB Pi-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
         {// If the SP simulation box option is activated, then the input and output ports are attached.
          // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QStringLiteral("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
         }
      *s += "</Paintings>\n";
      break;

    case TEE_TYPE:
      *s += QStringLiteral("<R R1 1 180 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QStringLiteral("<R R2 1 270 200 -20 60 0 3 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
      *s += QStringLiteral("<R R3 1 350 130 -40 20 0 2 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R3, 'f', 1));
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
        *s += QStringLiteral("<Pac P1 1 70 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 70 230 0 0 0 0>\n";

        // Output term
        *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
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
      *s += QStringLiteral("<Text 170 60 12 #000000 0 \"%1 dB Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QStringLiteral("<Text 390 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

    case BRIDGE_TYPE:
      *s += QStringLiteral("<R R1 1 260 130 -30 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
      *s += QStringLiteral("<R R2 1 180 200 -90 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
      *s += QStringLiteral("<R R3 1 340 200 11 -30 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zout);
      *s += QStringLiteral("<R R4 1 260 260 11 -14 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
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
        *s += QStringLiteral("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 50 230 0 0 0 0>\n";

        // Output term
        *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
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
      *s += QStringLiteral("<Text 140 60 12 #000000 0 \"%1 dB Bridged-Tee-Type Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
      {// If the SP simulation box option is activated, then the input and output ports are attached.
       // Thus, it doesn't make sense to have a text field indicating the input/output impedance
          *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
          *s += QStringLiteral("<Text 400 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
      }
      *s += "</Paintings>\n";
      break;

      case REFLECTION_TYPE:
        *s += QStringLiteral("<R R1 1 130 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += "<GND * 1 130 330 0 0 0 0>\n";
        *s += QStringLiteral("<Coupler X1 5 200 200 29 -26 0 1 \"0.7071\" 0 \"90\" 0 \"%1\" 0>\n").arg(ATT->Zin);
        *s += QStringLiteral("<R R1 1 270 300 15 -26 0 1 \"%1\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += "<GND * 1 270 330 0 0 0 0>\n";

        if (SP_box)
        {
          // S-parameter simulation block
          //-----------------------------
          // Resistor attenuators are broadband ckts, so it's pointless to ask the user to input the analysis freq sweep. Let's do a wideband
          // sweep and then the user can modify that in the schematic
          *s += QStringLiteral("<.SP SP1 1 80 400 0 83 0 0 \"lin\" 1 \"50 MHz\" 1 \"3 GHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n");

          // Equations
          *s += "<Eqn Eqn1 1 300 400 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

          // Input term
          *s += QStringLiteral("<Pac P1 1 50 200 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
          *s += "<GND * 1 50 230 0 0 0 0>\n";

          // Output term
          *s += QStringLiteral("<Pac P1 1 350 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
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
        *s += QStringLiteral("<Text 100 100 12 #000000 0 \"%1 dB Reflection Attenuator\">\n").arg(ATT->Attenuation);
        if (!SP_box)
        {// If the SP simulation box option is activated, then the input and output ports are attached.
         // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QStringLiteral("<Text 70 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QStringLiteral("<Text 270 135 10 #000000 0 \"Z0: %1 Ohm\">\n").arg(ATT->Zout);
        }
        *s += "</Paintings>\n";

        break;

      case QW_SERIES_TYPE:
        if (ATT->useLumped)
        {
          double w = 2*PI*ATT->freq;
          *s += QStringLiteral("<L L1 1 250 0 -40 -60 0 0 \"%1H\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(num2str(ATT->Zin/w));
          *s += QStringLiteral("<C C1 1 180 -60 -90 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
          *s += "<GND * 1 180 -90 0 0 1 0>\n";
          *s += QStringLiteral("<C C1 1 320 -60 20 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
          *s += "<GND * 1 320 -90 0 0 1 0>\n";
        }
        else
        {
            *s += QStringLiteral("<TLIN Line1 1 250 0 -38 -75 0 0 \"%1 Ohm\" 1 \"%2 mm\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(ATT->Zin).arg(QString::number(ATT->L*1e3, 'f', 1));
        }
        *s += QStringLiteral("<R R1 1 100 50 15 -26 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += QStringLiteral("<R R1 1 100 150 15 -26 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 100 180 0 0 0 0>\n";
        *s += QStringLiteral("<R R1 1 400 150 -100 -15 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += "<GND * 1 400 180 0 0 0 0>\n";

        if (SP_box)
        {
          // S-parameter simulation block
          //-----------------------------
          // The quarter-wave line is a narrowband device... so let's set the SP sweep from f0/2 to 3*f0/2
          QString freq_start = QStringLiteral("%1").arg(0.5*ATT->freq*1e-6);//MHz
          QString freq_stop = QStringLiteral("%1").arg(1.5*ATT->freq*1e-6);//MHz
          *s += QStringLiteral("<.SP SP1 1 100 270 0 83 0 0 \"lin\" 1 \"%1 MHz\" 1 \"%2 MHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(freq_start).arg(freq_stop);

          // Equations
          *s += "<Eqn Eqn1 1 320 270 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

          // Input term
          *s += QStringLiteral("<Pac P1 1 0 150 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
          *s += "<GND * 1 0 180 0 0 0 0>\n";

          // Output term
          *s += QStringLiteral("<Pac P1 1 500 150 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
          *s += "<GND * 1 500 180 0 0 0 0>\n";
        }
        *s += "</Components>\n";

        *s += "<Wires>\n";
        *s += "<100 20 100 0 \"\" 0 0 0 \"\">\n";
        *s += "<50 0 220 0 \"\" 0 0 0 \"\">\n";
        *s += "<100 80 100 120 \"\" 0 0 0 \"\">\n";
        *s += "<400 120 400 0 \"\" 0 0 0 \"\">\n";
        *s += "<280 0 450 0 \"\" 0 0 0 \"\">\n";

        if (SP_box)
        {
            //Term 1 to input port
            *s += "<0 120 0 0 \"\" 0 0 0 \"\">\n";
            *s += "<0 0 50 0 \"\" 0 0 0 \"\">\n";

            //Term 2 to output port
            *s += "<500 120 500 0 \"\" 0 0 0 \"\">\n";
            *s += "<450 0 500 0 \"\" 0 0 0 \"\">\n";
        }
        if (ATT->useLumped)
        {//Add extra wiring to connect the shunt capacitors to the main line
            *s += "<180 -30 180 0 \"\" 0 0 0 \"\">\n";
            *s += "<320 -30 320 0 \"\" 0 0 0 \"\">\n";
        }
        *s += "</Wires>\n";
        *s += "<Diagrams>\n";
        *s += "</Diagrams>\n";
        *s += "<Paintings>\n";

        //In the case of the Pi-equivalent of the quarter wavelength line it is needed to put the title slighly higher.
        if (ATT->useLumped) *s += QStringLiteral("<Text 80 -140 12 #000000 0 \"%1 dB @ %2Hz Quarter-Wave series attenuator\">\n").arg(ATT->Attenuation).arg(num2str(ATT->freq));
        else *s += QStringLiteral("<Text 80 -120 12 #000000 0 \"%1 dB @ %2Hz Quarter-Wave series attenuator\">\n").arg(ATT->Attenuation).arg(num2str(ATT->freq));

        if (!SP_box)
        {// If the SP simulation box option is activated, then the input and output ports are attached.
         // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QStringLiteral("<Text 50 -30 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QStringLiteral("<Text 390 -30 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
        }
        *s += "</Paintings>\n";
        break;
      case QW_SHUNT_TYPE:
        if (ATT->useLumped)
        {
           double w = 2*PI*ATT->freq;
           *s += QStringLiteral("<L L1 1 200 60 20 -35 0 1 \"%1H\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(num2str(ATT->Zin/w));
           *s += QStringLiteral("<C C1 1 200 -60 -90 -20 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
           *s += "<GND * 1 200 -90 0 0 1 0>\n";
           *s += QStringLiteral("<C C1 1 320 150 0 60 0 1 \"%1F\" 1 \"\" 0 \"neutral\" 0>\n").arg(num2str(1/(ATT->Zin*w)));
           *s += "<GND * 1 320 180 0 0 0 0>\n";
        }
        else
        {
           *s += QStringLiteral("<TLIN Line1 1 200 60 20 -35 0 1 \"%1 Ohm\" 1 \"%2 mm\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(ATT->Zin).arg(QString::number(ATT->L*1e3, 'f', 1));
        }
        *s += QStringLiteral("<R R1 1 160 150 -40 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += "<GND * 1 160 180 0 0 0 0>\n";
        *s += QStringLiteral("<R R1 1 240 150 -20 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(ATT->Zin);
        *s += "<GND * 1 240 180 0 0 0 0>\n";
        *s += QStringLiteral("<R R1 1 300 0 -30 -60 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));

        if (SP_box)
        {
           // S-parameter simulation block
           //-----------------------------
           // The quarter-wave line is a narrowband device... so let's set the SP sweep from f0/2 to 3*f0/2
           QString freq_start = QStringLiteral("%1").arg(0.5*ATT->freq*1e-6);//MHz
           QString freq_stop = QStringLiteral("%1").arg(1.5*ATT->freq*1e-6);//MHz
           *s += QStringLiteral("<.SP SP1 1 100 270 0 83 0 0 \"lin\" 1 \"%1 MHz\" 1 \"%2 MHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n").arg(freq_start).arg(freq_stop);

           // Equations
           *s += "<Eqn Eqn1 1 320 270 -32 19 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n";

           // Input term
           *s += QStringLiteral("<Pac P1 1 0 150 -100 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
           *s += "<GND * 1 0 180 0 0 0 0>\n";

           // Output term
           *s += QStringLiteral("<Pac P1 1 500 150 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
           *s += "<GND * 1 500 180 0 0 0 0>\n";
        }
        *s += "</Components>\n";
        *s += "<Wires>\n";
        *s += "<160 120 160 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line
        *s += "<160 105 200 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line

        *s += "<240 120 240 105 \"\" 0 0 0 \"\">\n";//Second resistor to qw line
        *s += "<240 105 200 105 \"\" 0 0 0 \"\">\n";//First resistor to qw line

        *s += "<200 105 200 90 \"\" 0 0 0 \"\">\n";//Connect the previous wires to the line

        *s += "<200 30 200 0 \"\" 0 0 0 \"\">\n";//Connect qw line to main branch
        *s += "<200 0 270 0 \"\" 0 0 0 \"\">\n";//qw line to series resistor

        *s += "<120 0 200 0 \"\" 0 0 0 \"\">\n";//Input port
        *s += "<330 0 410 0 \"\" 0 0 0 \"\">\n";//Output port

        if (SP_box)
        {
            //Term 1 to input port
            *s += "<0 120 0 0 \"\" 0 0 0 \"\">\n";
            *s += "<0 0 120 0 \"\" 0 0 0 \"\">\n";

            //Term 2 to output port
            *s += "<500 120 500 0 \"\" 0 0 0 \"\">\n";
            *s += "<410 0 500 0 \"\" 0 0 0 \"\">\n";
        }

        if (ATT->useLumped)
        {//Add extra wiring to connect the shunt capacitors
              *s += "<320 120 320 105 \"\" 0 0 0 \"\">\n";
              *s += "<320 105 180 105 \"\" 0 0 0 \"\">\n";

              *s += "<200 -30 200 0 \"\" 0 0 0 \"\">\n";
        }

        *s += "</Wires>\n";
        *s += "<Diagrams>\n";
        *s += "</Diagrams>\n";
        *s += "<Paintings>\n";

        //Put the title a little bit higher because of the shunt cpa
        if (ATT->useLumped) *s += QStringLiteral("<Text 80 -140 12 #000000 0 \"%1 dB @ %2Hz Quarter-Wave shunt attenuator\">\n").arg(ATT->Attenuation).arg(num2str(ATT->freq));
        else *s += QStringLiteral("<Text 80 -120 12 #000000 0 \"%1 dB @ %2Hz Quarter-Wave shunt attenuator\">\n").arg(ATT->Attenuation).arg(num2str(ATT->freq));

        if (!SP_box)
        {// If the SP simulation box option is activated, then the input and output ports are attached.
         // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QStringLiteral("<Text 50 -30 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QStringLiteral("<Text 390 -30 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
        }
        *s += "</Paintings>\n";
        break;
      case L_PAD_1ST_SERIES:
          *s += QStringLiteral("<R R1 1 255 130 -35 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
          *s += QStringLiteral("<R R2 1 330 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
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
             *s += QStringLiteral("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
             *s += "<GND * 1 50 230 0 0 0 0>\n";

             // Output term
             *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
             *s += "<GND * 1 460 230 0 0 0 0>\n";
           }
          *s += "</Components>\n";
          *s += "<Wires>\n";
          *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
          *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";
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
          *s += QStringLiteral("<Text 160 60 12 #000000 0 \"%1 dB L-pad 1st Series Attenuator\">\n").arg(ATT->Attenuation);
          if (!SP_box)
             {// If the SP simulation box option is activated, then the input and output ports are attached.
              // Thus, it doesn't make sense to have a text field indicating the input/output impedance
                *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
                *s += QStringLiteral("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(QString::number(ATT->R3, 'f', 1));
             }
          *s += "</Paintings>\n";
          break;

      case L_PAD_1ST_SHUNT:
          *s += QStringLiteral("<R R1 1 180 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
          *s += QStringLiteral("<R R2 1 255 130 -35 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R2, 'f', 1));
          *s += "<GND * 1 180 230 0 0 0 0>\n";

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
             *s += QStringLiteral("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
             *s += "<GND * 1 50 230 0 0 0 0>\n";

             // Output term
             *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
             *s += "<GND * 1 460 230 0 0 0 0>\n";
           }
          *s += "</Components>\n";
          *s += "<Wires>\n";
          *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
          *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";
          *s += "<180 130 180 170 \"\" 0 0 0 \"\">\n";
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
          *s += QStringLiteral("<Text 160 60 12 #000000 0 \"%1 dB L-pad 1st Shunt Attenuator\">\n").arg(ATT->Attenuation);
          if (!SP_box)
             {// If the SP simulation box option is activated, then the input and output ports are attached.
              // Thus, it doesn't make sense to have a text field indicating the input/output impedance
                *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
                *s += QStringLiteral("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(QString::number(ATT->R3, 'f', 1));
             }
          *s += "</Paintings>\n";
      break;
     case R_SERIES:
      *s += QStringLiteral("<R R1 1 255 130 -35 -45 0 0 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
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
         *s += QStringLiteral("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
         *s += "<GND * 1 50 230 0 0 0 0>\n";

         // Output term
         *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
         *s += "<GND * 1 460 230 0 0 0 0>\n";
       }
      *s += "</Components>\n";
      *s += "<Wires>\n";
      *s += "<285 130 350 130 \"\" 0 0 0 \"\">\n";
      *s += "<140 130 225 130 \"\" 0 0 0 \"\">\n";

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
      *s += QStringLiteral("<Text 160 60 12 #000000 0 \"%1 dB R Series Attenuator\">\n").arg(ATT->Attenuation);
      if (!SP_box)
         {// If the SP simulation box option is activated, then the input and output ports are attached.
          // Thus, it doesn't make sense to have a text field indicating the input/output impedance
            *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
            *s += QStringLiteral("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
         }
      *s += "</Paintings>\n";
      break;

      case R_SHUNT:

        *s += QStringLiteral("<R R1 1 250 200 -15 60 0 1 \"%1 Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(ATT->R1, 'f', 1));
        *s += "<GND * 1 250 230 0 0 0 0>\n";
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
           *s += QStringLiteral("<Pac P1 1 50 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zin);
           *s += "<GND * 1 50 230 0 0 0 0>\n";

           // Output term
           *s += QStringLiteral("<Pac P1 1 460 200 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n").arg(ATT->Zout);
           *s += "<GND * 1 460 230 0 0 0 0>\n";
         }
        *s += "</Components>\n";
        *s += "<Wires>\n";
        *s += "<140 130 350 130 \"\" 0 0 0 \"\">\n";
        *s += "<250 130 250 170 \"\" 0 0 0 \"\">\n";
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
        *s += QStringLiteral("<Text 160 60 12 #000000 0 \"%1 dB Shunt R Attenuator\">\n").arg(ATT->Attenuation);
        if (!SP_box)
           {// If the SP simulation box option is activated, then the input and output ports are attached.
            // Thus, it doesn't make sense to have a text field indicating the input/output impedance
              *s += QStringLiteral("<Text 50 122 10 #000000 0 \"Z1: %1 Ohm\">\n").arg(ATT->Zin);
              *s += QStringLiteral("<Text 360 122 10 #000000 0 \"Z2: %1 Ohm\">\n").arg(ATT->Zout);
           }
        *s += "</Paintings>\n";
        break;
    }

  return s;
}

//COPIED FROM QUCS-POWERCOMBINING TOOL
// Converts a double number into string adding the corresponding prefix
QString num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = QString::number(Num, 'f', 1);
  if(c)  Str += c;

  return Str;
}
