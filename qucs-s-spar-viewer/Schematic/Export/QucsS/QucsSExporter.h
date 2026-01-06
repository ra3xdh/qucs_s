/// @file QucsSExporter.h
/// @brief Qucs-S schematic export class (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef QUCSSEXPORTER_H
#define QUCSSEXPORTER_H

#include "../SchematicExporter.h"

// This is needed for catching the current Qucs-S version
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/// @class QucsSExporter
/// @brief Exports schematic content to Qucs-S netlist format
///
/// This class serves as the base for all schematic export implementations.
///
/// It handles the conversion of internal schematic representations
/// to the Qucs-S schematic format. It processes components, wires, and nodes,
/// applying appropriate transformations and scaling. The exporter supports
/// multiple backend simulators (Qucsator, NGspice, Xyce) and maintains
/// compatibility blacklists for unsupported components per simulator.
///
class QucsSExporter : public SchematicExporter {
public:
    /// @brief Constructs a QucsSExporter with reference to schematic content
    /// @param owner Reference to the SchematicContent object to be exported
    explicit QucsSExporter(SchematicContent& owner);

    /// @brief Class destructor
    ~QucsSExporter() {}

    /// @brief Sets the schematic content to be exported
    /// @param schematic Reference to the SchematicContent object
    void setSchematiContent(const SchematicContent& schematic);

    /// @brief Exports the schematic to Qucs-S format
    /// @return QString containing complete Qucs-S netlist, or "-1" on error
    QString exportSchematic();

    /// @brief Name of the backend simulator (Qucsator, NGspice, or Xyce)
    QString backend_simulator;


private:
    /// @brief Reference to the schematic content being exported
    SchematicContent& schematic;

private:
        // These variables apply a general offset in the Qucs-S schematic export process
        int x_offset; ///< x-axis offset
        int y_offset; ///< y-axis offset

        // General schematic scaling versus the tool's schematic
        int scale_x; ///< x-axis scale factor
        int scale_y; ///< y-axis scale factor

        // Component processing

        /// @brief Processes all components and generates Qucs-S component definitions
        /// @param backend_simulator Name of the target simulator backend
        /// @return QString containing all component definitions, or "-1" if unsupported components found
        /// @details Iterates through all components, checks compatibility with backend,
        ///          calls appropriate parser for each component type, and generates
        ///          simulation boxes and substrate definitions
        QString processComponents_QucsS(QString);

        /// @name Component export methods
        /// @{
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

        /// @brief Extracts microstrip substrate properties from a component
        /// @param Comp ComponentInfo structure containing substrate parameters
        /// @return MS_Substrate structure with extracted properties (εr, h, tanδ, etc.)
        /// @details Reads substrate parameters like relative permittivity, height,
        ///          metal thickness, loss tangent, and conductivity from component data
        MS_Substrate get_MS_Substrate(ComponentInfo Comp);

        /// @brief Generates Qucs-S substrate definition boxes for all substrates
        /// @param subs_list List of all MS_Substrate definitions used in the design
        /// @param x_bottom X-coordinate for substrate box placement
        /// @param y_bottom Y-coordinate for substrate box placement
        /// @details Removes duplicate substrates and formats each unique substrate
        ///          with proper units (mm for height, μm for metal thickness)
        QString addSubstrateBox(QList<MS_Substrate> subs_list, int x_bottom, int y_bottom);
        QString parseMicrostripLine_QucsS(ComponentInfo);
        QString parseMicrostripCoupledLines_QucsS(ComponentInfo);
        QString parseMicrostripStep_QucsS(ComponentInfo);
        QString parseMicrostripOpen_QucsS(ComponentInfo);
        QString parseMicrostripVia_QucsS(ComponentInfo);
        /// @}
        
        /// @brief Processes all wires and generates Qucs-S wire definitions
        /// @return QString containing all wire segments in Qucs-S format
        /// @details Creates orthogonal (horizontal/vertical only) wire paths,
        ///          splitting diagonal connections into L-shaped segments.
        ///          Automatically skips ground connections that don't need routing.
        QString processWires_QucsS();

        /// @brief Processes internal nodes and records their positions
        /// @details Populates ComponentPinMap with node positions after applying
        ///          scaling factors.
        /// @note This will be needed later for tracing the wires
        void processNodes_QucsS();

        /// @brief Maps simulator backends to lists of unsupported component types
        /// @details Each backend simulator (NGspice, Xyce, Qucsator) has different
        ///          component incompatibilities. For example:
        ///          - NGspice: Cannot handle microstrip vias, steps, coupled lines
        ///          - Xyce: Cannot handle most microstrip components
        QMap<QString, QList<ComponentType>> Export_Blacklists;
};

#endif // QUCSSEXPORTER_H
