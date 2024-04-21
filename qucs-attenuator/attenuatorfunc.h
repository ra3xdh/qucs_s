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
#define REFLECTION_TYPE 3

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
  bool minR; //Only for the reflection attenuator
};

class QString;

class QUCS_Att
{
 public:
  QUCS_Att();
  ~QUCS_Att();

  int Calc(tagATT*);
  static QString* createSchematic(tagATT*, bool);


};

#endif
