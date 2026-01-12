/// @file SchematicContent.h
/// @brief Container for schematic circuit data (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef SCHEMATICCONTENT_H
#define SCHEMATICCONTENT_H

#include <QGraphicsTextItem>
#include <QPen>
#include <QRegularExpression>
#include <QMessageBox> // Needed to show warnings during export process
#include <utility> // std::as_const()

#include "../Misc/general.h"
#include "infoclasses.h"
#include "structures.h"

#include "Export/QucsS/QucsSExporter.h"

class Component;

/// @class SchematicContent
/// @brief Container for schematic circuit data
class SchematicContent {
    public:
        /// @brief Class constructor
        SchematicContent();

        /// @brief Class destructor
        ~SchematicContent(){}

        /// @brief Generate netlist for the built-in S-parameter simulator
        /// @return Netlist
        QString getSParameterNetlist();

        /// @brief Set netlist directly
        /// @param Netlist string
        void setNetlist(QString netlist) { this->netlist = netlist; }

        QString Name;                          ///< Schematic name
        QString Type;                          ///< Schematic type (filter, attenuator, etc.)

        /// Schematic elements
        QList<ComponentInfo> Comps;            ///< Component list
        QList<WireInfo> Wires;                 ///< Wire list
        QList<NodeInfo> Nodes;                 ///< Node list

        /// @brief Export schematic to specified environment (so far, Qucs-S only)
        /// @param environment Target environment (e.g., "Qucs-S")
        /// @param backend Backend simulator
        /// @return Exported schematic string
        QString exportSchematic(QString environment, QString backend);


        /// @brief Convert schematic to Qucs-S format
        /// @param backend Backend simulator
        /// @return Qucs-S formatted schematic
        QString export2QucsS(QString); // Convert the schematic content to Qucs-S format

        /// @brief Set frequency sweep parameters for export
        /// @param f1 Start frequency
        /// @param f2 Stop frequency
        /// @param N Number of points
        void setFrequencySweep(QString f1, QString f2, int N) {
          f_start = f1;
          f_stop = f2;
          n_points = N;
        }

        /// Frequency sweep settings (required for exporting)
        QString f_start, f_stop;               ///< Frequency sweep start and stop
        int n_points;                          ///< Number of frequency points

    private:
        /// @brief Assign net to all wires connected to a node
        /// @param NodeID Node identifier
        /// @param net Net name to assign
        void assignNetToWiresConnectedToNode(QString, QString);

    public:
        /// @name Component, wire and node management
        /// @{

        /// @brief Add component to schematic
        /// @param C Component information
        void appendComponent(struct ComponentInfo C) {Comps.append(C);}

        /// @brief Add wire to schematic
        /// @param WI Wire information
        void appendWire(WireInfo WI);

        /// @brief Add wire with connection details
        /// @param O Origin ID
        /// @param ON Origin port
        /// @param D Destination ID
        /// @param DN Destination port
        void appendWire(QString O, int ON, QString D, int DN);

        /// @brief Add wire with connection details and color
        /// @param O Origin ID
        /// @param ON Origin port
        /// @param D Destination ID
        /// @param DN Destination port
        /// @param c Wire color
        void appendWire(QString O, int ON, QString D, int DN, QColor c);

        /// @brief Add node to schematic
        /// @param N Node information
        void appendNode(struct NodeInfo N){ Nodes.append(N); }

        /// @brief Add text annotation
        /// @param text Text item
        void appendText(QGraphicsTextItem* text) {Texts.append(text);}
        /// @}

        /// @name Impedance accessors
        /// @{
        /// @brief Get input impedance value
        /// @return Input impedance as double
        double getZin() { return Comps[0].val["Z"].toDouble(); }

        /// @brief Get output impedance value
        /// @return Output impedance as double
        double getZout() {
          return Comps[Comps.size() - 1].val["Z"].toDouble();
        }

        /// @brief Get input impedance string
        /// @return Input impedance string
        QString getZinString() { return Comps[0].val["Z"]; }

        ///
        /// @brief Get output impedance string
        /// @return Output impedance string
        ///
        QString getZoutString(){
          return Comps[Comps.size() - 1].val["Z"];
        }
        /// @}

        /// @brief Get component list
        /// @return List of components
        QList<ComponentInfo> getComponents() { return Comps; }

        /// @brief Set component list
        /// @param C Component list
        void setComponents(QList<ComponentInfo> C) { Comps = C; }

        /// @brief Get wire list
        /// @return List of wires
        QList<WireInfo> getWires() { return Wires; }

        /// @brief Get node list
        /// @return List of nodes
        QList<NodeInfo> getNodes() { return Nodes; }

        /// @brief Set node list
        /// @param N Node list
        void setNodes(QList<NodeInfo> N) { Nodes = N; }

        /// @brief Get text list
        /// @return List of text items
        QList<QGraphicsTextItem*> getTexts() { return Texts; }

        /// @brief Get count of specific component type
        /// @param Comp Component type
        /// @return Number of components of that type
        int getComponentCounter(ComponentType);


        QMap<ComponentType, int> NumberComponents;  ///< Number of elements of each component type (#caps, #res, etc.)
        unsigned int NumberWires;                   ///< Total wire count

    private:
        QList<QGraphicsTextItem*> Texts;       ///< Text annotations
        QString Description;                   ///< Schematic description
        QString netlist;                       ///< Generated netlist
};

#endif // SCHEMATICCONTENT_H
