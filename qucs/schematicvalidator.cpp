/// @file schematicvalidator.cpp
/// @brief Schematic validation class (implementation)
/// @author Andrés Martínez Mera
/// @date July 12, 2026

#include "schematicvalidator.h"
#include "schematic.h"
#include "node.h"
#include "wire.h"
#include "components/component.h"
#include "components/subcircuit.h"

QVector<ValidationIssue> SchematicValidator::validate() const {
  QSet<QString> visitedFiles;
  return runAllChecks(visitedFiles);
}


QVector<ValidationIssue> SchematicValidator::runAllChecks(QSet<QString> &visitedFiles) const {
  QVector<ValidationIssue> issues;

  // Top-level schematic checks. Not applicable to subcircuits
  issues += checkFrequencySweepType();
  issues += checkMCRunsCount();
  issues += checkMinimumPortsInSPSimulation();
  issues += checkMissingSimulation();

  // Structural checks: run at top-level and recursively inside every subcircuit.
  issues += checkStructuralIssues(visitedFiles);

  return issues;
}


QVector<ValidationIssue> SchematicValidator::checkStructuralIssues(QSet<QString> &visitedFiles) const {
  QVector<ValidationIssue> issues;

  issues += checkDanglingWires();
  issues += checkUnconnectedPorts();
  issues += checkSubcircuits(visitedFiles);

  return issues;
}


QVector<ValidationIssue> SchematicValidator::checkSubcircuits(QSet<QString> &visitedFiles) const{
  QVector<ValidationIssue> issues;

  for (Component *component : sch->a_DocComps) {
    if (!component->isActive || component->Model != "Sub") {
      // Discard all but subcircuits
      continue;
    }

    Subcircuit *subcirc = static_cast<Subcircuit*>(component);
    QString file = subcirc->getSubcircuitFile();

    if (file.isEmpty() || visitedFiles.contains(file)) {
      // Skip already inspected subcircuits
      continue;
    }
    visitedFiles.insert(file);

    Schematic *nested = new Schematic(nullptr, file);
    if (!nested->loadDocument()) {
      ValidationIssue issue;
      issue.title = QObject::tr("Missing subcircuit");
      issue.message = QObject::tr("Cannot load subcircuit file '%1'.").arg(file);
      issue.severity = 1; // Critical: Simulation won't run if the subckt is missing
      issue.suggestedFix = QObject::tr(
          "Check that the subcircuit file exists and is a valid schematic.");
      issues.append(issue);
      delete nested;
      continue;
    }

    // Check subcircuit
    SchematicValidator nestedValidator;
    nestedValidator.setSchematic(nested);
    nestedValidator.setSimulationBackend(backend);
    QVector<ValidationIssue> SubCktIssues = nestedValidator.checkStructuralIssues(visitedFiles);

    for (ValidationIssue issue : std::as_const(SubCktIssues)) {
      issue.message = QObject::tr("[in subcircuit '%1'] %2").arg(file, issue.message);
      issues.append(issue);
    }

    delete nested;
  }

  return issues;
}

QVector<ValidationIssue> SchematicValidator::checkFrequencySweepType() const
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

      // Issue title
      issue.title = QObject::tr("Wrong sweep");

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

QVector<ValidationIssue> SchematicValidator::checkMCRunsCount() const
{
  QVector<ValidationIssue> issues;
  for (Component *comp : sch->a_DocComps) {
    if (!comp->isActive)
      continue;
    if (comp->Model == ".MC") {
      Property* runsProp = comp->getProperty("Runs");
      bool ok        = false;
      const int runs = runsProp->Value.trimmed().toInt(&ok);
      if (!ok || runs <= 0) {
        ValidationIssue issue;
        // Issue title
        issue.title = QObject::tr("Wrong Monte Carlo runs count");
        // Error message
        issue.message = QObject::tr("%1 uses a malformed or negative runs count")
                            .arg(comp->Name);
        // Issue relevance
        issue.severity = 1; // Critical - Simulation will fail
        // Suggested solution
        issue.suggestedFix = QObject::tr("Set Runs property as positive"
                                         " integer number at component %1").arg(comp->Name);
        issues.append(issue);
      }
    }
  }
  return issues;
}


QVector<ValidationIssue> SchematicValidator::checkMinimumPortsInSPSimulation() const {
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

    if (component->Model.startsWith("Pac")) {
      // AC power source found. They start with VP for ngspice
      acSourceCount++;
    }
  }

  if (acSourceCount < 2) {
    ValidationIssue issue;
    issue.title = QObject::tr("Wrong S-parameter simulation setup");
    issue.message = QObject::tr(
                        "The schematic has %1 AC power source. "
                        "Ngspice requires at least 2 for S-parameter analysis.").arg(acSourceCount);
    issue.severity = 1; // Critical - simulation will fail
    issue.suggestedFix = QObject::tr(
        "If this is a 1-port SP simulation, add another AC Power souce component with the negative port connected to GND");
    issues.append(issue);
  }

  return issues;
}

QVector<ValidationIssue> SchematicValidator::checkMissingSimulation() const
{
  QVector<ValidationIssue> issues;

  // Model strings for all recognized simulation controller blocks.
  static const QStringList kSimulationBlockModels = {
      ".AC", ".SP", ".TR", ".DC", ".HB", ".SW", ".NOISE", ".TF"
  };

  for (Component *component : sch->a_DocComps) {
    if (component->isActive && kSimulationBlockModels.contains(component->Model))
      // Found one block. It's ok
      return issues;
  }

  ValidationIssue issue;
  issue.title = QObject::tr("Missing simulation block");
  issue.message = QObject::tr(
      "The schematic does not contain any active simulation block.");
  issue.severity = 1; // Critical - nothing to simulate
  issue.suggestedFix = QObject::tr(
      "Add a simulation block (e.g. .AC, .SP, .TR, .DC) to the schematic.");

  issues.append(issue);

  return issues;
}

QVector<ValidationIssue> SchematicValidator::checkDanglingWires() const
{
  QVector<ValidationIssue> issues;
  // Inspect all wires
  for (Wire *wire : sch->a_DocWires) {
    Node *endpoint1 = wire->Port1;
    Node *endpoint2 = wire->Port2;

    // To be considered a dangling wire:
    // 1) The wire has no label associated
    // 2) It doesn't lead to another wire
    // 3) It's not tied to another component
    bool endpoint1_isOpen = !endpoint1->hasLabel()
                            && endpoint1->wires().size() <= 1
                            && endpoint1->components().empty();
    bool endpoint2_isOpen = !endpoint2->hasLabel()
                            && endpoint2->wires().size() <= 1
                            && endpoint2->components().empty();

    if (endpoint1_isOpen && endpoint2_isOpen) {
      // Both ends open: the wire is entirely disconnected
      ValidationIssue issue;
      issue.title = QObject::tr("Dangling wire");
      issue.message = QObject::tr(
                           "Wire is not connected to anything at either end (near (%1, %2) and (%3, %4)).")
                           .arg(endpoint1->cx).arg(endpoint1->cy)
                           .arg(endpoint2->cx).arg(endpoint2->cy);
      issue.severity = 3; // Minor, but the user should review it. In many cases this will be something merely aesthetic, but
      // it may happen that the user forgot to connect something
      issue.suggestedFix = QObject::tr(
          "Remove the wire.");
      issues.append(issue);
    }
    else if (endpoint1_isOpen) {
      // First end open
      ValidationIssue issue;

      issue.title = QObject::tr("Dangling wire");
      issue.message = QObject::tr("Wire has an unconnected end near (%1, %2).").arg(endpoint1->cx).arg(endpoint1->cy);
      issue.severity = 3;
      issue.suggestedFix = QObject::tr(
          "Terminate the open end or remove the dangling wire.");
      issues.append(issue);
    }
    else if (endpoint2_isOpen) {
      // Second end open
      ValidationIssue issue;

      issue.title = QObject::tr("Dangling wire");
      issue.message = QObject::tr("Wire has an unconnected end near (%1, %2).").arg(endpoint2->cx).arg(endpoint2->cy);
      issue.severity = 3;
      issue.suggestedFix = QObject::tr(
          "Terminate the open end or remove the dangling wire.");
      issues.append(issue);
    }
  }
  return issues;
}

QVector<ValidationIssue> SchematicValidator::checkUnconnectedPorts() const
{
  QVector<ValidationIssue> issues;
  for (Component *component : sch->a_DocComps) {
    // Iterate over each component in the schematic
    if (!component->isActive) {
      // Discard deactivated components
      continue;
    }
    for (Port *port : std::as_const(component->Ports)) {
      // For each port, check if the ports' net reaches something. Otherwise, report issue
      Node *node = port->Connection;
      bool isFloating = !node || (!node->hasLabel() && !netReachesOtherComponent(node, component));
      if (isFloating) {

        // First get the name of the component.
        QString componentLabel;
        if (component->Name.isEmpty()){
          // The component's name is empty (e.g. GND). Then take the model name.
          componentLabel = QObject::tr("%1 component at (%2, %3)")
                               .arg(component->Model).arg(component->cx).arg(component->cy);
        } else {
          // The component has a name (the vast majority of components)
          componentLabel = component->Name;
        }

        ValidationIssue issue;
        issue.title = QObject::tr("Disconnected port");
        issue.message = QObject::tr("Port of %1 is not connected to anything.")
                                 .arg(componentLabel);
        issue.severity = 2; // Warning. Simulation may run (e.g. 1-port S-parameter ngspice require one dangling port), but the user must check this.
        issue.suggestedFix = QObject::tr(
            "Connect this pin to the intended net, or check for a typo in the net label.");
        issues.append(issue);
      }
    }
  }
  return issues;
}

bool SchematicValidator::netReachesOtherComponent(Node *node, Component *owner) const {
  QSet<Node*> visited; // Nodes already visited during the traversal, to avoid revisiting again.
  QVector<Node*> stack; // Nodes still to be explored
  stack.push_back(node);

  while (!stack.isEmpty()) {
    Node *n = stack.takeLast();

    if (!n || visited.contains(n)) {
      // Skip null nodes and nodes visited
      continue;
    }
    visited.insert(n);

    if (n->hasLabel())
      return true;

    for (Component *c : n->components()) {
      // Check every component attached to this node.
      if (c != owner)
        return true;
    }

    // Follow every wire touching this node to the node on its other end, and queue it up for exploration,
    for (Wire *w : n->wires()) {
      Node *other = (w->Port1 == n) ? w->Port2 : w->Port1;
      if (other && !visited.contains(other))
        stack.push_back(other);
    }
  }

  // Explored the entire net reachable from 'node' and found no component other than 'owner', so the net is effectively dangling.
  return false;
}