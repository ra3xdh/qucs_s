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
#include <QMessageBox> // Needed to show warnings during export process

#include "../Misc/general.h"
#include "infoclasses.h"
#include "structures.h"

#include "Export/QucsS/QucsSExporter.h"

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

        // Export schematic
        QString exportSchematic(QString environment, QString backend);

        QString export2QucsS(QString); // Convert the schematic content to Qucs-S format

        void setFrequencySweep(QString, QString, int);

        // Frequency sweep settings (required for exporting)
        QString f_start, f_stop;
        int n_points;

    private:
        void assignNetToWiresConnectedToNode(QString, QString);

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

        int getComponentCounter(ComponentType);
        QMap<ComponentType, int>
            NumberComponents; // Indicates how many components of each type the schematic are in teh schematic
        unsigned int NumberWires;

    private:
        QList<QGraphicsTextItem*> Texts;
        QString Description;
        QString netlist;
};

#endif // SCHEMATICCONTENT_H
