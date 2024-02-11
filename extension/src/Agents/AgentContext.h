#pragma once
#include "FluidHTN/Contexts/BaseContext.h"
#include "Agents/Knowledge/AgentWorldState.h"

using namespace FluidHTN;

class AgentContext : public BaseContext<WsAgent, uint8_t, AgentWorldState>
{
    bool _done = false;

public:
    AgentContext() { _WorldState = MakeSharedPtr<AgentWorldState>(); }

    bool &Done() { return _done; }

    void SetStateAgent(WsAgent state, int value)
    {
        _WorldState->SetState(static_cast<WsAgent>(state), static_cast<uint8_t>(value));
    }
    void SetStateAgent(WsAgent state, int value, bool dirty, FluidEffectType eff)
    {
        BaseContext::SetState(static_cast<WsAgent>(state), static_cast<uint8_t>(value), dirty, eff);
    }

    uint8_t GetStateAgent(WsAgent state) { return BaseContext::GetState(static_cast<WsAgent>(state)); }
};

typedef BaseContext<WsAgent, uint8_t, AgentWorldState> BaseContextType;

class MyDebugContext : public AgentContext
{
public:
    MyDebugContext()
    {
        _DebugMTR = true;
        _LogDecomposition = true;
    }
};
