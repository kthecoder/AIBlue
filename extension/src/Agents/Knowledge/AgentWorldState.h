#include "FluidHTN/Contexts/BaseContext.h"

using namespace FluidHTN;
using namespace std;

enum class WsAgent
{
    wsEnemyRange,
    wsHealth,
};

enum class EnemyRange
{
    InViewRange,
    OutOfRange,
};

typedef uint8_t EnemyRangeTy;

class AgentWorldState : public IWorldState<WsAgent, uint8_t, AgentWorldState>
{
    uint8_t CurrentWorldState[2];
    EnemyRange currentRange;

public:
    AgentWorldState()
    {
        // TODO This initial setup could be incorrect in the end
        currentRange = static_cast<EnemyRange>(CurrentWorldState[static_cast<int>(WsAgent::wsEnemyRange)]);
        CurrentWorldState[(int)(WsAgent::wsEnemyRange)] = (int)(EnemyRange::InViewRange);
    }

    bool HasState(WsAgent state, uint8_t value)
    {
        return (CurrentWorldState[(int)state] == value);
    }

    uint8_t &GetState(WsAgent state) { return CurrentWorldState[(int)state]; }

    void SetState(WsAgent state, uint8_t value) { CurrentWorldState[(int)state] = value; }

    //! Size must match the Enums length
    int GetMaxPropertyCount() { return 2; }
};
