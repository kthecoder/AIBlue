# Agents

A set of Fluid HTN creations that define the Agents: Awareness, Knowledge, & Decisions.

A walkthrough example of how to design and implement an Agent system using Fluid HTN.

## What do my Agents look like?

I want a co-op game, let's say 4 players, that can shoot at a group of Enemy Agents. Each Agent needs to know how to move toward the players, be able to shoot their weapons, loose health, damage players.

Agents can be different types, different DPS, Defense, Range, HP, Attack Cooldown, ReloadTime, etc. We can visual this for players with different colored Agents Models.

## When & What

1. Agents can move toward players
   1. If they aren't in range of a player
1. Agents can LookAt players
   1. If enemy player in range
   1. If player NOT in sight
1. Agents can shoot when
   1. If they are in range of an enemy player &&
   1. If they can see a player
1. Agents should loose health
   1. If they get hit
1. Agents can damage a player
   1. If a bullet hits a player

Agents Should Not

1. Stay Idle
   1. This is a horde game after all

## Create an Agents Notation

Some of the things we wrote above don't have to do with the Agent in particular. Such as, damaging players can be done on the the player script, agents loosing health can be done on the agents collision shape script. Damage and Health should be taken care of in their own system and so we let that system take care of those things. We only want to be concerned with what the Agent needs to do. This reduces the Agent to needing the ability to Move, Look, Shoot, Reload, & Death.

If we really think about the problem we have one goal for the Agents, AttackPlayer. This results in the following Agent Notation:

```
Sequence Task [ AttackPlayer(playerLocation) ]
    Preconditions [ Condition1(playerInRange), Condition2(playerInSight) ]
        Primitive Task 1 ( ShootEnemy(playerID) )
            Preconditions [ hasBullets() ]
            Operator [ ShootWeaponOperator() ]
    Preconditions [Condition1(playerInRange)]
        Primitive Task 2 [ LookAt(playerID) ]
            Operator [ RotateAgent() ]
    Primitive Task 3 [ MoveTo(playerLocation) ]
        Operator [ MoveToOperator(playerLocation) ]
            Effects [ Effect(playerInRange, true) ]
```

Since Attacking the Player is the goal, we define Shooting the Player as the top priority that should always be chosen if possible, over all other options.

Lets look more into what is going on:

```
// Our Compound Task that the Agent can do
Sequence Task [ AttackPlayer(playerID) ]
    //If the Agent is in range and looking at enemy player
    Preconditions [ Condition1(playerInRange), Condition2(playerInSight) ]
        Primitive Task ( ShootEnemy(playerID) )
            Preconditions [ hasBullets() ]
            Operator [ ShootWeaponOperator() ]
    //If the Agent is in range but NOT looking at enemy player
    Preconditions [Condition1(playerInRange)]
        Primitive Task [ LookAt(playerID) ]
            Operator [ RotateAgent() ]
    //Else, Default to Moving to Player Location
    Primitive Task [ MoveTo(playerID) ]
        Operator [ MoveToOperator(playerID) ]
            Effects [ Effect(playerInRange, true) ]
```

How do we know which player to attack? We could randomly choose from the list of players or we could create a sensor to find which player is closer and go for that player. It could be interesting if more Agents go after a particular player requiring other players to come to their aid.

## Convert to Fluid HTN

Now that we have define our Agent Notation we can easily understand how to apply it to Fluid HTN.

```
public enum AgentState : byte
{
    wsEnemyRange,
    wsHealth,
}

public enum EnemyRange : byte
{
    ViewRange,
    OutOfRange,
}

AgentDomainBuilder("HordeAgent")
    .Sequence("AttackPlayer")
        .ShootEnemy()
        .RotateAgent()
        .MoveTo()
    .End()
    .Build();
```

## Write Code

Now that we know what we want our Agent to do and how to do it, we can begin writing code to implement this Agent feature set.

# Notes

1. Receiving Damage
   1. You could extend this example so that when an Agent gets hit, they play an animation
