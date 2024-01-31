#include "ai_blue_agent.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

AIBlueAgent::AIBlueAgent()
{
    _planner = Planner();
    _context = AgentContext();
    _domain = *_agent_domainDefinition.CreateAgentDomainBuilder().Build();
}

AIBlueAgent::~AIBlueAgent()
{
}

void AIBlueAgent::planner_tick()
{
    // Tick the Planner
    _planner.Tick<WsAgent, uint8_t, AgentWorldState>(_domain, _context);
}

// TODO You will need to Bind Sensors so that they can be called in Godot

void AIBlueAgent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("planner_tick"), &AIBlueAgent::planner_tick);
}