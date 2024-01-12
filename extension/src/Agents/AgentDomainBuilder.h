#include "BaseDomainBuilder.h"

using namespace FluidHTN;

/*
    Agent Domain Builder

    Description : Contains the helper functions to cleanly write domain logic.
                  Encapsulates the Actions, Conditions, Operators, and effects of
                  a task.

*/

class AgentDomainBuilder final : public BaseDomainBuilder
{
public:
    AgentDomainBuilder(StringType n) : BaseDomainBuilder(n) {}

    // TODO Implement Extra HasState, SetState

    //======================================================//
    // Encapsulated HTN Tasks
    //======================================================//
    // TODO Implement Encapsulated HTN Tasks
    void MoveTo()
    {
        AddAction("MoveTo");

        AddOperator([](IContext &) -> TaskStatus
                    { 
                        //TODO Implement Godot Function that you call to Execute MoveTo
                        //! Godot Function must return Enum of TaskStatus
                        // You could return an INT of 0 - 2, and convert it in a helper class

                        return TaskStatus::Success; });
    }
};
