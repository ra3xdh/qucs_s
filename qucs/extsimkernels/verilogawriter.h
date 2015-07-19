#ifndef VERILOGAWRITER_H
#define VERILOGAWRITER_H

#include <QtCore>
#include <schematic.h>

class VerilogAwriter
{

public:
    VerilogAwriter();
    ~VerilogAwriter();
protected:
    bool prepareToVerilogA(Schematic *sch);
public:
    bool createVA_module(QTextStream &stream, Schematic *sch);
    
};

#endif // VERILOGAWRITER_H
