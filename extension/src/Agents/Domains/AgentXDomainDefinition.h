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

    // TODO Implement Domain Definition
    // Returns a Domain with Tasks, Conditions, Effects
public:
    Domain CreateAgentDomainBuilder()
    {
        AgentDomainBuilder builder("AgentX");

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