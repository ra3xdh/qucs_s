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
#include "component.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QMutableListIterator>
#include <QRegularExpression>

#include <iostream>

CdlNetlistWriter::CdlNetlistWriter(
        QTextStream& netlistStream,
        Schematic* schematic,
        bool resolveSpicePrefix) :
    a_netlistStream(netlistStream),
    a_schematic(schematic),
    a_resolveSpicePrefix(resolveSpicePrefix),
    a_netListString(),
    a_netListStringStream(&a_netListString, QIODeviceBase::WriteOnly),
    a_effectiveNetlistStream(resolveSpicePrefix ? a_netListStringStream : a_netlistStream)
{
    if (a_resolveSpicePrefix)
    {
        // reserve space for a netlist of 100000 line of 80 characters each
        a_netListString.reserve(100000 * 80);
    }
}

bool CdlNetlistWriter::write()
{
    a_effectiveNetlistStream << "* Qucs " << PACKAGE_VERSION << "  " << a_schematic->getDocName() << "\n";

    a_effectiveNetlistStream << AbstractSpiceKernel::collectSpiceLibs(a_schematic);
    QString s(AbstractSpiceKernel::collectSpiceLibs(a_schematic));
    a_effectiveNetlistStream << s;

    if (prepareNetlist() == -10)
    {
        return false;
    }

    startNetlist();

    a_effectiveNetlistStream << ".END\n";

    if (a_resolveSpicePrefix)
    {
        a_effectiveNetlistStream.flush();
        resolveSpicePrefix();

        a_netlistStream << a_netListString;
    }

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
    for (Component *pc : a_schematic->a_DocComps)
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

    if (!a_schematic->giveNodeNames(&a_effectiveNetlistStream, countInit, collect, &errorText, numPorts))
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
    for (Component *pc : a_schematic->a_DocComps)
    {
        if (pc->isEquation)
        {
            s = pc->getExpression(spicecompat::CDL);
            a_effectiveNetlistStream << s;
        }
    }

    // global net 0 is always ground
    a_effectiveNetlistStream << ".GLOBAL 0:G\n";

    // Components
    for (Component *pc : a_schematic->a_DocComps)
    {
        if (a_schematic->getIsAnalog() && !pc->isSimulation && !pc->isEquation)
        {
            s = pc->getSpiceNetlist(spicecompat::CDL);

            a_effectiveNetlistStream << s;
        }
    }

    //TODO: Clarification of whether models should be exported
    // Modelcards
    /*for (Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next())
    {
        if (pc->SpiceModel==".MODEL")
        {
            s = pc->getSpiceModel();
            a_effectiveNetlistStream << s;
        }
    }*/
}

void CdlNetlistWriter::resolveNetListContinuation(QStringList& netList)
{
    QMutableListIterator<QString> it(netList);
    while (it.hasNext())
    {
        it.next();
        if (it.hasNext())
        {
            QString line(QString(it.peekNext()).trimmed());
            if (!line.isEmpty() && line[0] == '+')
            {
                line[0] = ' ';
                it.value() += line;
                it.next();
                it.remove();
                it.previous();
            }
        }
    }
}

void CdlNetlistWriter::resolveSpicePrefix()
{
#if 0 // several test-cases for resolve spice prefix
    a_netListString = QString::fromUtf8(

"* Qucs 25.1.1  /home/herman/.qucs/CDL-Export-testcases_prj/cmim.sch\n"
"\n"
".SUBCKT IHP_PDK_nonlinear_components_diodevss_2kv  gnd PVdd Pad PVss m=1\n"
"      XD12 PVdd Pad PVss diodevss_2kv m={m}\n"
".ENDS\n"
"  \n"
"\n"
"\n"
"Xdiodevss_2kv1 0  net_vdd net_pin net_vss IHP_PDK_nonlinear_components_diodevss_2kv m=1\n"
"Xdiodevss_2kv2 0  net_vdd net_pin IHP_PDK_nonlinear_components_diodevss_2kv m=1\n"
"Xdiodevss_2kv3 0  net_vdd net_pin net_vss\n"
"+ IHP_PDK_nonlinear_components_diodevss_2kv m=1\n"
"\n"
".GLOBAL 0:G\n"
"\n"
"\n"
"XC1 net_top  net_bottom cmim w=7.0u l=7.0u m=1\n"
"XD1 net_top  net_bottom dantenna w=0.78u l=0.78u m=1\n"
"XM1 net_d  net_g  net_s  net_b sg13_hv_nmos w=0.13u l=0.13u ng=1 m=1ßn\n"
"M2 net_d  net_g  net_s  net_b sg13_hv_nmos w=0.13u l=0.13u ng=1 m=1ßn\n"
"XQ1 net_col  net_base  net_emi  bet_bulk npn13G2 Nx=1\n"
"XR1 net_bottom  net_top rsil w=1u l=1u m=1\n"
".END"
    );
#endif


    Q_ASSERT(!a_netListString.isEmpty());

    QStringList netList(a_netListString.split("\n"));

    resolveNetListContinuation(netList);

    // acquire subcircuits and respective number of ports
    QRegularExpression subcktPattern(
            QString::fromUtf8("^\\s*((\\.SUBCKT.*)|(\\.SUBCIRCUIT.*)|(\\.MACRO.*))"),
            QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(subcktPattern.isValid());

    QMap<QString, bool> subcktsWithPins;

    int from(0);
    int idx;
    while ((idx = netList.indexOf(subcktPattern, from)) != -1)
    {
        netList[idx].replace("=", " = ");
        netList[idx].replace("/", " / ");
        QStringList subcktComponents(netList[idx].split(' ', Qt::SkipEmptyParts));
        uint32_t numPins(subcktComponents.size()-2);

        int firstParamEqualSign(subcktComponents.indexOf("="));
        if (firstParamEqualSign != -1)
        {
            numPins -= (subcktComponents.size() - firstParamEqualSign + 1);
        }

        if (subcktComponents.indexOf("/") != -1)
        {
            --numPins;
        }

        const QString subcktKey(QString::fromUtf8("%1_%2").arg(subcktComponents[1]).arg(numPins));
        subcktsWithPins[subcktKey] = true;
        qDebug() << "map subcircuit key: " << subcktKey;

        from = idx+1;
    }


    // acquire subcircuit instances and respective number of ports
    QRegularExpression subcktInstPattern(
            QString::fromUtf8("^\\s*X(C|D|L|R|Q|J|M|I|V).*"),
            QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(subcktInstPattern.isValid());

    from = 0;
    while ((idx = netList.indexOf(subcktInstPattern, from)) != -1)
    {
        netList[idx].replace("=", " = ");
        netList[idx].replace("/", " / ");
        QStringList subcktInstComponents(netList[idx].split(' ', Qt::SkipEmptyParts));
        uint32_t numPins(subcktInstComponents.size()-2);

        int firstParamEqualSign(subcktInstComponents.indexOf("="));
        if (firstParamEqualSign != -1)
        {
            numPins -= (subcktInstComponents.size() - firstParamEqualSign + 1);
        }
        uint32_t modelIdx(numPins+1);

        if (subcktInstComponents.indexOf("/") != -1)
        {
            --numPins;
        }

        const QString subcktInstKey(QString::fromUtf8("%1_%2").arg(subcktInstComponents[modelIdx]).arg(numPins));
        // check whether referenced subcircuit is present in current netlist
        if (!subcktsWithPins.contains(subcktInstKey))
        {
            qDebug() << "remove spice prefix from: " << subcktInstKey;
            a_netListString.replace(
                    subcktInstComponents[0],
                    subcktInstComponents[0].right(subcktInstComponents[0].size()-1));
        }

        from = idx+1;
    }
}

