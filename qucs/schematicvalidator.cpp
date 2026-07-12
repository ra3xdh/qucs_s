/// @file schematicvalidator.cpp
/// @brief Schematic validation class (implementation)
/// @author Andrés Martínez Mera
/// @date July 12, 2026

#include "schematicvalidator.h"
#include "schematic.h"
#include "components/component.h"

QVector<ValidationIssue> SchematicValidator::validate(Schematic *sch, const QString &backend) const
{
  QVector<ValidationIssue> issues;

  issues += checkFrequencySweepType(sch, backend);
  issues += checkMinimumPortsInSPSimulation(sch, backend);

  return issues;
}

QVector<ValidationIssue> SchematicValidator::checkFrequencySweepType(
    Schematic *sch, const QString &backend) const
{
  QVector<ValidationIssue> issues; // It may be several SP/AC blocks

  if (backend.toLower() == "qucsator")
    return issues; // qucsator handles list sweeps fine

  for (Component *component : sch->a_DocComps) {
    if (!component->isActive)
      continue;

    bool isFrequencySweepBlock = (component->Model == ".AC" || component->Model == ".SP");
    if (!isFrequencySweepBlock)
      continue;

    Property *sweepType = component->getProperty("Type");
    bool usesListSweep = sweepType && sweepType->Value == "list";
    if (usesListSweep) {
      ValidationIssue issue;

      // Error message
      issue.message = QObject::tr("%1 uses a 'list' frequency sweep, which %2 does not support.")
                          .arg(component->Name, backend);

      // Issue relevance
      issue.severity = 1; // Critical - Simulation will fail

      // Suggested solution
      issue.suggestedFix = QObject::tr("Use lin or log frequency sweep in %1").arg(component->Name);
      issues.append(issue);
    }
  }

  return issues;
}


QVector<ValidationIssue> SchematicValidator::checkMinimumPortsInSPSimulation(
    Schematic *sch, const QString &backend) const {
  QVector<ValidationIssue> issues;

  if (backend.toLower() != "ngspice")
    return issues; // only ngspice has this restriction


  bool hasActiveSPBlock = false;
  for (Component *component : sch->a_DocComps) {
    if (component->isActive && component->Model == ".SP") {
      hasActiveSPBlock = true;
      break;
    }
  }
  if (!hasActiveSPBlock)
    return issues; // no S-parameter simulation, nothing to check

  int acSourceCount = 0;
  for (Component *component : sch->a_DocComps) {
    if (!component->isActive) {
      // Ignore deactivated components
      continue;
    }

    if (component->Model.startsWith("VP")) {
      // AC power source found. They start with VP for ngspice
      acSourceCount++;
    }
  }

  if (acSourceCount < 2) {
    ValidationIssue issue;
    issue.message = QObject::tr(
                        "The schematic has %1 AC power source."
                        "ngspice requires at least 2 for S-parameter analysis.").arg(acSourceCount);
    issue.severity = 1; // Critical - simulation will fail
    issue.suggestedFix = QObject::tr(
        "If this is a 1-port SP simulation, add another AC Power souce component with the negative port connected to GND");
    issues.append(issue);
  }

  return issues;
}
