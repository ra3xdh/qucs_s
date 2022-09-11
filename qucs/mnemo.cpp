/***************************************************************************
                               mnemo.cpp
                              -----------
    begin                : Sat Jun 11 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mnemo.h"
#include <QString>

struct tSpecialChar {
     char Mnemonic[16];
     unsigned short Unicode;
};


struct tSpecialChar SpecialChars[] = {
     {"alpha", 0x03B1}, {"beta", 0x03B2}, {"gamma", 0x03B3},
     {"delta", 0x03B4}, {"epsilon", 0x03B5}, {"zeta", 0x03B6},
     {"eta", 0x03B7}, {"theta", 0x03B8}, {"iota", 0x03B9},
     {"kappa", 0x03BA}, {"lambda", 0x03BB}, {"mu", 0x03BC},
     {"nu", 0x03BD}, {"xi", 0x03BE}, {"pi", 0x03C0},
     {"rho", 0x03C1}, {"sigma", 0x03C3}, {"tau", 0x03C4},
     {"upsilon", 0x03C5}, {"phi", 0x03C6}, {"chi", 0x03C7},
     {"psi", 0x03C8}, {"omega", 0x03C9},

     {"varpi", 0x03D6}, {"varrho", 0x03F1},

     {"Gamma", 0x0393}, {"Delta", 0x0394}, {"Theta", 0x0398},
     {"Lambda", 0x039B}, {"Xi", 0x039E}, {"Pi", 0x03A0},
     {"Sigma", 0x03A3}, {"Upsilon", 0x03A5}, {"Phi", 0x03A6},
     {"Psi", 0x03A8}, {"Omega", 0x03A9},

     {"textmu", 0x00B5}, {"cdot", 0x00B7}, {"times", 0x00D7},
     {"pm", 0x00B1}, {"mp", 0x2213}, {"partial", 0x2202},
     {"nabla", 0x2207}, {"infty", 0x221E}, {"int", 0x222B},
     {"approx", 0x2248}, {"neq", 0x2260}, {"in", 0x220A},
     {"leq", 0x2264}, {"geq", 0x2265}, {"sim", 0x223C},
     {"propto", 0x221D}, {"onehalf", 0x00BD}, {"onequarter", 0x00BC},
     {"twosuperior", 0x00B2}, {"threesuperior", 0x00B3},
     {"diameter", 0x00F8}, {"ohm", 0x03A9},

     {"", 0}  // end mark
};


// This function replaces the LaTeX tags for special characters
// into its unicode value.
void encode_String(const QString& Input, QString& Output)
{
  int Begin = 0, End = 0;
  struct tSpecialChar *p;

  Output = "";
  while((Begin=Input.indexOf('\\', Begin)) >= 0) {
    Output += Input.mid(End, Begin - End);
    End = Begin++;

    p = SpecialChars;
    while(p->Unicode != 0)   // test all special characters
      if(Input.mid(Begin).startsWith(p->Mnemonic)) {
        Output += QChar(p->Unicode);
        End = Begin + strlen(p->Mnemonic);
        break;
      }
      else p++;
  }
  Output += Input.mid(End);
}

// This function replaces the unicode of special characters
// by its LaTeX tags.
void decode_String(QString& Output)
{
  struct tSpecialChar *p = SpecialChars;
  while(p->Unicode != 0) {   // test all special characters
    Output.replace(QChar(p->Unicode), "\\"+QString(p->Mnemonic));
    p++;
  }
}
