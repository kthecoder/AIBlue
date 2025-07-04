#ifndef AI_BLUE_AGENT_CLASS_H
#define AI_BLUE_AGENT_CLASS_H

#include <godot_cpp/classes/node3d.hpp>

#include "Agents/Domains/AgentXDomainDefinition.h"
#include "FluidHTN/Domain.h"
#include "FluidHTN/Planners/Planner.h"
#include "Agents/AgentContext.h"

using namespace godot;

class AIBlueAgent : public Node3D
{
    GDCLASS(AIBlueAgent, Node3D);

protected:
    static void _bind_methods();

private:
    AgentXDomainDefinition _agentX_domainDefinition;
    FluidHTN::Planner _planner;
    FluidHTN::Domain _domain;
    AgentContext _context;

public:
    AIBlueAgent();
    ~AIBlueAgent();

    bool agent_setup(Variant agentNode);
    void planner_tick();

    void _physics_process(double delta) override;

    /*
     *  Sensors
     */

    // TODO : Add all your sensors to the top level class here

    void vision_sensor(int state, int value);
    void movement_update(int state, int value);
};

#endif // AIBlueAgent_CLASS_H