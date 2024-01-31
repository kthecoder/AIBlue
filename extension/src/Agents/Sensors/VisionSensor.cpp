#include "Agents/Sensors/VisionSensor.h"

void updateVision(int wsEnumValue)
{
    // TODO Call Set State
    AgentWorldState().SetState(WsAgent::wsEnemyRange, EnemyRangeTy(EnemyRange::InViewRange));
}