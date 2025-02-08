#ifndef HEALER_H
#define HEALER_H

#include "qt3_compat/q3ptrlist.h"
#include <QPoint>
#include <memory>
#include <map>
#include <set>
#include <vector>

namespace qucs_s {

template<typename T1, typename T2>
struct UnorderedPair {
    T1 first;
    T2 second;

    bool operator==(const UnorderedPair<T1,T2>& other) const {
        return
            (first == other.first && second == other.second)
            ||
            (first == other.second && second == other.first);
    }
};
}

template<typename T1, typename T2>
struct std::hash<qucs_s::UnorderedPair<T1,T2>>
{
    std::size_t operator()(const qucs_s::UnorderedPair<T1,T2>& p) const
    {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
    }
};

struct Port;
class Component;
class Wire;
class Node;
class Element;
class WireLabel;

namespace qucs_s {

class GenericPort final {
public:
    enum class PortType { Component, WireOne, WireTwo };
    enum class WirePort { One, Two };

    GenericPort(Port* port, Component* comp)
      : m_port{port}
      , m_comp{comp}
      , m_portType{PortType::Component} {}

    GenericPort(Wire* wire, WirePort portId)
      : m_wire{wire}
      , m_portType{portId == WirePort::One
                           ? PortType::WireOne
                           : PortType::WireTwo} {}

private:
    Port* m_port = nullptr;
    Component* m_comp = nullptr;
    Wire* m_wire = nullptr;
    PortType m_portType;
public:
    Element* host() const;
    QPoint center() const;
    [[nodiscard]] Node* replaceNodeWith(Node* new_node);
};


struct SchematicMutator {
    virtual void deleteWire(Wire* w) = 0;
    virtual void connectPorts(GenericPort* a, GenericPort* b) = 0;
    virtual void connectPortWithNode(GenericPort* port, Node* node) = 0;
    virtual void putLabel(WireLabel* label, const QPoint& p) = 0;
    virtual void moveNode(Node* node, const QPoint& p) = 0;
};


class AbstractAction { 
public:
    virtual ~AbstractAction() = default;
    virtual void execute(SchematicMutator* mutator) = 0;
};

class Healer {

    std::multimap<Node*,std::unique_ptr<GenericPort>> joints;
    std::map<Node*,Node*> stable_node_cache;
    std::map<Node*,std::vector<Wire*>> traversed_wires_cache;

    std::set<Wire*> obsolete_wires;

    bool hasMismatchedPorts(Node* node) const;
    bool allPortsMismatched(Node* node) const;

    [[ nodiscard ]] std::vector<AbstractAction*> processFullMismatch(Node* node);
    [[ nodiscard ]] std::vector<AbstractAction*> processPartialMismatch(GenericPort* port, Node* node);

    std::pair<Node*,std::vector<Wire*>> findStableNode(Node* begin, Element* doNotGo);
    void traverseAndFillCache(Node* begin, Element* doNotGo);
    std::pair<Node*,std::vector<Wire*>> mismatchFoundWhileTraversing(std::vector<Node*>& passed_nodes, std::vector<Wire*>& passed_wires);

public:
    struct HealingAction {
        std::unique_ptr<AbstractAction> action;
        void execute(SchematicMutator* mutator);
    };

    Healer(const Q3PtrList<Component>* components, const Q3PtrList<Wire>* wires);
    std::vector<HealingAction> planHealing();
};
}

#endif