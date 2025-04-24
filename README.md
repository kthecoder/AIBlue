![AI Blue a Fluid Hierarchical Task Network Port](https://github.com/kthecoder/AIBlue/blob/main/Architecture/Assets/ai_blue_banner.png)

A simple HTN planner based around the principles of the Builder Pattern, [Original](https://github.com/ptrefall/fluid-hierarchical-task-network) ported into [C++ Version](https://github.com/amoldeshpande/fluid-hierarchical-task-network), used to create a Godot template.

This is a work in progress! I have not created an example to test the success of this design yet.

![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)

# Disclaimer

The AI system in written in C++, the sensors and operators written in GdScript and make function calls to the C++ code.

Code is no longer _generated from paddy-exe/GDExtensionSummator_ but [the Godot quickstart template for GDExtension development with Godot 4.0+.](https://github.com/godotengine/godot-cpp-template). (Github gives no way to remove that from the repo without wiping this one and remaking it.)

# Usage

Godot 4.4 is the currently used Version.

For step by step instructions on how to use this template see : [Project Read Me](https://github.com/kthecoder/AIBlue/blob/main/PROJECTREADME.md)

# How it Works

Agents are fully designed in the C++ extension, operators and sensors are done in GdScript. Once you have fully designed your Agent(s) knowledge/context, domain int c++ and in gdextension call all your operators from gdscript functions, you can then compile the plugin and include it's binaries in your project where Godot will call the binaries of your built extension. For more information on building see : [Project ReadMe](https://github.com/kthecoder/AIBlue/blob/main/PROJECTREADME.md)

![AI Blue Agent : FluidHTN CPP in GDExtension Model](https://github.com/kthecoder/AIBlue/blob/main/Architecture/Assets/Architecture.drawio.png)

1. Context
   1. Knowledge system, the brain of the Agent
   1. Enums for what it knows in the moment
      1. i.e. EnemyInRange
   1. Lists for data it needs to know about
      1. i.e. KnownEnemies
1. Domain
   1. How the Agent knows what to do and if they can do what
   1. Priority
      1. Items higher in the Domain are chosen if they meet their preconditions
1. Effects
   1. If an Action succeeds how does it change the Agents Knowledge
   1. Effects is our prediction of how state will change for performing said action
1. Conditions
   1. What must be true for an Action to be chosen
1. Operators (A.K.A. ActionsAPI)
   1. Agent Actions can be fully built in Godot/GdScript or C++.
   1. If GdScript is chosen, we can define operators in GdScript and execute them from C++
      1. i.e. Execute : MoveTo(enemyX14)
         1. Where enemyX14 was chosen by the Planner based on the Knowledge of the AI's Context
      1. The Planner needs to know the success or failure of an operator
   1. If C++ is chosen, updating the Agent will require passing in necessary information like delta, position, animations, etc.
1. Sensors (A.K.A. StimuliAPI)
   1. Sensors can be fully built in Godot/GdScript and then encoded and passed to the AI
   1. i.e. Collisions shape on Agent collides and detects using Raycast that enemy is in front of it. Update the Agent's world state by sending the sensor data of 'EnemyInRange' to GDExtension.

# The Process of Agent System Creation

The steps necessary to understand and design an Agent System.

## Step 1 : Define the Problem

Start by writing out what you want the Agent to be able to do and learn about.

i.e. I want an Agent that can pick up apples, see enemies, shoot enemies, etc.

# Step 2 : Define When & What

When can the Agent do the things you want it to do?

i.e. They can shoot when they see an enemy, they can pick up apples that are within range, etc.

What happens when the Agent does the things you want it to do?

i.e. After shooting enemy bullets decrease, after picking up an apple there are less apples on the ground, etc.

## Step 2 : Create HTN Notation from the Problem

Now that we know what we want to accomplish. We can then break it down into its parts.

We do this by defining the ecosystem of the Agent(s) in HTN Notation. We can call this "Agent Notation".

The following describes how to create and write your Agent logic before you even begin to code:

```
Select Task [TaskName(term1, term2,...)]
    Preconditions [Condition1, Condition2,...]
        Subtasks [task1(term1, term2,...). task2(term1, term2,...),...]
    Splice ( subdomain1 )
    Preconditions [Condition1, Condition2,...]
        Subtasks [task1(term1, term2,...). task2(term1, term2,...),...]
PausePlan()
Sequence Task [TaskName(term1, term2,...)]
    Slot( slotID )
    Preconditions [Condition1, Condition2,...]
        Subtasks [task1(term1, term2,...). task2(term1, term2,...),...]

Primitive Task 1 [task1()]
    Preconditions [<conditions>] //Optional
    Operator [task1Operator(<parameters>)]
        Effects [<effect>] // Optional
Primitive Task 2 [task2()]
    Preconditions [<conditions] //Optional
    Operator [task2Operator(<parameters>)]
        Effects [<effect>] // Optional
```

**HTN Notation Adapted From [Chapter : 12.3](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)**

The goal is to split up the Agents features into Awareness, Knowledge, Actions, and Tasks. Take everything you decided about your Agent and split it up into the following using HTN Notation:

1. Choices
   1. The top successful/valid option is the decision that Agent will make
1. Awareness
   1. Stimuli from the Agents environment
   1. Sensors that collect information and convert into Knowledge the AI can understand
1. Knowledge
   1. You can have as many of these as you need
   1. i.e. AgentWorldKnowledge
      1. i.e. EnemyInRange | Agent can see an enemy
      1. i.e. AgentHealth
1. Tasks
   1. What the Agent can decide to do
   1. Your Selector is your IF/ELSE chain, and your Sequence is your AND chain
   1. Selector - Complete a Single Task
      1. i.e. Can Shoot a target
         1. Pre-Conditions
            1. i.e. If EnemyInRange
   1. Sequence - Complete all sub-tasks successfully
      1. Will require that ALL sub-tasks are valid and will branch down each one in order.
      1. i.e. FindEnemy(), MoveToEnemy(), AttackEnemy()
1. Actions/Primitives (2 Separate things in Code but same thing in idea)
   1. What the Agent can do
   1. i.e. Shoot Target
      1. Effects
         1. i.e. AgentAttackScore += 1

Agent Notation Example :

```
Select Task [ KillEnemy() ]
    Preconditions [ (EnemyRange, DashBackRange, LessOrEqual), (EnemyVisible, True) ]
        Primitive(DashBack)
    Preconditions [ (EnemyRange, LessOrEqual), (EnemyVisible, True) ]
        Primitive(MoveAwayFromEnemy)
        Primitive(ShootAtEnemy)
    Preconditions [ (Location, NavigationNearEnemy, NotEqual), (EnemyRange, GreaterOrEqual), (EnemyVisible, True), (PathBlocked, True)]
        Primitive(MoveTo)
            Operator [ MoveTo() ]
        Primitive(NavigationIdle)
            Operator [ NavigationIdle() ]
    Precondition(EnemyVisible, False)
        Select Task [EnemyLost() ]
Select Task [ EnemyLost() ]
    ...
Select Task [ RushEnemy() ]
    PrimitiveTask [ (RunTowardsCover) ]
    PrimitiveTask [ (EnterCover) ]
    PrimitiveTask [ (ThrowGrenade) ]
    PrimitiveTask [ (ShootAtEnemy) ]
PrimitiveTask [ (ShootAtEnemy) ]
    Preconditions [ (EnemyVisible, True) ]
        Operator[ ShootAtEnemyOperator() ]
            Effects [ (RotateToFocus), (Vulnerability, 1), (Frustrated, 0) ]
PrimitiveTask [ (RunTowardsCover) ]
    ...
PrimitiveTask [ (EnterCover) ]
    ...
PrimitiveTask [ (ThrowGrenade) ]
    ...
PrimitiveTask [ (ShootAtEnemy) ]
    ...
PrimitiveTask [ (ScanPatrolPoint) ]
    Preconditions [ (AlertLevel, IGNORANT), (EnemyVisible, False) ]
        Operator[ ScanPatrolPointOperator() ]
        Effects [ (AreaClear, True) ]
Primitive(MeleeEnemy)
    Preconditions [ (EnemyInRange, True) ]
        Operator [ MeleeEnemyOperator() ]
            Effects [ (Damaged, False),(IsInPain, NO_PAIN), (Balanced, True) ]
Primitive(MoveTo(destination))
Primitive(NavigationIdle)

```

## Step 3 : Convert your HTN Notation to Fluid HTN

Now that you have taken the time to really think out what you want your Agent(s) to do, it should be trivial how to convert it to code. Take your HTN Notation (which is a written version of the Fluid HTN Domain) & implement everything you need for the Agent to plan, come alive.

The following attempts to illustrate the architecture of the CPP version of Fluid HTN:

![Fluid HTN CPP Version Architecture Diagram](https://github.com/kthecoder/AIBlue/blob/main/Architecture/Assets/architecture.png)

Based on your Agent Notation:

1. Implement : Sensors
   1. Awareness inputs of things the Agent can consume
   1. Define Simple systems that create Knowledge/Enum-Values
1. Implement : World State
   1. Knowledge : WorldStates you need and what their values will be.
   1. Define the Enums
1. Implement : Actions
   1. Primitive task that can contain conditions, an operator and effects.
   1. Defined in the Domain
1. Implement : Operators
   1. Executions of Primitive Tasks
   1. Define `IOperator` for each primitive task
   1. Must return success or failure
1. Implement : Pre-conditions
   1. Requirements for choosing a task
   1. `Conditions(string name, Func<T, bool> condition)`
   1. Must return a boolean that represents a success or failure
1. Implement : Effects
   1. State Changes if a task is successful
   1. `Effect(string name, EffectType effectType, Action<T, EffectType> action)`
1. Implement : Domain
   1. The Domain definition : term to describe the entire task hierarchy

Using the example from [Fluid HTN Troll Bridge Example](https://github.com/ptrefall/fluid-troll-bridge) the pre-conditions, effects, actions, & end(); are all consumed into the AIBuilder.

This simplifies the Domain to look like in the example:

```
return new AIDomainBuilder("Troll")
            .Select("Received damage")
                .ReceivedDamage()
            .End()
            .Sequence("Bridge patrol")
                .HasState(AIWorldState.HasBridgesInSight)
                .FindBridge()
                .MoveToBridge()
                .Wait(2f)
            .End()
            .Build();
```

Where `.ReceivedDamage()` includes, action, pre-condition, operator, effect, and `End()`.

The condition `.HasState(AIWorldState.HasBridgesInSight)` is not inside a primitive because it must be validated before going into the `FindBridge()` primitive.

## Step 4 : Code

Assuming you want to write the Operators & Sensors in GdScript.

Implement the following Interfaces:

1. BaseContext | i.e. AgentA : BaseContext
   1. Brain of the Agent
   1. Enums for what it knows in the moment
      1. i.e. EnemyInRange
   1. Lists for data it needs to know about
      1. i.e. KnownEnemies
1. BaseDomainBuilder
   1. Contains Operators
1. IOperator
   1. Create Operators for all possible Primitives
   1. This simply calls the functions from GdScript
1. Domain<AgentA>
   1. The HTN of your Agent Type
   1. This can include subdomains, slots, and splices
1. ISensors
   1. Functions that consume World State and update Agents Knowledge

Implement the following in Godot:

1. Sensors
   1. i.e. Enemy Detection -> EnemyInRange
   1. Send data converted to WorldState values to the C++ code
1. Operators
   1. Agents ActionAPI

## Step 5 : GDExtension

Connect up your code to Godot using GDExtension by exposing the necessary functions & signals.

# Fluid HTN Overview

Each Agent has a brain of knowledge, a sensory system, an actions system, and a decisions system.

The goal of HTN is to go through the decomposition and find a plan that succeeds. Going through the designed Choices, Conditions, Effects, Operators and World State.

[A link to a simple example](https://discord.com/channels/572332490581082127/572332490581082129/588238548918861827)

Parts of this Read Me are pulled from : [Fluid HTN Ref](https://github.com/ptrefall/fluid-hierarchical-task-network/blob/master/README.md)

## Sensory System

The agent needs a way to know what is going on around it to be able to make decisions.

Sensors are ways in which the Agent knows what it can and cannot do.

## Knowledge System

The agent needs a way to store information about the world around it to be able to understand it. You have to define what the data means to the AI, and you also have to both collect it (sensors) and translate it into the context (brain) that the AI understands.

Knowledge in an HTN is encoded by using an ENUM that defines the world state.

"The world state is essentially a vector of properties that describe what our
HTN is going to reason about." [Chapter : 12.2.1](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf) "...In the case of WsIsTired, the byte can represent the Boolean values zero and one. With WsEnemyRange, the values in the enum EEnemyRange are
used. It’s important to note that the world state only needs to represent what is needed for
the HTN to make decisions. That’s why WsEnemyRange is represented by abstract values,
instead of the actual range. The goal of the world state isn’t to represent every possible
state of every possible object in the game. It only needs to represent the problem space that our
planner needs to make decisions."

```
    enum EHtnWorldStateProperties
    {
        WsEnemyRange,
        WsHealth,
        WsIsTired,
        …
    }

    enum EEnemyRange
    {
        MeleeRange,
        ViewRange,
        OutOfRange,
        …
    }

    vector<byte> CurrentWorldState;
    EEnemyRange currentRange = CurrentWorldState[WsEnemyRange];
    CurrentWorldState[WsEnemyRange] = MeleeRange;
```

## Decisions System

HTN outputs a plan, which is a sequence of tasks. Decisions are defined by creating Primitives and Compound Tasks. Where Primitives are single step and compound are a set of primitives.

### Primitive Tasks

Primitive Tasks are comprised of an operator and sets of effects & conditions.

1. Conditions
   1. In order for a primitive to execute conditions must be valid
   1. Executing Conditions
      1. Evaluated every planner tick
      1. Used for cases where you need to re-evaluate the validity of your conditions after planning and after setting a new task as current task during execution
         1. (e.g. the operator of the task will return continue).
         1. Here, the operator returns continue and the condition becomes "invalid", the planner will not automatically know.
         1. Executing Conditions is one way to ensure that the planner realize that the task is now invalid.
      1. Another way is to put this logic inside the Operator and have it return Failure, which should yield the same result in practice (it triggers a replan)
1. Effects
   1. Effects is our prediction of how state will change for performing said action
   1. If task is successful, effects will apply to Agents world state and make the current plan we are evaluating continue to formulate
   1. PlanOnly : Effect
      1. Temporarily changes the world state during planning, used as prediction about the future.
      1. Its change on the world state is removed before plan execution.
      1. This can be useful when we need other systems to set the world state during execution.
   1. PlanAndExecute : Effect
      1. Only that during execution
      1. When the task they represent complete its execution successfully, the effect is re-applied
      1. This is useful in the cases where you don't have other systems to set the world state during execution.
   1. Permanent : Effect
      1. Applied during planning, but not removed from the world state before execution
      1. This can be very useful when there's some state we change only during planning
      1. e.g. do this thing three times then do this other thing.
      1. It can also be useful when we want to update world state in our runtime code, where we definitely want our changes to be permanent.
1. Operator
   1. Atomic Action the Agent does

"As an example, let’s take the two tasks SprintToEnemy and WalkToNextBridge.
Both of these tasks use the MoveTo operator, but the two tasks change the state of our
NPC in different ways. On the successful completion of SprintToEnemy, our NPC will
be at the enemy and tired, specified by the task’s effects. WalkToNextBridge task’s
effects would set the NPC’s location to the bridge and he’d be a little more bored." [Chapter : 12.2.3](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)

### Compound Tasks

Can be understood as high level tasks that have multiple ways of being accomplished.

"Using the Trunk Thumper as an example, he may have the task AttackEnemy. Our Thumper may
have different ways of accomplishing this task. If he has access to a tree trunk, he may
run to his target and use it as a melee weapon to “thump” his enemy. If no tree trunks are
available, he can pull large boulders from the ground and toss them at our enemy. He may
have a multitude of other approaches if the conditions are right." [Chapter : 12.2.4](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)

In Fluid HTN, Compound tasks are split between "Selectors" and "Sequencers".

1. Selectors
   1. Must be able to decompose a single sub-task
1. Sequencers
   1. Must be able to decompose all its sub-tasks successfully for itself to have decomposed successfully
1. Random Select
   1. Extensions Project : https://github.com/ptrefall/fluid-hierarchical-task-network-ext
1. Utility Select
   1. Extensions Project : https://github.com/ptrefall/fluid-hierarchical-task-network-ext

## Actions System

Actions are defined as Operators. The things the AI can actually do.

Every time an operator updates, it returns a status whether it succeeded, failed or need to continue next tick.

## Partial Planning

Used only inside a Sequence in the Domain. It allows to only plan up to that point and continue when the partial part of the plan has completed.

## Splicing

Splicing lets us inject sub-domains for code-reuse.

Is a selector, so the root of a sub domain will always be encapsulated in a slot selector of the parent domain

## Slots

Slots allows us to inject subdomains at runtime.

Ability to setup compound tasks specially designed for specialized behavior that are added into the hierarchy at runtime. Typically this would be smart objects, scripted events or meeting needs to add or remove new behavior at runtime, like binding abilities to weapons or spells or otherwise.

You can setup pre-determined slots that are filled later dynamically.

Interaction with smart objects only if they are in range or available.

## Planner Execution

The execution will check conditions of a task when it starts, run execution conditions every tick, run the task operator every tick and apply "executing" effects upon task success.
