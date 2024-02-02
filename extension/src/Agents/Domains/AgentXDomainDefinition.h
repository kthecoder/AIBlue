#include "Agents/AgentDomainBuilder.h"
#include "Domain.h"

using namespace FluidHTN;

/*

    AgentX Domain Definition

    Description : Helper class that implements the creation of an AI Agents HTN Tasks

    Reference ~ FluidHTN-CPP Test Files : DomainBuilderTests.cpp

*/

class AgentXDomainDefinition
{

private:
    godot::Node3D *agentNode;

public:
    void init_domain(Node3D *givenAgentNode)
    {
        if (givenAgentNode == nullptr)
        {
            UtilityFunctions::print("AgentXDomainDefinition NULL PTR");
        }
        else
        {
            if (givenAgentNode->has_method("moveTo"))
            {
                UtilityFunctions::print("AgentXDomainDefinition : Agent has function Move To");
            }
            else
            {
                UtilityFunctions::print("AgentXDomainDefinition : Agent does not have function Move To");
            }
            agentNode = givenAgentNode;
        }
    }

    Domain CreateAgentDomainBuilder()
    {
        AgentDomainBuilder builder("AgentX");
        if (agentNode == nullptr)
        {
            UtilityFunctions::print("AgentXDomainDefinition2 NULL PTR");
        }
        builder.init_builder(agentNode);

        /*
            Tasks are encapsulated and defined in AgentDomainBuilder.h
        */

        builder.AddSelector("MoveTo");
        builder.MoveTo();
        builder.End(); // End the MoveTO
        builder.End(); // End the Selector

        return *builder.Build();
    }
};