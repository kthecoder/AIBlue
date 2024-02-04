#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Domain.h"
#include "Planners/Planner.h"
#include "Tasks/CompoundTasks/Selector.h"
#include "Tasks/PrimitiveTasks/PrimitiveTask.h"
#include "DomainContextTester.h"

/**
 *  Test : Get Plan -> Returns Clear Instance At Start - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Get_Plan_Returns_Clear_Instance_At_Start_Validated)
{
    Planner planner;
    auto plan = planner.GetPlan();

    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test :Get Current Task Returns Null At Start - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Get_Current_Task_Returns_Null_At_Start_Validated)
{
    Planner planner;
    auto task = planner.GetCurrentTask();

    EXPECT_TRUE(task == nullptr);
}

/**
 *  Test : Tick Without Initialized Context Fails - Exception
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_Without_Initialized_Context_Fails)
{
    bool exceptionThrown = false;

    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;

    try
    {
        planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    }
    catch (const std::exception &e)
    {
        exceptionThrown = true;
    }

    EXPECT_TRUE(exceptionThrown);
}

/**
 *  Test : Tick With Empty Domain - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_With_Empty_Domain_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");

    Planner planner;
    ctx.Init();

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
}

/**
 *  Test :Tick With Primitive Task Without Operator - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_With_Primitive_Task_Without_Operator_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;
    ctx.Init();
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<ITask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    auto currentTask = planner.GetCurrentTask();

    EXPECT_TRUE(currentTask == nullptr);
    EXPECT_TRUE(planner.LastStatus() == TaskStatus::Failure);
}

/**
 *  Test : Tick With Func Operator With Null Func - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_With_Func_Operator_With_Null_Func_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;
    ctx.Init();

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>(nullptr);

    task2->SetOperator(f);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    auto currentTask = planner.GetCurrentTask();

    EXPECT_TRUE(currentTask == nullptr);
    EXPECT_TRUE(planner.LastStatus() == TaskStatus::Failure);
}

/**
 *  Test : Tick With Default Success Operator Wont StackOverflows - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_With_Default_Success_Operator_Wont_StackOverflows_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;
    ctx.Init();
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>([](IContext &)
                                                         { return TaskStatus::Success; });
    task2->SetOperator(f);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    auto currentTask = planner.GetCurrentTask();

    EXPECT_TRUE(currentTask == nullptr);
    EXPECT_TRUE(planner.LastStatus() == TaskStatus::Success);
}

/**
 *  Test : Tick With Default Continue Operator - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, Tick_With_Default_Continue_Operator_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;
    ctx.Init();
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>([](IContext &)
                                                         { return TaskStatus::Continue; });

    task2->SetOperator(f);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    auto currentTask = planner.GetCurrentTask();

    EXPECT_TRUE(currentTask != nullptr);
    EXPECT_TRUE(planner.LastStatus() == TaskStatus::Continue);
}

/**
 *  Test : On New Plan - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_New_Plan_Validated)
{
    DomainTestContext ctx;
    Domain domain("Test");
    Planner planner;
    bool test = false;
    ctx.Init();
    planner.OnNewPlan = [&](TaskQueueType p)
    { test = (p.size() == 1); };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>([](IContext &)
                                                         { return TaskStatus::Continue; });
    task2->SetOperator(f);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Replace Plan - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Replace_Plan_Validated)
{
    bool validation_test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnReplacePlan = [&](TaskQueueType op, SharedPtr<ITask> ct, TaskQueueType p)
    {
        validation_test = ((op.size() == 0) && (ct != nullptr) && (p.size() == 1));
    };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    task3->AddCondition(c);
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    SharedPtr<IOperator> f1 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });

    task3->SetOperator(f1);
    task4->SetOperator(f2);
    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);

    ctx.Done() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    ctx.Done() = false;
    ctx.IsDirty() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(validation_test);
}

/**
 *  Test : On New Task - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_New_Task_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnNewTask = [&](SharedPtr<ITask> &t)
    { test = (t->Name() == "Sub-task"); };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>([](IContext &)
                                                         { return TaskStatus::Continue; });

    task2->SetOperator(f);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);
    EXPECT_TRUE(test);
}

/**
 *  Test : On New Task Condition Failed - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_New_Task_Condition_Failed_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnNewTaskConditionFailed = [&](SharedPtr<ITask> &t, SharedPtr<ICondition> &)
    {
        test = (t->Name() == "Sub-task1");
    };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    task3->AddCondition(c);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    SharedPtr<IOperator> f = MakeSharedPtr<FuncOperator>([](IContext &)
                                                         { return TaskStatus::Success; });
    task3->SetOperator(f);
    // Note that one should not use AddEffect on types that's not part of WorldState unless you
    // know what you're doing. Outside of the WorldState, we don't get automatic trimming of
    // state change. This method is used here only to invoke the desired callback, not because
    // its correct practice.
    SharedPtr<IEffect> effect =
        MakeSharedPtr<ActionEffect>("TestEffect", FluidEffectType::PlanAndExecute, [](IContext &context, FluidEffectType)
                                    { static_cast<DomainTestContext &>(context).Done() = true; });
    task3->AddEffect(effect);

    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task4->SetOperator(f2);
    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);

    ctx.Done() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    ctx.Done() = false;
    ctx.IsDirty() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Stop Current Task - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Stop_Current_Task_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnStopCurrentTask = [&](SharedPtr<PrimitiveTask> &t)
    { test = (t->Name() == "Sub-task2"); };

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    task3->AddCondition(c);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    SharedPtr<IOperator> f1 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task3->SetOperator(f1);
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task4->SetOperator(f2);

    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);

    ctx.Done() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    ctx.Done() = false;
    ctx.IsDirty() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Current Task Completed Successfully - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Current_Task_Completed_Successfully_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnCurrentTaskCompletedSuccessfully = [&](SharedPtr<PrimitiveTask> &t)
    {
        test = (t->Name() == "Sub-task1");
    };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    task3->AddCondition(c);
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    SharedPtr<IOperator> f1 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Success; });
    task3->SetOperator(f1);
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task4->SetOperator(f2);

    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);

    ctx.Done() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    ctx.Done() = false;
    ctx.IsDirty() = true;
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Apply Effect - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Apply_Effect_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnApplyEffect = [&](SharedPtr<IEffect> &e)
    { test = (e->Name() == "TestEffect"); };

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &context)
                                                           {
                uint8_t trudat = 1;
                return static_cast<DomainTestContext&>(context).HasState(
                    DomainTestState::HasA,
                    trudat); });

    task3->AddCondition(c);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    SharedPtr<IOperator> f1 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Success; });
    task3->SetOperator(f1);

    SharedPtr<IEffect> eff =
        MakeSharedPtr<ActionEffect>("TestEffect", FluidEffectType::PlanAndExecute, [](IContext &context, FluidEffectType type)
                                    { static_cast<DomainTestContext &>(context).SetState(DomainTestState::HasA, true, true, type); });

    task3->AddEffect(eff);

    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task4->SetOperator(f2);

    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);

    ctx.SetContextState(ContextState::Executing);
    ctx.SetState(DomainTestState::HasA, true, true, FluidEffectType::Permanent);
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    ctx.SetContextState(ContextState::Executing);
    ctx.SetState(DomainTestState::HasA, false, true, FluidEffectType::Permanent);
    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Current Task Failed - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Current_Task_Failed_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnCurrentTaskFailed = [&](SharedPtr<PrimitiveTask> &t)
    { test = (t->Name() == "Sub-task"); };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");

    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Failure; });

    task2->SetOperator(f2);

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Current Task Continues - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Current_Task_Continues_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnCurrentTaskContinues = [&](SharedPtr<PrimitiveTask> &t)
    { test = (t->Name() == "Sub-task"); };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");

    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });

    task2->SetOperator(f2);

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}

/**
 *  Test : On Current Task Executing Condition Failed - Validated
 *
 *  Description :
 *
 */

TEST(PlannerTests, On_Current_Task_Executing_Condition_Failed_Validated)
{
    bool test = false;
    Domain domain("Test");
    DomainTestContext ctx;
    Planner planner;
    ctx.Init();
    planner.OnCurrentTaskExecutingConditionFailed = [&](SharedPtr<PrimitiveTask> &t, SharedPtr<ICondition> &c)
    {
        test = ((t->Name() == "Sub-task") && (c->Name() == "TestCondition"));
    };
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test1");

    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    SharedPtr<IOperator> f2 = MakeSharedPtr<FuncOperator>([](IContext &)
                                                          { return TaskStatus::Continue; });
    task2->SetOperator(f2);

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &context)
                                                           { return static_cast<DomainTestContext &>(context).Done(); });

    task2->AddExecutingCondition(c);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    planner.Tick<DomainTestState, uint8_t, DomainTestWorldState>(domain, ctx);

    EXPECT_TRUE(test);
}