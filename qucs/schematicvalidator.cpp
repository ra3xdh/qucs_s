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
  // issues += checkSomeOtherThing(sch, backend);

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
