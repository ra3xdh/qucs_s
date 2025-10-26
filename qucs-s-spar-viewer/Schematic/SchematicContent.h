/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#ifndef SCHEMATICCONTENT_H
#define SCHEMATICCONTENT_H

#include <QGraphicsTextItem>
#include <QPen>
#include <QRegularExpression>

#include "../Misc/general.h"
#include "infoclasses.h"
#include "structures.h"

class Component;
class SchematicContent {
    public:
        SchematicContent();
        ~SchematicContent();

        QString getSParameterNetlist();
        void setNetlist(QString);
        QString Name;
        QString Type;

        QList<ComponentInfo> Comps;
        QList<WireInfo> Wires;
        QList<NodeInfo> Nodes;

        QString export2QucsS(); // Convert the schematic content to Qucs-S format

        void setFrequencySweep(QString, QString, int);

    private:
        void assignNetToWiresConnectedToNode(QString, QString);

        // Frequency sweep settings (required for exporting)
        QString f_start, f_stop;
        int n_points;

    public:
        // Setter getter functions

        // Components, wires and nodes
        void appendComponent(ComponentInfo);
        void appendWire(WireInfo);
        void appendWire(QString, int, QString, int);
        void appendWire(QString, int, QString, int, QColor);
        void appendNode(NodeInfo);
        void appendText(QGraphicsTextItem* text);

        double getZin();
        double getZout();
        QString getZinString();
        QString getZoutString();

        QList<ComponentInfo> getComponents();
        void setComponents(QList<ComponentInfo> C);

        QList<WireInfo> getWires();
        QList<NodeInfo> getNodes();
        void setNodes(QList<NodeInfo> N);
        QList<QGraphicsTextItem*> getTexts();

        QMap<ComponentType, int>
            NumberComponents; // List for assigning IDs to the filter components
        unsigned int NumberWires;

    private:
        QList<QGraphicsTextItem*> Texts;
        QString Description;
        QString netlist;

    private:
        // Qucs-S parsing functions

        // Component processing
        int scale_x_QucsS_export, scale_y_QucsS_export;
        QMap<QString, QList<QPoint>> ComponentPinMap; // Keep information of the pins position of a component
        QString processComponents_QucsS();
        QString parseTerm_QucsS(ComponentInfo);
        QString parseResistor_QucsS(ComponentInfo);
        QString parseInductor_QucsS(ComponentInfo);
        QString parseCapacitor_QucsS(ComponentInfo);
        QString parseGND_QucsS(ComponentInfo);
        QString parseIdealTransmissionLine_QucsS(ComponentInfo);

        // Microstrip components
        QString parseMicrostripLine_QucsS(ComponentInfo);
        QString parseMicrostripStep_QucsS(ComponentInfo);

        // Wire processing
        QString processWires_QucsS();

        // Process internal nodes
        void processNodes_QucsS();
};

#endif // SCHEMATICCONTENT_H
