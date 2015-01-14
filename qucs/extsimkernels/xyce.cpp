#include "xyce.h"

Xyce::Xyce(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, AbstractSpiceKernel::Xyce, parent)
{
}

void Xyce::createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                    QStringList &vars, QStringList &outputs)
{
    if(!Sch->prepareSpiceNetlist(stream)) return; // Unable to perform ngspice simulation

    QString s;
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
      if(Sch->isAnalog &&
         !(pc->isSimulation) &&
         !(pc->isProbe)) {
        s = pc->getSpiceNetlist(true);
        stream<<s;
      }
    }

    // determine which simulations are in use
    simulations.clear();
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           s = pc->getSpiceNetlist(true);
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulations.append("ac");
           if (sim_typ==".TR") simulations.append("tran");
           //if (sim_typ==".DC") simulations.append("dc");
           stream<<s;
       }
    }

    // set variable names for named nodes and wires
    vars.clear();
    for(Node *pn = Sch->DocNodes.first(); pn != 0; pn = Sch->DocNodes.next()) {
      if(pn->Label != 0) {
          if (!vars.contains(pn->Label->Name)) {
              vars.append(pn->Label->Name);
          }
      }
    }
    for(Wire *pw = Sch->DocWires.first(); pw != 0; pw = Sch->DocWires.next()) {
      if(pw->Label != 0) {
          if (!vars.contains(pw->Label->Name)) {
              vars.append(pw->Label->Name);
          }
      }
    }
    vars.sort();
    qDebug()<<vars;

    //execute simulations

    QFileInfo inf(Sch->DocName);
    QString basenam = inf.baseName();

    QString sim;                 // see results
    outputs.clear();
    foreach (sim,simulations) {
        QString nod,nods;
        nods.clear();
        foreach (nod,vars) {
            nods += QString("v(%1) ").arg(nod);
        }
        QString filename = QString("%1_%2.txt").arg(basenam).arg(sim);
        QString write_str = QString(".PRINT  %1 format=raw filename=%2\n").arg(sim).arg(filename).arg(nods);
        stream<<write_str;
        outputs.append(filename);
        stream<<endl;
    }

    stream<<".END\n";
}

void Xyce::slotSimulate()
{

}
