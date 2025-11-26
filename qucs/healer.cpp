#include "healer.h"

#include "component.h"
#include "multi_point.h"
#include "node.h"
#include "wire.h"
#include "wirelabel.h"

#include <algorithm>
#include <concepts>
#include <limits>
#include <memory>
#include <ranges>
#include <set>


template<>
struct std::less<QPoint> {
    bool operator()(const QPoint& lhs, const QPoint& rhs) const
    {
        if (lhs.x() == rhs.x()) return lhs.y() < rhs.y();
        return lhs.x() < rhs.x();
    }
};


namespace qucs_s {

using std::make_unique;
using std::map;
using std::unique_ptr;
using std::shared_ptr;
using std::vector;



class MoveNode : public AbstractAction {
    Node* m_node;
    QPoint m_coords;
public:
    MoveNode(Node* node, const QPoint& to) : m_node{node}, m_coords{to} {}
    void execute(SchematicMutator* mutator) override { mutator->moveNode(m_node, m_coords); }
    int priority() const override { return 100; }
};


class MovePort : public AbstractAction {
    GenericPort* m_port;
    QPoint m_coords;
public:
    MovePort(GenericPort* port, const QPoint& coords) : m_port{port}, m_coords{coords} {}
    void execute(SchematicMutator* mutator) override { mutator->movePort(m_port, m_coords); }
    int priority() const override { return 90; }
};


class ReplaceNode : public AbstractAction {
    GenericPort* m_port;
public:
    ReplaceNode(GenericPort* port) : m_port{port} {}
    void execute(SchematicMutator* mutator) override { mutator->replaceNode(m_port); }
    int priority() const override { return 80; }
};


class ReattachLabel : public AbstractAction {
    WireLabel* m_label;
    Node* m_dest;
public:
    ReattachLabel(WireLabel* label, Node* new_host) : m_label{label}, m_dest{new_host} {}
    void execute(SchematicMutator* mutator) override { mutator->putLabel(m_label, m_dest); }
    int priority() const override { return 70; }
};


class ConnectWithWire : public AbstractAction {
    QPoint m_point_A;
    QPoint m_point_B;
public:
    ConnectWithWire(const QPoint& a, const QPoint& b) : m_point_A{a}, m_point_B{b} {}
    void execute(SchematicMutator* mutator) override { mutator->connectWithWire(m_point_A, m_point_B); }
    int priority() const override { return 50; }
};


class DeleteWire : public AbstractAction {
    Wire* m_wire;
public:
    DeleteWire(Wire* wire) : m_wire{wire} {}
    void execute(SchematicMutator* mutator) override { mutator->deleteWire(m_wire); }
    int priority() const override { return 0; }
};


// GenericPort
// --------------------------------------------------------------------------------------

QPoint GenericPort::center() const
{
    switch (m_portType) {
        case PortType::Component:
            return m_comp->center() + QPoint{m_port->x, m_port->y};
        case PortType::WireOne:
            return m_wire->P1();
        case PortType::WireTwo:
            return m_wire->P2();
    }
    assert(false);
}


Node* GenericPort::replaceNodeWith(Node* new_node)
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
            if (old_node) old_node->disconnect(m_wire);
            m_wire->Port1 = new_node;
            new_node->connect(m_wire);

            // Ensure that only this port is connected to the new node
            if (m_wire->Port2 == new_node) m_wire->Port2 = nullptr;
            return old_node;
        }
        case PortType::WireTwo: {
            if (new_node == m_wire->Port2) return m_wire->Port2;

            auto* old_node = m_wire->Port2;
            if (old_node) old_node->disconnect(m_wire);
            m_wire->Port2 = new_node;
            new_node->connect(m_wire);

            // Ensure that only this port is connected to the new node
            if (m_wire->Port1 == new_node) m_wire->Port1 = nullptr;
            return old_node;
        }
    }
    assert(false);
}


void GenericPort::moveCenterTo(const QPoint& coords)
{
    assert(isOfWire());
    auto* wire = hostWire();

    switch (m_portType) {
    case (PortType::WireOne):
        wire->setP1(coords);
        return;

    case (PortType::WireTwo):
        wire->setP2(coords);
        return;

    default:
        assert(false);
    }
}


Node* GenericPort::node() const
{
    switch (m_portType) {
    case PortType::WireOne:
      return m_wire->Port1;
    case PortType::WireTwo:
      return m_wire->Port2;
    case PortType::Component:
      return m_port->Connection;
    }

    assert(false);

    return nullptr;
}


// Utils
// --------------------------------------------------------------------------------------

class JointStateAssessor
{
    std::multimap<QPoint,GenericPort*> m_port_locations;
    std::set<QPoint> m_unique_locations;
    Node* m_node;

public:
    JointStateAssessor(Node* node, const vector<shared_ptr<GenericPort>>& ports)
        : m_node{node}
    {
        for (const auto& p : ports) {
            m_unique_locations.emplace(p->center());
            m_port_locations.emplace(p->center(), p.get());
        }
    }

    bool onlyWirePortsAt(const QPoint& loc) const
    {
        return std::none_of(
            m_port_locations.lower_bound(loc),
            m_port_locations.upper_bound(loc),
            [](const auto& loc_port) { return loc_port.second->isOfComponent(); });
    }

    bool isOK() const
    {
        return m_unique_locations.size() == 1 && m_unique_locations.contains(m_node->center());
    }

    bool isOnlyNodeMisplaced() const
    {
        return m_unique_locations.size() == 1 && !m_unique_locations.contains(m_node->center());
    }

    bool onlyTwoPortClusters() const
    {
        return  m_unique_locations.size() == 2;
    }

    const std::multimap<QPoint,GenericPort*>& portLocations() const
    {
        return m_port_locations;
    }

    const std::set<QPoint>& uniqueLocations() const
    {
        return m_unique_locations;
    }
};


namespace {

template<std::ranges::forward_range R>
requires std::same_as<QPoint, std::iter_value_t<R>>
QPoint findClosest(const R& points, const QPoint point)
{
    QPoint closest = *points.begin();
    double min_dist = geom::distance(point, closest);
    for (auto& pt : points) {
        auto d = geom::distance(pt, point);
        if (d < min_dist) closest = pt;
    }
    return closest;
}


inline bool canReshape(Node* node, const JointStateAssessor& jsa)
{
    return jsa.onlyTwoPortClusters() && jsa.onlyWirePortsAt(node->center());
}


bool isSpecialCase(const JointStateAssessor& jsa)
{
    if (!jsa.onlyTwoPortClusters()) return false;

    auto single_wire_port_loc = std::ranges::find_if(
        jsa.uniqueLocations(),
        [&jsa](auto& loc) { return jsa.portLocations().count(loc) == 1 && jsa.onlyWirePortsAt(loc); }
    );


    if (single_wire_port_loc == jsa.uniqueLocations().end()) return false;

    const auto other_loc = std::ranges::find_if(jsa.uniqueLocations(), [single_wire_port_loc](auto loc) { return loc != *single_wire_port_loc; });
    assert(other_loc != jsa.uniqueLocations().end());

    if (jsa.onlyWirePortsAt(*other_loc)) return false;

    Wire* single_wire = jsa.portLocations().lower_bound(*single_wire_port_loc)->second->hostWire();
    assert(single_wire != nullptr);

    const QPoint p1 = single_wire->P1();
    const QPoint p2 = single_wire->P2();
    return *other_loc == p1 || *other_loc == p2 || qucs_s::geom::is_it_line(*other_loc, p1, p2);
}
}

// HealerImpl
// --------------------------------------------------------------------------------------

class Healer::HealerImpl
{
    HealerParameters m_params;
    std::size_t m_affectedCount;

    using PortGroup = std::vector<std::shared_ptr<GenericPort>>;
    std::map<Node*, PortGroup> m_port_groups;

    vector<Healer::HealingAction> processMisplacedNodeCase(Node* node, const JointStateAssessor& jsa) const;
    vector<Healer::HealingAction> processSpecialCase(Node* node, const JointStateAssessor& jsa) const;
    vector<Healer::HealingAction> processReshapingCase(Node* node, const JointStateAssessor& jsa) const;
    vector<Healer::HealingAction> processGenericCase(Node* node, const JointStateAssessor& jsa) const;

public:
    HealerImpl(const std::list<Component*>* components, const std::list<Wire*>* wires, const HealerParameters& hp, std::size_t affected_count);
    std::vector<Healer::HealingAction> planHealing() const;
};


Healer::HealerImpl::HealerImpl(const std::list<Component*>* components, const std::list<Wire*>* wires, const HealerParameters& hp, std::size_t affected_count)
    : m_params{hp}
    , m_affectedCount{affected_count}
{
    for (auto* comp : *components) {
        for (auto* port : comp->Ports) {
            m_port_groups[port->Connection].push_back(std::make_unique<GenericPort>(port, comp));
        }
    }
    for (auto* wire : *wires) {
        m_port_groups[wire->Port1].push_back(std::make_unique<GenericPort>(wire, GenericPort::WirePort::One));
        m_port_groups[wire->Port2].push_back(std::make_unique<GenericPort>(wire, GenericPort::WirePort::Two));
    }
}


vector<Healer::HealingAction> Healer::HealerImpl::planHealing() const
{
    vector<HealingAction> healing_plan;

    for (const auto& [node, port_group] : m_port_groups) {
        const JointStateAssessor joint_state{node, port_group};

        if (joint_state.isOK()) {
            continue;
        }

        vector<unique_ptr<AbstractAction>> healing_actions;

        if (joint_state.isOnlyNodeMisplaced()) {
            healing_actions = processMisplacedNodeCase(node, joint_state);
        }
        else if (isSpecialCase(joint_state)) {
            healing_actions = processSpecialCase(node, joint_state);
        }
        else if (m_params.allowWireReshaping && canReshape(node, joint_state)) {
            healing_actions = processReshapingCase(node, joint_state);
        }
        else {
            healing_actions = processGenericCase(node, joint_state);
        }

        for (auto& a : healing_actions) {
            healing_plan.emplace_back(std::move(a));
        }
    }

    std::ranges::sort(healing_plan, [](const auto& lhs, const auto& rhs) { return lhs->priority() > rhs->priority(); });
    return healing_plan;
}


vector<Healer::HealingAction> Healer::HealerImpl::processMisplacedNodeCase(Node* node, const JointStateAssessor& jsa) const
{
    assert(jsa.isOnlyNodeMisplaced());
    vector<HealingAction> actions;

    if (m_affectedCount != 0) {
        actions.push_back(make_unique<MoveNode>(node, *jsa.uniqueLocations().begin()));
        return actions;
    }

    for (const auto& port : m_port_groups.at(node)) {
        if (port->isOfComponent()) {
            actions.push_back(make_unique<ReplaceNode>(port.get()));
            actions.push_back(make_unique<ConnectWithWire>(port->center(), node->center()));
        } else {
            auto* wire = port->hostWire();

            if (m_params.allowWireReshaping || qucs_s::geom::is_it_line(node->center(), wire->P1(), wire->P2())) {
                actions.push_back(make_unique<MovePort>(port.get(), node->center()));
            } else {
                auto* other_node = wire->Port1 == node ? wire->Port2 : wire->Port1;

                if (wire->hasLabel()) {
                    actions.push_back(make_unique<ReattachLabel>(wire->releaseLabel().release(), other_node));
                }

                actions.push_back(make_unique<ConnectWithWire>(node->center(), other_node->center()));
                actions.push_back(make_unique<DeleteWire>(wire));
            }
        }
    }

    return actions;
}


vector<Healer::HealingAction> Healer::HealerImpl::processSpecialCase(Node* node, const JointStateAssessor& jsa) const
{
    assert(jsa.uniqueLocations().size() == 2);

    GenericPort* single_wire_port = nullptr;
    for (const auto& [location, port] : jsa.portLocations()) {
        if (port->isOfWire() && jsa.portLocations().count(location) == 1) {
            single_wire_port = port;
            break;
        }
    }
    assert(single_wire_port != nullptr);

    const auto other_loc = std::ranges::find_if(jsa.uniqueLocations(), [single_wire_port](auto loc) { return loc != single_wire_port->center(); });
    assert(other_loc != jsa.uniqueLocations().end());

    vector<HealingAction> actions;
    if (node->center() != *other_loc) {
        actions.push_back(make_unique<MoveNode>(node, *other_loc));
    }
    actions.push_back(make_unique<MovePort>(single_wire_port, *other_loc));

    return actions;
}


vector<Healer::HealingAction> Healer::HealerImpl::processReshapingCase(Node* node, const JointStateAssessor& jsa) const
{
    assert(jsa.uniqueLocations().size() == 2);

    const auto other_loc = std::ranges::find_if(jsa.uniqueLocations(), [node](auto loc) { return loc != node->center(); });

    vector<HealingAction> actions;
    actions.push_back(make_unique<MoveNode>(node, *other_loc));

    for (auto port : m_port_groups.at(node)) {
        if (port->center() == *other_loc) continue;
        actions.push_back(make_unique<MovePort>(port.get(), *other_loc));
    }

    return actions;
}


vector<Healer::HealingAction> Healer::HealerImpl::processGenericCase(Node* node, const JointStateAssessor& jsa) const
{
    vector<Healer::HealingAction> actions;

    auto node_loc = node->center();

    if (!jsa.portLocations().contains(node->center())) {
        node_loc = findClosest(jsa.uniqueLocations(), node_loc);
        actions.push_back(make_unique<MoveNode>(node, node_loc));
    }

    for (auto port : m_port_groups.at(node)) {
        if (port->center() == node_loc) continue;

        actions.push_back(make_unique<ReplaceNode>(port.get()));
        actions.push_back(make_unique<ConnectWithWire>(port->center(), node_loc));
    }

    return actions;
}


// Healer
// --------------------------------------------------------------------------------------

Healer::Healer(const std::list<Component*>* components, const std::list<Wire*>* wires, const HealerParameters& hp)
    : pimpl{make_unique<HealerImpl>(
        components, wires, hp,
        std::ranges::count_if(*wires, [](auto* w) { return w->isSelected; }) + std::ranges::count_if(*components, [](auto* w) { return w->isSelected; })
        )}
{
}


Healer::~Healer() = default;


std::vector<Healer::HealingAction>Healer::planHealing() const
{
    return pimpl->planHealing();
}

}
