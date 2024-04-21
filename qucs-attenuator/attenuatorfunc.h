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
  double R4;
  double PR1;//Power dissipated by R1 [W]
  double PR2;//Power dissipated by R2 [W]
  double PR3;//Power dissipated by R3 [W]
  double PR4;//Power dissipated by R4 [W]
  double Pin;//Input power [W]
  bool minR; //The reflection attenuator can be designed using two different resistor values. The first one is such
             //as R < Z0 whereas the other is such as R > Z0. This field is just a flag to indicate what solution
             //qucsattenuator should use.
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
