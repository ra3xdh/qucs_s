#include "src_eqndef.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


Src_eqndef::Src_eqndef()
{
  Description = QObject::tr("Equation defined (B-type) voltage or current source");

  Arcs.append(new Arc(-14,-14, 28, 28,     0, 16*360,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(7,-10,"Eqn",Qt::black,10.0,0.0,-1.0));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "B";
  SpiceModel = "B";
  Name  = "B";

  Props.append(new Property("V", "0", true,"Expression"));

  rotate();  // fix historical flaw
}

Src_eqndef::~Src_eqndef()
{
}

Component* Src_eqndef::newOne()
{
  return new Src_eqndef();
}

Element* Src_eqndef::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation defined (B-type) voltage or current source");
  BitmapFile = (char *) "src_eqndef";

  if(getNewOne)  return new Src_eqndef();
  return 0;
}

QString Src_eqndef::netlist()
{
    return QString("");
}

QString Src_eqndef::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }
    s += QString(" %1 = %2 \n").arg(Props.at(0)->Name).arg(Props.at(0)->Value);
    return s;
}
