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

    // TODO Implement Encapsulated HTN Tasks
};
