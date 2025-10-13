#ifndef SCHEMATIC_SELECTION_H
#define SCHEMATIC_SELECTION_H

#include <vector>
#include <QRect>

class Component;
class Wire;
class Painting;
class Diagram;
class WireLabel;
class Marker;
class Node;

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
};
#endif
