/// @file schematicvalidator.h
/// @brief Schematic validation class (definition)
/// @author Andrés Martínez Mera
/// @date July 12, 2026

#ifndef SCHEMATICVALIDATOR_H
#define SCHEMATICVALIDATOR_H

#include <QString>
#include <QVector>

class Schematic;

/// @brief A single problem found in a schematic
/// @param message Human-readable description for the user
/// @param severity Relevance of the issue
///        1: Critical - The simulation backend will fail
///        2: Warning  - Simulation may proceed, but care must be taken.
///        3: Minor    - Not relevant
/// @param suggestedFix Message containing with a suggestion about how to solve the problem
struct ValidationIssue
{
  QString message;
  QString suggestedFix;
  int severity;
};

/// @class SchematicValidator
/// @brief Identify issues in an schematic given a simulation backend.
/// @details  Validation runs before the simulation is started, so the user
/// is warned about potential incompatibilities
class SchematicValidator
{
public:
  /// @brief Run all checks against an schematic given a certain simulation backend,
  /// e.g. qucsator-RF, ngspice, xyce
  QVector<ValidationIssue> validate(Schematic *sch, const QString &backend) const;

private:

  /// Checks @{

  /// @brief SP/AC frequency sweep must not be a list if the simulation backend is ngspice or xyce
  QVector<ValidationIssue> checkFrequencySweepType(Schematic *sch, const QString &backend) const;

  /// @}
};

#endif // SCHEMATICVALIDATOR_H
