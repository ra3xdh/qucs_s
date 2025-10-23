#ifndef SCHEMATIC_SELECTION_H
#define SCHEMATIC_SELECTION_H

#include <vector>
#include <QRect>

#include "component.h"
#include "wire.h"
#include "painting.h"
#include "diagram.h"
#include "wirelabel.h"
#include "marker.h"
#include "node.h"

struct SchematicSelection {
  QRect bounds;
  std::vector<Component*> components;
  std::vector<Wire*> wires;
  std::vector<Painting*> paintings;
  std::vector<Diagram*> diagrams;
  std::vector<WireLabel*> labels;
  std::vector<Marker*> markers;
  std::vector<Node*> nodes;

  // Return whether the selection is empty
  bool isEmpty() const {
    return components.empty() &&
      wires.empty() &&
      paintings.empty() &&
      diagrams.empty() &&
      labels.empty() &&
      markers.empty() &&
      nodes.empty();
  }
  // Valid checks if we are not empty and has a valid boundary
  bool isValid() const {
    return !isEmpty() && bounds.isValid() && !bounds.isEmpty();
  }

  // Return total amount of elements selected
  std::size_t count() const {
    return components.size() +
      wires.size() +
      paintings.size() +
      diagrams.size() +
      labels.size() +
      markers.size() +
      nodes.size();
  }

  // Clear selection
  void clear() {
    components.clear();
    wires.clear();
    paintings.clear();
    diagrams.clear();
    labels.clear();
    markers.clear();
    nodes.clear();
    bounds = QRect();
  }

  // Move center for all components
  void moveCenter(int dx, int dy) {
    for (auto* pc : components)   pc->moveCenter(dx, dy);
    for (auto* pw : wires)        pw->moveCenter(dx, dy);
    for (auto* pp : paintings)    pp->moveCenter(dx, dy);
    for (auto* pd : diagrams)     pd->moveCenter(dx, dy);
    for (auto* pl : labels)       pl->moveCenter(dx, dy);
    for (auto* pm : markers)      pm->moveCenter(dx, dy);
    for (auto* pn : nodes)        pn->moveCenter(dx, dy);

    // Move bounds
    bounds.moveCenter(QPoint(dx, dy));
  }
};
#endif
