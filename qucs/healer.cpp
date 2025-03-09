#include "healer.h"

#include "component.h"
#include "node.h"
#include "wire.h"

#include <memory>
#include <set>
#include <unordered_set>

#include "qt3_compat/q3ptrlist.h"


namespace qucs_s {

void Healer::HealingAction::execute(SchematicMutator* mutator) {
    action->execute(mutator);
}

class ConnectPorts : public AbstractAction {
    GenericPort* m_portA;
    GenericPort* m_portB;
public:
    ConnectPorts(GenericPort* a, GenericPort* b) : m_portA{a}, m_portB{b} {}
    void execute(SchematicMutator* mutator) override {
        mutator->connectPorts(m_portA, m_portB);
    }
};


class ConnectPortToNode : public AbstractAction {
    GenericPort* m_port;
    Node* m_node;
public:
    ConnectPortToNode(GenericPort* port, Node* node) : m_port{port}, m_node{node} {} 
    void execute(SchematicMutator* mutator) override { 
        mutator->connectPortWithNode(m_port, m_node);
    }
};


class MoveNode : public AbstractAction {
    Node* m_node;
    QPoint m_coords;
public:
    MoveNode(Node* node, const QPoint& to) : m_node{node}, m_coords{to} {} 
    void execute(SchematicMutator* mutator) override {
        mutator->moveNode(m_node, m_coords);
    }
};


class PutLabel : public AbstractAction {
    WireLabel* m_label;
    QPoint m_coords;
public:
    PutLabel(WireLabel* label, const QPoint& to) : m_label{label}, m_coords{to} {} 
    void execute(SchematicMutator* mutator) override { mutator->putLabel(m_label, m_coords); }
};


class DeleteWire : public AbstractAction {
    Wire* m_wire;
public:
    DeleteWire(Wire* wire) : m_wire{wire} {}
    void execute(SchematicMutator* mutator) override {
        mutator->deleteWire(m_wire);
    }
};

}


Element* qucs_s::GenericPort::host() const
{
    if (m_portType == PortType::Component) {
        return m_comp;
    }
    return m_wire;
}


QPoint qucs_s::GenericPort::center() const
{
    switch (m_portType) {
        case PortType::Component:
            return m_comp->center() + QPoint{m_port->x, m_port->y};
        case PortType::WireOne:
            return QPoint{m_wire->x1, m_wire->y1};
        case PortType::WireTwo:
            return QPoint{m_wire->x2, m_wire->y2};
    }
    assert(false);
}


[[nodiscard]] Node* qucs_s::GenericPort::replaceNodeWith(Node* new_node)
{
    switch (m_portType) {
        case PortType::Component: {
            if (new_node == m_port->Connection) return new_node;

            auto* old_node = m_port->Connection;
            if (old_node) old_node->disconnect(m_comp);
            m_port->Connection = new_node;
            new_node->connect(m_comp);

            // Ensure that only this and no other port of the component
            // is connected to the new node.
            for (auto* p : m_comp->Ports) {
                if (p->Connection == new_node && p != m_port) {
                    p->Connection = nullptr;
                }
            }

            return old_node;
        }
        case PortType::WireOne: {
            if (new_node == m_wire->Port1) return m_wire->Port1;

            auto* old_node = m_wire->Port1;
            new_node->connect(m_wire);
            m_wire->Port1->disconnect(m_wire);
            m_wire->Port1 = new_node;
            return old_node;
        }
        case PortType::WireTwo: {
            if (new_node == m_wire->Port2) return m_wire->Port2;

            auto* old_node = m_wire->Port2;
            new_node->connect(m_wire);
            m_wire->Port2->disconnect(m_wire);
            m_wire->Port2 = new_node;
            return old_node;
        }
    }
    assert(false);
}


qucs_s::Healer::Healer(const Q3PtrList<Component>* components, const Q3PtrList<Wire>* wires)
{
    for (auto* comp : *components) {
        for (auto* port : comp->Ports) {
            joints.emplace(
                port->Connection,
                std::make_unique<GenericPort>(port, comp));
        }
    }
    for (auto* wire : *wires) {
        joints.emplace(
            wire->Port1,
            std::make_unique<GenericPort>(wire, GenericPort::WirePort::One));
        joints.emplace(
            wire->Port2,
            std::make_unique<GenericPort>(wire, GenericPort::WirePort::Two));
    }
}

std::vector<qucs_s::Healer::HealingAction> qucs_s::Healer::planHealing()
{
    std::vector<HealingAction> healing_plan;

    std::set<Node*> skip_nodes;

    for (auto& [node, port] : joints) {
        if (skip_nodes.contains(node)) continue;

        if (joints.count(node) > 1 && allPortsMismatched(node)) {
            for (auto* action : processFullMismatch(node)) {
                healing_plan.push_back({ std::unique_ptr<AbstractAction>(action) });
            }
            skip_nodes.insert(node);
            continue;
        }

        if (node->center() == port->center()) {
            continue;
        }

        if (node->conn_count() == 1) {
            healing_plan.push_back({ std::make_unique<MoveNode>(node, port->center()) });
            continue;
        }

        for (auto* action : processPartialMismatch(port.get(), node)) {
            healing_plan.push_back({ std::unique_ptr<AbstractAction>(action) });
        }    
    }

    for (auto* w : obsolete_wires) {
        healing_plan.push_back({std::make_unique<DeleteWire>(w)});
    }

    return healing_plan;
}


std::pair<Node*,std::vector<Wire*>> qucs_s::Healer::findStableNode(Node* begin, Element* doNotGo)
{
    assert(hasMismatchedPorts(begin));

    if (!stable_node_cache.contains(begin)) {
        traverseAndFillCache(begin, doNotGo);
    }

    return {stable_node_cache.at(begin), traversed_wires_cache.at(begin)};
}


void qucs_s::Healer::traverseAndFillCache(Node* begin, Element* doNotGo)
{
    std::vector<Node*> passed_nodes{};
    std::vector<Wire*> passed_wires{};

    Node* current_node = begin;
    while (current_node->conn_count() == 2) {

        if (current_node != begin && hasMismatchedPorts(current_node)) {
            passed_nodes.push_back(current_node);
            auto [stable_node, wires] = mismatchFoundWhileTraversing(passed_nodes, passed_wires);
            stable_node_cache[begin] = stable_node;
            traversed_wires_cache[begin] = wires;
            return;
        }
        
        auto* wire = dynamic_cast<Wire*>(current_node->other_than(doNotGo));
        if (wire == nullptr) break;
        passed_wires.push_back(wire);
        doNotGo = wire;

        passed_nodes.push_back(current_node);
        current_node = wire->Port1 == current_node
                     ? wire->Port2
                     : wire->Port1;
    }

    passed_nodes.push_back(current_node);

    stable_node_cache[begin] = passed_nodes.back();
    traversed_wires_cache[begin] = passed_wires;
}

std::pair<Node*,std::vector<Wire*>> qucs_s::Healer::mismatchFoundWhileTraversing(std::vector<Node*>& passed_nodes, std::vector<Wire*>& passed_wires)
{
    assert(hasMismatchedPorts(passed_nodes.front()));
    assert(hasMismatchedPorts(passed_nodes.back()));


    //      A    B
    // o----o----o----o        n = 4, take node at i = 1
    // 0    1    2    3

    //           A
    // o----o----o----o----o   n = 5, take node at i = 2
    // 0    1    2    3    4

    std::size_t node_ix = static_cast<std::size_t>(std::ceil(passed_nodes.size() / 2.0) - 1);
    Node* stable_node = passed_nodes.at(node_ix);


    //      A    B
    // o----o----o----o        n = 3, take wires while i < 1
    //   0    1    2

    //           A
    // o----o----o----o----o   n = 4, take wires while i < 2
    //   0    1    2    3

    std::vector<Wire*> traversed;
    for (std::size_t i = 0; i < passed_wires.size() / 2; i++) {
        traversed.push_back(passed_wires.at(i));
    }

    return {stable_node, traversed};
}


bool qucs_s::Healer::hasMismatchedPorts(Node* node) const
{
    return std::any_of(joints.lower_bound(node), joints.upper_bound(node),
                        [node](auto& node_port) {
                        return node_port.second->center() != node->center();
                        });
}


bool qucs_s::Healer::allPortsMismatched(Node* node) const
{
    return std::all_of(joints.lower_bound(node), joints.upper_bound(node),
                        [node](auto& node_port) {
                        return node_port.second->center() != node->center();
                        });
}


[[ nodiscard ]] std::vector<qucs_s::AbstractAction*> qucs_s::Healer::processFullMismatch(Node* node)
{
    using PortPair = UnorderedPair<GenericPort*,GenericPort*>;

    std::unordered_set<PortPair> connected;
    std::vector<AbstractAction*> actions;

    for (auto outer = joints.lower_bound(node); outer != joints.upper_bound(node); outer++) {
        for (auto inner = joints.lower_bound(node); inner != joints.upper_bound(node); inner++) {

            PortPair port_pair{
                outer->second.get(),
                inner->second.get()
            };

            if (port_pair.first == port_pair.second) continue;
            if (connected.contains(port_pair)) continue;
            connected.insert(port_pair);
            actions.push_back(new ConnectPorts(port_pair.first, port_pair.second));
        }   
    }

    if (node->Label != nullptr) {
        actions.push_back(new PutLabel(node->Label, connected.begin()->first->center()));
    }

    return actions;
}


[[ nodiscard ]] std::vector<qucs_s::AbstractAction*> qucs_s::Healer::processPartialMismatch(GenericPort* port, Node* node)
{
    auto [stable_node, passed_wires] = findStableNode(node, port->host());
    std::vector<qucs_s::AbstractAction*> actions{new ConnectPortToNode(port, stable_node)};

    for (auto* w : passed_wires) {
        if (obsolete_wires.contains(w)) continue;
        if (w->Label != nullptr) {
            actions.push_back(new PutLabel(w->Label, stable_node->center()));
        }
        obsolete_wires.insert(w);
    }

    return actions;
}