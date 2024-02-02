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
    AgentDomainBuilder builder = AgentDomainBuilder("AgentX");

public:
    void init_domain(Node3D *givenAgentNode)
    {
        if (givenAgentNode)
        {
            builder.init_builder(givenAgentNode);
        }
        else
        {
            UtilityFunctions::print("AgentXDomainDefinition NULL PTR");
        }
    }

    Domain CreateAgentDomainBuilder()
    {
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