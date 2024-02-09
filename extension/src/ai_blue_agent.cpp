#include "ai_blue_agent.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

AIBlueAgent::AIBlueAgent()
{
    // UtilityFunctions::print("AIBlueAgent | Planner is being setup.");
    _planner = Planner();

    _context = AgentContext();
    // Must Initialize the Agent Context
    _context.Init();

    // UtilityFunctions::print("AIBlueAgent | Planner Finished Setup");
}

AIBlueAgent::~AIBlueAgent()
{
}

bool AIBlueAgent::agent_setup(Variant agentNode)
{
    if (agentNode)
    {

        NodePath path = agentNode;
        Node3D *node = Object::cast_to<Node3D>(get_node_or_null(path));
        // Build the Domain of the selected Agent
        _agentX_domainDefinition.init_domain(node);
        _domain = _agentX_domainDefinition.CreateAgentDomainBuilder();

        return true;
    }
    else
    {
        return false;
    }
}

void AIBlueAgent::_physics_process(double _delta)
{
    if (_domain.Root()->Name() != "DefaultDomainName")
    {
        planner_tick();
    }
}

void AIBlueAgent::planner_tick()
{
    // Tick the Planner
    _planner.Tick<WsAgent, uint8_t, AgentWorldState>(_domain, _context);
}

/**
 *  Sensors
 *
 *  Note : Sensors go here
 */

// TODO Update this to meet your Knowledge Definitions
void AIBlueAgent::vision_sensor(int state, int value)
{
    // Get Agent Movement State in the Agents Knowledge
    WsAgent wsAgentState = static_cast<WsAgent>(state);
    // Get the value to assign to that agents Knowledge State
    AgentMovement movementValue = static_cast<AgentMovement>(value);

    _context.SetStateAgent(wsAgentState, MovementTy(movementValue), true, FluidEffectType::PlanAndExecute);
}

/**
 *  Bind Methods
 *
 *  Note : All class Functions to be exposed to GDScript need to be setup here
 */

// TODO : You will need to Bind Sensors so that they can be called in Godot
void AIBlueAgent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("agent_setup", "node"), &AIBlueAgent::agent_setup);
    ClassDB::bind_method(D_METHOD("planner_tick"), &AIBlueAgent::planner_tick);
    ClassDB::bind_method(D_METHOD("vision_sensor", "state", "value"), &AIBlueAgent::vision_sensor);
}