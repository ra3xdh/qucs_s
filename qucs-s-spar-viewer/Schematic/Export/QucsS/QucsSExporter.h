/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef QUCSSEXPORTER_H
#define QUCSSEXPORTER_H

#include "../SchematicExporter.h"

// This is needed for catching the current Qucs-S version
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


class QucsSExporter : public SchematicExporter {
public:
    explicit QucsSExporter(SchematicContent& owner);
    ~QucsSExporter();

    void setSchematiContent(const SchematicContent& schematic);

    QString exportSchematic();
    QString backend_simulator;


private:
    SchematicContent& schematic;

private:
        // These variables apply a general offset in the Qucs-S schematic export process
        int x_offset, y_offset;

        // General schematic scaling versus the tool's schematic
        int scale_x, scale_y;

        // Component processing
        QString processComponents_QucsS(QString);
        QString parseTerm_QucsS(ComponentInfo);
        QString parseResistor_QucsS(ComponentInfo);
        QString parseInductor_QucsS(ComponentInfo);
        QString parseCapacitor_QucsS(ComponentInfo);
        QString parseGND_QucsS(ComponentInfo);
        QString parseIdealTransmissionLine_QucsS(ComponentInfo);
        QString parseIdealCoupledTransmissionLines_QucsS(ComponentInfo);
        QString parseShortStub_QucsS(ComponentInfo);
        QString parseIdealCoupler_QucsS(ComponentInfo);
        QString parseComplexImpedance_QucsS(ComponentInfo, double);
        QString parseSPAR_Block_QucsS(ComponentInfo);
        
                // Microstrip components
        MS_Substrate get_MS_Substrate(ComponentInfo Comp);
        QString addSubstrateBox(QList<MS_Substrate> subs_list, int x_bottom, int y_bottom);
        QString parseMicrostripLine_QucsS(ComponentInfo);
        QString parseMicrostripCoupledLines_QucsS(ComponentInfo);
        QString parseMicrostripStep_QucsS(ComponentInfo);
        QString parseMicrostripOpen_QucsS(ComponentInfo);
        QString parseMicrostripVia_QucsS(ComponentInfo);
        
               // Wire processing
        QString processWires_QucsS();

        // Process internal nodes
        void processNodes_QucsS();

        // Export blacklist
        // List of components not supported by the backenb simulator (Qucsator, NGspice, Xyce)
        QMap<QString, QList<ComponentType>> Export_Blacklists;
};

#endif // QUCSSEXPORTER_H
