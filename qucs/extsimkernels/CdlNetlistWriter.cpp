/*
 * CdlNetlistWriter.cpp - Schematic to CDL netlist converter implementation
 *
 * This file is part of Qucs-s
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "CdlNetlistWriter.h"
#include "abstractspicekernel.h"

#include "schematic.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QTextStream>

CdlNetlistWriter::CdlNetlistWriter(QTextStream& netlistStream, Schematic* schematic) :
    a_netlistStream(netlistStream),
    a_schematic(schematic)
{
}

bool CdlNetlistWriter::write()
{
    a_netlistStream << "* Qucs " << PACKAGE_VERSION << "  " << a_schematic->getDocName() << "\n";

    a_netlistStream << AbstractSpiceKernel::collectSpiceLibs(a_schematic);

    if (prepareNetlist() == -10)
    {
        return false;
    }

    startNetlist();

    a_netlistStream << ".END\n";

    return true;
}

int CdlNetlistWriter::prepareNetlist()
{
    if (a_schematic->getShowBias() > 0)
    {
        a_schematic->setShowBias(-1);  // do not show DC bias anymore
    }

    a_schematic->setIsVerilog(false);
    a_schematic->setIsAnalog(true);

    bool isTruthTable = false;
    int allTypes = 0;
    int numPorts = 0;

    // Detect simulation domain (analog/digital) by looking at component types.
    for (Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next())
    {
        if (pc->isActive == COMP_IS_OPEN)
        {
            continue;
        }
        if (pc->Model.at(0) == '.')
        {
            if (pc->Model == ".Digi")
            {
                if (pc->Props.front()->Value != "TimeList")
                {
                    isTruthTable = true;
                }
                if (pc->Props.back()->Value != "VHDL")
                {
                    a_schematic->setIsVerilog(true);
                }

                allTypes |= isDigitalComponent;
                a_schematic->setIsAnalog(false);
            }
            else
            {
                allTypes |= isAnalogComponent;
            }
        }
        else if (pc->Model == "DigiSource")
        {
            numPorts++;
        }
    }

    if ((allTypes & isAnalogComponent) == 0)
    {
        if (allTypes == 0)
        {
            // If no simulation exists, assume analog simulation. There may
            // be a simulation within a SPICE file. Otherwise Qucsator will
            // output an error.
            a_schematic->setIsAnalog(true);
            allTypes |= isAnalogComponent;
            numPorts = -1;
        }
        else
        {
            if (!isTruthTable)
            {
                numPorts = 0;
            }
        }
    }
    else
    {
        numPorts = -1;
        a_schematic->setIsAnalog(true);
    }

    int countInit = 0;  // counts the nodesets to give them unique names

    QStringList collect;
    QPlainTextEdit errorText;

    if (!a_schematic->giveNodeNames(&a_netlistStream, countInit, collect, &errorText, numPorts))
    {
        fprintf(stderr, "Error giving NodeNames\n");
        return -10;
    }

    return numPorts;
}

void CdlNetlistWriter::startNetlist()
{
    QString s;

    // Parameters, Initial conditions, Options
    for (Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next())
    {
        if (pc->isEquation)
        {
            s = pc->getExpression(false, true);
            a_netlistStream << s;
        }
    }

    // global net 0 is always ground
    a_netlistStream << ".GLOBAL 0:G\n";

    // Components
    for (Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next())
    {
        if (a_schematic->getIsAnalog() && !pc->isSimulation && !pc->isEquation)
        {
            s = pc->getSpiceNetlist(false, true);
            a_netlistStream << s;
        }
    }

    //TODO: Clarification of whether models should be exported
    // Modelcards
    /*for (Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next())
    {
        if (pc->SpiceModel==".MODEL")
        {
            s = pc->getSpiceModel();
            a_netlistStream << s;
        }
    }*/
}


