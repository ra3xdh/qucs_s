//-*- C++ -*-
/****************************************************************************
**     Qucs Attenuator Synthesis
**     attenuatorfunc.h
**
**     since 2006/6/14
**
**
**
**
**
*****************************************************************************/
#ifndef QUCS_ATT_H
#define QUCS_ATT_H

#define PI_TYPE 0
#define TEE_TYPE 1
#define BRIDGE_TYPE 2

#include <cmath>

struct tagATT
{
  int Topology;
  double Zin;
  double Zout;
  double Attenuation;
  double MinimumATT;
  double R1;
  double R2;
  double R3;
};

class QString;

class QUCS_Att
{
 public:
  QUCS_Att();
  ~QUCS_Att();

  int Calc(tagATT*);
  static QString* createSchematic(tagATT*);


};

#endif
