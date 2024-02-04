#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Domain.h"
#include "Planners/Planner.h"
#include "Tasks/CompoundTasks/Sequence.h"
#include "Tasks/CompoundTasks/Selector.h"
#include "Tasks/PrimitiveTasks/PrimitiveTask.h"
#include "Tasks/CompoundTasks/PausePlanTask.h"
#include "Effects/Effect.h"
#include "DomainContextTester.h"

/**
 *  Test : Add Condition - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Add_Condition_Validated)
{
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("TestCondition", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    bool bRet = task->AddCondition(c);
    EXPECT_TRUE(bRet);
    EXPECT_TRUE(task->Conditions().size() == 1);
}

/**
 *  Test : Add Subtask - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Add_Subtask_Validated)
{
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    bool bRet = task->AddSubTask(task2);

    EXPECT_TRUE(bRet);
    EXPECT_TRUE(task->Subtasks().size() == 1);
}

/**
 *  Test : Is Valid Fails Without Subtasks - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Is_Valid_Fails_Without_Subtasks_Validated)
{
    DomainTestContext ctx;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");

    EXPECT_FALSE(task->IsValid(ctx));
}

/**
 *  Test : Is Valid - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Is_Valid_Validated)
{
    DomainTestContext ctx;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    task->AddSubTask(task2);

    EXPECT_TRUE(task->IsValid(ctx));
}

/**
 *  Test : Decompose Requires Context Init Fails - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Requires_Context_Init_Fails_Validated)
{
    DomainTestContext ctx;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");

    EXPECT_THROW({
        TaskQueueType plan;
        task->Decompose(ctx, 0, plan);
    },
                 std::exception);
}

/**
 *  Test : Decompose With No Subtasks - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_With_No_Subtasks_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Failed);
    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test : Decompose With Subtasks - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_With_Subtasks_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
}

/**
 *  Test : Decompose Nested Subtasks - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Subtasks_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task4->AddCondition(c);
    task3->AddSubTask(task4);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(task5);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> task6 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task2->AddSubTask(task6);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task7 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(task7);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task4", plan.front()->Name());
}

/**
 *  Test : Decompose With Subtasks One Fail - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_With_Subtasks_One_Fail_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Failed);
    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test : Decompose With Subtasks Compound Subtask Fails - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_With_Subtasks_Compound_Subtask_Fails_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task->AddSubTask(task2);
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Failed);
    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test : Decompose Failure Return To Previous World State - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Failure_Return_To_Previous_World_State_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);
    ctx.SetState(DomainTestState::HasA, true, true, FluidEffectType::PlanAndExecute);
    ctx.SetState(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    ctx.SetState(DomainTestState::HasC, true, true, FluidEffectType::PlanOnly);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<IEffect> eff =
        MakeSharedPtr<ActionEffect>("TestEffect", FluidEffectType::Permanent, [](IContext &context, FluidEffectType)
                                    { static_cast<DomainTestContext &>(context).SetState(DomainTestState::HasA,
                                                                                         false,
                                                                                         true,
                                                                                         FluidEffectType::PlanOnly); });
    task2->AddEffect(eff);
    task->AddSubTask(task2);
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Sub-task2");
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Failed);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 1);
    EXPECT_EQ(1, (int)ctx.GetStateDTS(DomainTestState::HasA));
    EXPECT_EQ(1, (int)ctx.GetStateDTS(DomainTestState::HasB));
    EXPECT_EQ(1, (int)ctx.GetStateDTS(DomainTestState::HasC));
}

/**
 *  Test : Decompose Nested Compound Subtask Lose To MTR - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Compound_Subtask_Lose_To_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task4->AddCondition(c);
    task3->AddSubTask(task4);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(subtask2);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task2->AddSubTask(subtask3);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(subtask4);

    ctx.LastMTR().Add(0);
    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 2);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[1] == -1);
}

/**
 *  Test : Decompose Nested Compound Subtask Lose To MTR2 - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Compound_Subtask_Lose_To_MTR2_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task4->AddCondition(c);
    task3->AddSubTask(task4);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(subtask2);

    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    subtask3->AddCondition(c);
    task2->AddSubTask(subtask3);
    task2->AddSubTask(task3);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(subtask4);

    ctx.LastMTR().Add(1);
    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 2);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[1] == -1);
}

/**
 *  Test : Decompose Nested Compound Subtask Equal To MTR - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Compound_Subtask_Equal_To_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    subtask2->AddCondition(c);
    task3->AddSubTask(subtask2);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task3->AddSubTask(subtask3);

    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    subtask1->AddCondition(c);

    task2->AddSubTask(subtask1);
    task2->AddSubTask(task3);

    task->AddSubTask(task2);

    SharedPtr<PrimitiveTask> task6 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(task6);

    ctx.LastMTR().Add(1);
    ctx.LastMTR().Add(1);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 2);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 1);
    EXPECT_EQ("Sub-task3", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task4", plan.front()->Name());
}

/**
 *  Test : Decompose Nested Compound Subtask Lose To MTR Return To Previous World State - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Compound_Subtask_Lose_To_MTR_Return_To_Previous_World_State_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);
    ctx.SetState(DomainTestState::HasA, true, true, FluidEffectType::PlanAndExecute);
    ctx.SetState(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    ctx.SetState(DomainTestState::HasC, true, true, FluidEffectType::PlanOnly);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    subtask2->AddCondition(c);
    task3->AddSubTask(subtask2);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    SharedPtr<IEffect> eff =
        MakeSharedPtr<ActionEffect>("TestEffect", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasA, false, true, FluidEffectType::PlanOnly); });
    subtask3->AddEffect(eff);
    task3->AddSubTask(subtask3);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    SharedPtr<IEffect> eff2 =
        MakeSharedPtr<ActionEffect>("TestEffect2", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasB, false, true, FluidEffectType::PlanOnly); });
    subtask4->AddEffect(eff2);
    task2->AddSubTask(subtask4);

    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    subtask1->AddEffect(eff);
    task->AddSubTask(subtask1);
    task->AddSubTask(task2);

    SharedPtr<PrimitiveTask> subtask5 = MakeSharedPtr<PrimitiveTask>("Sub-task5");
    SharedPtr<IEffect> eff3 =
        MakeSharedPtr<ActionEffect>("TestEffect3", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasC, false, true, FluidEffectType::PlanOnly); });
    subtask5->AddEffect(eff3);
    task->AddSubTask(subtask5);

    ctx.LastMTR().Add(0);
    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 2);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[1] == -1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 1);
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasA));
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasB));
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasC));
}

/**
 *  Test : Decompose Nested Compound Subtask Fail Return To Previous World State - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Decompose_Nested_Compound_Subtask_Fail_Return_To_Previous_World_State_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();
    ctx.SetContextState(ContextState::Planning);
    ctx.SetState(DomainTestState::HasA, true, true, FluidEffectType::PlanAndExecute);
    ctx.SetState(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    ctx.SetState(DomainTestState::HasC, true, true, FluidEffectType::PlanOnly);

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Sequence>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Test3");
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    subtask2->AddCondition(c);
    task3->AddSubTask(subtask2);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    SharedPtr<IEffect> eff =
        MakeSharedPtr<ActionEffect>("TestEffect", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasA, false, true, FluidEffectType::PlanOnly); });
    subtask3->AddEffect(eff);
    task3->AddSubTask(subtask3);
    task2->AddSubTask(task3);

    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    SharedPtr<IEffect> eff2 =
        MakeSharedPtr<ActionEffect>("TestEffect2", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasB, false, true, FluidEffectType::PlanOnly); });
    subtask4->AddEffect(eff2);

    task2->AddSubTask(subtask4);

    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    subtask1->AddEffect(eff);
    task->AddSubTask(subtask1);

    task->AddSubTask(task2);

    SharedPtr<PrimitiveTask> subtask5 = MakeSharedPtr<PrimitiveTask>("Sub-task5");
    SharedPtr<IEffect> eff3 =
        MakeSharedPtr<ActionEffect>("TestEffect3", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType)
                                    { static_cast<DomainTestContext &>(ctx).SetState(DomainTestState::HasC, false, true, FluidEffectType::PlanOnly); });
    subtask5->AddEffect(eff3);
    task->AddSubTask(subtask5);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Failed);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 1);
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasA));
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasB));
    EXPECT_EQ(1, (int)ctx.GetState(DomainTestState::HasC));
}

/**
 *  Test : Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Pause_Plan_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task->AddSubTask(subtask1);
    SharedPtr<PausePlanTask> pause = MakeSharedPtr<PausePlanTask>();
    task->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task->AddSubTask(subtask2);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx.HasPausedPartialPlan());
    EXPECT_TRUE(ctx.PartialPlanQueue().size() == 1);
    ITask *tptr1 = static_cast<ITask *>(task.get());
    EXPECT_EQ(tptr1, ctx.PartialPlanQueue().front().Task.get());
    EXPECT_EQ(2, ctx.PartialPlanQueue().front().TaskIndex);
}

/**
 *  Test : Continue Paused Plan - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Continue_Paused_Plan_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task->AddSubTask(subtask1);
    SharedPtr<PausePlanTask> pause = MakeSharedPtr<PausePlanTask>();
    task->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task->AddSubTask(subtask2);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx.HasPausedPartialPlan());
    EXPECT_TRUE(ctx.PartialPlanQueue().size() == 1);
    ITask *tptr1 = static_cast<ITask *>(task.get());
    EXPECT_EQ(tptr1, ctx.PartialPlanQueue().front().Task.get());
    EXPECT_EQ(2, ctx.PartialPlanQueue().front().TaskIndex);

    ctx.HasPausedPartialPlan() = false;
    plan = TaskQueueType();
    while (ctx.PartialPlanQueue().size() > 0)
    {
        auto kvp = ctx.PartialPlanQueue().front();
        ctx.PartialPlanQueue().pop();
        TaskQueueType p;
        auto s = StaticCastPtr<CompoundTask>(kvp.Task)->Decompose(ctx, kvp.TaskIndex, p);
        if (s == DecompositionStatus::Succeeded || s == DecompositionStatus::Partial)
        {
            while (p.size() > 0)
            {
                plan.push(p.front());
                p.pop();
            }
        }
    }
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
}

/**
 *  Test : Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Nested_Pause_Plan_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Test3");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task3->AddSubTask(subtask1);
    SharedPtr<PausePlanTask> pause = MakeSharedPtr<PausePlanTask>();
    task3->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(subtask2);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task2->AddSubTask(subtask3);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(subtask4);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx.HasPausedPartialPlan());
    EXPECT_TRUE(ctx.PartialPlanQueue().size() == 2);
    PartialPlanQueueType queueCopy = ctx.PartialPlanQueue();
    ITask *tptr1 = static_cast<ITask *>(task3.get());
    EXPECT_EQ(tptr1, queueCopy.front().Task.get());
    EXPECT_EQ(2, queueCopy.front().TaskIndex);
    queueCopy.pop();
    EXPECT_EQ(static_cast<ITask *>(task.get()), queueCopy.front().Task.get());
    EXPECT_EQ(1, queueCopy.front().TaskIndex);
}

/**
 *  Test : Continue Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Continue_Nested_Pause_Plan_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Test3");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task3->AddSubTask(subtask1);
    SharedPtr<PausePlanTask> pause = MakeSharedPtr<PausePlanTask>();
    task3->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(subtask2);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task2->AddSubTask(subtask3);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(subtask4);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx.HasPausedPartialPlan());
    EXPECT_TRUE(ctx.PartialPlanQueue().size() == 2);
    PartialPlanQueueType queueCopy = ctx.PartialPlanQueue();
    ITask *tptr1 = static_cast<ITask *>(task3.get());
    EXPECT_EQ(tptr1, queueCopy.front().Task.get());
    EXPECT_EQ(2, queueCopy.front().TaskIndex);
    queueCopy.pop();
    EXPECT_EQ(static_cast<ITask *>(task.get()), queueCopy.front().Task.get());
    EXPECT_EQ(1, queueCopy.front().TaskIndex);

    ctx.HasPausedPartialPlan() = false;
    plan = TaskQueueType();
    while (ctx.PartialPlanQueue().size() > 0)
    {
        auto kvp = ctx.PartialPlanQueue().front();
        ctx.PartialPlanQueue().pop();
        TaskQueueType p;
        auto s = StaticCastPtr<CompoundTask>(kvp.Task)->Decompose(ctx, kvp.TaskIndex, p);

        if (s == DecompositionStatus::Succeeded || s == DecompositionStatus::Partial)
        {
            while (p.size() > 0)
            {
                plan.push(p.front());
                p.pop();
            }
        }

        if (ctx.HasPausedPartialPlan())
            break;
    }

    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task4", plan.front()->Name());
}

/**
 *  Test : Continue Multiple Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(SequenceTests, Continue_Multiple_Nested_Pause_Plan_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Test3");
    SharedPtr<CompoundTask> task4 = MakeSharedPtr<Sequence>("Test3");

    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    task3->AddSubTask(subtask1);
    SharedPtr<PausePlanTask> pause = MakeSharedPtr<PausePlanTask>();
    task3->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task3->AddSubTask(subtask2);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task2->AddSubTask(subtask3);

    SharedPtr<PrimitiveTask> subtask5 = MakeSharedPtr<PrimitiveTask>("Sub-task5");
    task4->AddSubTask(subtask5);
    task4->AddSubTask(pause);
    SharedPtr<PrimitiveTask> subtask6 = MakeSharedPtr<PrimitiveTask>("Sub-task6");
    task4->AddSubTask(subtask6);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(subtask4);
    task->AddSubTask(task4);
    SharedPtr<PrimitiveTask> subtask7 = MakeSharedPtr<PrimitiveTask>("Sub-task7");
    task->AddSubTask(subtask7);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx.HasPausedPartialPlan());
    EXPECT_TRUE(ctx.PartialPlanQueue().size() == 2);
    PartialPlanQueueType queueCopy = ctx.PartialPlanQueue();
    ITask *tptr1 = static_cast<ITask *>(task3.get());
    EXPECT_EQ(tptr1, queueCopy.front().Task.get());
    EXPECT_EQ(2, queueCopy.front().TaskIndex);
    queueCopy.pop();
    EXPECT_EQ(static_cast<ITask *>(task.get()), queueCopy.front().Task.get());
    EXPECT_EQ(1, queueCopy.front().TaskIndex);

    ctx.HasPausedPartialPlan() = false;
    plan = TaskQueueType();
    while (ctx.PartialPlanQueue().size() > 0)
    {
        auto kvp = ctx.PartialPlanQueue().front();
        ctx.PartialPlanQueue().pop();
        TaskQueueType p;
        auto s = StaticCastPtr<CompoundTask>(kvp.Task)->Decompose(ctx, kvp.TaskIndex, p);

        if (s == DecompositionStatus::Succeeded || s == DecompositionStatus::Partial)
        {
            while (p.size() > 0)
            {
                plan.push(p.front());
                p.pop();
            }
        }

        if (ctx.HasPausedPartialPlan())
            break;
    }

    EXPECT_TRUE(plan.size() == 3);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task4", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task5", plan.front()->Name());

    ctx.HasPausedPartialPlan() = false;
    plan = TaskQueueType();
    while (ctx.PartialPlanQueue().size() > 0)
    {
        auto kvp = ctx.PartialPlanQueue().front();
        ctx.PartialPlanQueue().pop();
        TaskQueueType p;
        auto s = StaticCastPtr<CompoundTask>(kvp.Task)->Decompose(ctx, kvp.TaskIndex, p);

        if (s == DecompositionStatus::Succeeded || s == DecompositionStatus::Partial)
        {
            while (p.size() > 0)
            {
                plan.push(p.front());
                p.pop();
            }
        }

        if (ctx.HasPausedPartialPlan())
            break;
    }

    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("Sub-task6", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("Sub-task7", plan.front()->Name());
}