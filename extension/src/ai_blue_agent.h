#ifndef AI_BLUE_AGENT_CLASS_H
#define AI_BLUE_AGENT_CLASS_H

// We don't need windows.h in this plugin but many others do and it throws up on itself all the time
// So best to include it
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>

#include "Agents/Domains/AgentXDomainDefinition.h"
#include "FluidHTN/Domain.h"
#include "FluidHTN/Planners/Planner.h"
#include "Agents/AgentContext.h"

using namespace godot;

class AIBlueAgent : public RefCounted
{
    GDCLASS(AIBlueAgent, RefCounted);

protected:
    static void _bind_methods();

private:
    AgentXDomainDefinition _agentX_domainDefinition;
    Planner _planner;
    Domain _domain;
    AgentContext _context;
    // TODO Sensory System
    // StimuliSystem _sensors;

public:
    AIBlueAgent();
    ~AIBlueAgent();

    void planner_tick();
};

#endif // AIBlueAgent_CLASS_H