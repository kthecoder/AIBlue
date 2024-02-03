#include "FluidHTN/BaseDomainBuilder.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace FluidHTN;
using namespace godot;

/*
    Agent Domain Builder

    Description : Contains the helper functions to cleanly write domain logic.
                  Encapsulates the Actions, Conditions, Operators, and effects of
                  a task.

*/

class AgentDomainBuilder final : public BaseDomainBuilder
{

private:
    Node3D *agentNode;

public:
    void init_builder(Node3D *givenAgentNode)
    {
        agentNode = givenAgentNode;
    }
    AgentDomainBuilder(StringType n) : BaseDomainBuilder(n) {}

    // TODO Implement Extra HasState, SetState

    //======================================================//
    // Encapsulated HTN Tasks
    //======================================================//
    // TODO Implement Encapsulated HTN Tasks

    TaskStatus MoveToOperator(IContext &)
    {
        if (agentNode == nullptr)
        {
            // UtilityFunctions::print("AgentDomainBuilder : NULL PTR");
            return TaskStatus::Failure;
        }
        else
        {
            if (agentNode->has_method("moveTo"))
            {
                Variant result = agentNode->call("moveTo");
                // Variant result = agent->call("moveTo");
                // UtilityFunctions::print(result);
                if (result)
                {
                    return TaskStatus::Success;
                }
                else
                {
                    return TaskStatus::Failure;
                }
            }
            else
            {
                // UtilityFunctions::print("AgentDomainBuilder : No Function moveTo");
                return TaskStatus::Failure;
            }
        }
    }

    void MoveTo()
    {
        AddAction("MoveTo");

        AddOperator(std::bind(&AgentDomainBuilder::MoveToOperator, this, std::placeholders::_1));

        // UtilityFunctions::print("Added MoveTo Operator");
    }
};