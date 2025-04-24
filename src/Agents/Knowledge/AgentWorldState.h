#include "FluidHTN/Contexts/BaseContext.h"

using namespace FluidHTN;
using namespace std;

enum class WsAgent
{
    wsEnemyRange,
    wsAgentMovement,
    wsHealth,
};

enum class EnemyRange
{
    InViewRange,
    OutOfRange,
};

enum class AgentMovement
{
    NewPosition,
    Arrived,
};

typedef uint8_t EnemyRangeTy;
typedef uint8_t MovementTy;

class AgentWorldState : public IWorldState<WsAgent, uint8_t, AgentWorldState>
{
    uint8_t CurrentWorldState[2];
    EnemyRange currentRange;

public:
    AgentWorldState()
    {
        // TODO : Initial States for the Agent are setup here
        CurrentWorldState[(int)(WsAgent::wsEnemyRange)] = (int)(EnemyRange::InViewRange);
        CurrentWorldState[(int)(WsAgent::wsEnemyRange)] = (int)(AgentMovement::Arrived);
    }

    bool HasState(WsAgent state, uint8_t value)
    {
        return (CurrentWorldState[(int)state] == value);
    }

    uint8_t &GetState(WsAgent state) { return CurrentWorldState[(int)state]; }

    void SetState(WsAgent state, uint8_t value) { CurrentWorldState[(int)state] = value; }

    //! Size must match the CurrentWorldState length
    int GetMaxPropertyCount() { return 2; }
};
