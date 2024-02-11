#include "FluidHTN/BaseDomainBuilder.h"
#include "AgentContext.h"

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

    //======================================================//
    //
    // Encapsulated HTN Tasks
    //
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
                // Call the
                int result = agentNode->call("moveTo");

                //? Can print output using: UtilityFunctions::print(result);
                // UtilityFunctions::print(result);
                return static_cast<TaskStatus>(result);
            }
            else
            {
                //? Can print output using:UtilityFunctions::print("AgentDomainBuilder : No Function moveTo");
                return TaskStatus::Failure;
            }
        }
    }

    void MoveTo()
    {
        AddCondition("AgentNewPos", [=](IContext &ctx)
                     { return static_cast<AgentContext &>(ctx).HasState(WsAgent::wsAgentMovement, (int)(AgentMovement::NewPosition)); });
        AddAction("MoveTo");

        AddOperator(std::bind(&AgentDomainBuilder::MoveToOperator, this, std::placeholders::_1));

        AddEffect("ArrivedAtNewPos", FluidEffectType::Permanent, [=](IContext &ctx, FluidEffectType effectType)
                  { return static_cast<AgentContext &>(ctx).SetState(
                        WsAgent::wsAgentMovement, (int)(AgentMovement::Arrived), true, effectType); });

        End();
    }

    /**
     *
     *  Task : Idle
     *
     *  Description : Always succeeds the default state
     *
     */

    TaskStatus IdleOperator(IContext &ctx)
    {
        return TaskStatus::Success;
    }

    void Idle()
    {
        AddAction("Idle");

        AddOperator(std::bind(&AgentDomainBuilder::IdleOperator, this, std::placeholders::_1));

        End();
    }
};