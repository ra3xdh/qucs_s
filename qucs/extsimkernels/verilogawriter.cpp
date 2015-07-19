#include "verilogawriter.h"
#include <QPlainTextEdit>

VerilogAwriter::VerilogAwriter()
{
}

VerilogAwriter::~VerilogAwriter()
{

}

bool VerilogAwriter::prepareToVerilogA(Schematic *sch)
{
    QStringList collect;
    QPlainTextEdit *err = new QPlainTextEdit;
    QTextStream stream;
    if (sch->prepareNetlist(stream,collect,err)==-10) { // Broken netlist
        delete err;
        return false;
    }
    delete err;
    sch->clearSignalsAndFileList(); // for proper build of subckts
    return true;
}

bool VerilogAwriter::createVA_module(QTextStream &stream, Schematic *sch)
{
    prepareToVerilogA(sch);

    QString s;
    stream<<"`include \"disciplines.vams\"\n";
    stream<<"`include \"constants.vams\"\n";

    QStringList ports;
    QStringList nodes;
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->Model=="Port") {
            QString s = pc->Ports.first()->Connection->Name;
            if (!ports.contains(s)) ports.append(s);
        } else {
            foreach(Port *pp,pc->Ports) {
                QString s = pp->Connection->Name;
                if (!nodes.contains(s)) nodes.append(s);
            }
        }
    }



    stream<<"analog begin \n"
            "@(initial model)\n"
            "begin \n";
    stream<<"end\n";

     for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
         stream<<pc->getVerilogACode();
     }

    stream<<"end\n"
            "endmodule\n";
    return true;
}
