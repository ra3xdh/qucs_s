#ifndef HEALER_H
#define HEALER_H

#include <QPoint>
#include <memory>
#include <map>
#include <vector>
#include <list>

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
    bool isOfWire() const { return m_portType != PortType::Component; }
    bool isOfComponent() const { return !isOfWire(); }

    template<typename T>
    T* host() const { static_assert(!std::is_same<T,T>::value); }
    Node* node() const;
    QPoint center() const;
    Node* replaceNodeWith(Node* new_node);

    void moveCenterTo(const QPoint& coords);
};


struct SchematicMutator {
    virtual ~SchematicMutator() = default;
    virtual void deleteWire(Wire* /*unused*/) {}
    virtual void connectWithWire(const QPoint& /*unused*/, const QPoint& /*unused*/) {}
    virtual void putLabel(WireLabel* /*unused*/, Node* /*unused*/) {}
    virtual void moveNode(Node* /*unused*/, const QPoint& /*unused*/) {}
    virtual void movePort(GenericPort* /*unused*/, const QPoint& /*unused*/) {}
    virtual void replaceNode(GenericPort* /*unused*/) {}
};


struct AbstractAction { 
    virtual ~AbstractAction() = default;
    virtual void execute(SchematicMutator* mutator) = 0;
};


struct HealerParameters {
    bool allowWireReshaping = true;
    bool allowWireRelaying = false;
    int wireRelayingDepth = -1;
};


class Healer {
    class HealerImpl;
    std::unique_ptr<HealerImpl> pimpl;

public:
    using HealingAction = std::unique_ptr<AbstractAction>;

    Healer(const std::list<Component*>* components, const std::list<Wire*>* wires, const HealerParameters& params);
    ~Healer();
    std::vector<HealingAction> planHealing() const;
};

}
#endif