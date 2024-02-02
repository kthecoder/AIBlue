#include "ai_blue_agent.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

AIBlueAgent::AIBlueAgent()
{
    UtilityFunctions::print("AIBlueAgent | Planner is being setup.");
    _planner = Planner();

    _context = AgentContext();
    // Must Initialize the Agent Context
    _context.Init();

    UtilityFunctions::print("AIBlueAgent | Planner Finished Setup");
}

AIBlueAgent::~AIBlueAgent()
{
}

bool AIBlueAgent::agent_setup(Object *agentNode)
{
    if (agentNode == nullptr)
    {
        return false;
    }
    else
    {
        if (agentNode->has_method("moveTo"))
        {
            UtilityFunctions::print("AIBLUE : Agent has function Move To");
        }
        else
        {
            UtilityFunctions::print("AIBLUE : Agent does not have function Move To");
        }
        // Build the Domain of the selected Agent
        _agentX_domainDefinition.init_domain(Object::cast_to<Node3D>(agentNode));
        _domain = _agentX_domainDefinition.CreateAgentDomainBuilder();

        return true;
    }
}

void AIBlueAgent::planner_tick()
{
    // Tick the Planner
    _planner.Tick<WsAgent, uint8_t, AgentWorldState>(_domain, _context);
}

// TODO You will need to Bind Sensors so that they can be called in Godot

void AIBlueAgent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("agent_setup", "node"), &AIBlueAgent::agent_setup);
    ClassDB::bind_method(D_METHOD("planner_tick"), &AIBlueAgent::planner_tick);
}