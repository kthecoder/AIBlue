#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Domain.h"
#include "Tasks/Task.h"
#include "Tasks/CompoundTasks/CompoundTask.h"
#include "Tasks/PrimitiveTasks/PrimitiveTask.h"
#include "Tasks/CompoundTasks/PausePlanTask.h"
#include "Tasks/CompoundTasks/Selector.h"
#include "Tasks/CompoundTasks/Sequence.h"
#include "Tasks/CompoundTasks/DecompositionStatus.h"
#include "Effects/Effect.h"
#include "DomainContextTester.h"

/**
 *  Test : Domain Roots & has Name - Validated
 *
 *  Description : Creating a Domain should setup a Root with a Domain name
 *
 */

TEST(DomainTests, Domain_Roots_And_Has_Name_Validated)
{
    Domain domain("My Domain");

    EXPECT_TRUE(domain.Root() != nullptr);
    EXPECT_TRUE(domain.Root()->Name() == "My Domain");
}

/**
 *  Test : Add Subtask to Parent - Validated
 *
 *  Description : Validate that adding a Task works
 *
 */

TEST(DomainTests, Add_Subtask_to_Parent_Validated)
{
    Domain domain("My Domain");

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("Test");
    SharedPtr<ITask> task2 = MakeSharedPtr<Selector>("Test2");
    domain.Add(task1, task2);
    // EXPECT_TRUE(std::find(task1->Subtasks().begin(), task1->Subtasks().end(), task2) != task1->Subtasks().end());
    EXPECT_TRUE(task2->Parent().get() == task1.get());
}

/**
 *  Test : No Context Find Plan Invalid - Exception
 *
 *  Description : How can you plan without a plan?
 *
 */

TEST(DomainTests, No_Context_Find_Plan_Invalid)
{
    auto domain = MakeSharedPtr<Domain>("My Domain");
    SharedPtr<BaseContextType> ctx = MakeSharedPtr<DomainTestContext>();

    EXPECT_THROW({
        TaskQueueType plan;
        domain->FindPlan(*ctx, plan);
    },
                 std::exception);
}

/**
 *  Test : Find Plan with no items is Null - Validated
 *
 *  Description : A plan with no items returns no plan, DecompositionStatus::Rejected
 *
 */

TEST(DomainTests, Find_Plan_with_no_items_is_Null_Validated)
{
    SharedPtr<BaseContextType> ctx = MakeSharedPtr<DomainTestContext>();

    Domain domain("My Domain");
    TaskQueueType plan;

    ctx->Init();

    auto status = domain.FindPlan(*ctx, plan);
    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test : Plan Found Context is Executing - Validated
 *
 *  Description : After Domain.findPlan, move on to context -> state is executing
 *
 */

TEST(DomainTests, Plan_Found_Context_is_Executing_Validated)
{
    SharedPtr<BaseContextType> ctx = MakeSharedPtr<DomainTestContext>();
    Domain domain("My Domain");
    TaskQueueType plan;

    ctx->Init();

    domain.FindPlan(*ctx, plan);
    EXPECT_TRUE(ctx->GetContextState() == ContextState::Executing);
}

/**
 *  Test : Find Plan - Validated
 *
 *  Description : DecompositionStatus::Succeeded, a plan is found with 1 item
 *
 */

TEST(DomainTests, Find_Plan)
{
    SharedPtr<BaseContextType> bctx = MakeSharedPtr<DomainTestContext>();
    Domain domain("My Domain");
    TaskQueueType plan;

    bctx->Init();

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Selector>("My Selector");

    SharedPtr<ITask> task2 = MakeSharedPtr<PrimitiveTask>("My Sub-task");

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_TRUE(plan.front()->Name() == "My Sub-task");
}

/**
 *  Test : Find Plan Trims Non-Permanent State Change - Validated
 *
 *  Description : Finds a plan after having trimmed the Non-Permanent Action Effects
 *
 */

TEST(DomainTests, Find_Plan_Trims_Non_Permanent_State_Change_Validated)
{
    SharedPtr<BaseContextType> bctx = MakeSharedPtr<DomainTestContext>();
    Domain domain("Test");
    TaskQueueType plan;
    bctx->Init();
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Sequence>("Test");

    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<IEffect> effect1 =
        MakeSharedPtr<ActionEffect>("TestEffect1", FluidEffectType::PlanOnly, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasA, true, true, t); });
    task2->AddEffect(effect1);

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");
    SharedPtr<IEffect> effect2 =
        MakeSharedPtr<ActionEffect>("TestEffect2", FluidEffectType::PlanAndExecute, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasB, true, true, t); });
    task3->AddEffect(effect2);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task3");
    SharedPtr<IEffect> effect3 =
        MakeSharedPtr<ActionEffect>("TestEffect3", FluidEffectType::Permanent, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasC, true, true, t); });
    task4->AddEffect(effect3);
    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);
    domain.Add(task1, task3);
    domain.Add(task1, task4);
    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 0);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 0);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasA) == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasB) == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasC) == 1);
    EXPECT_TRUE(plan.size() == 3);
}

/**
 *  Test : Find Plan Clears State Change When Plan is Null - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Find_Plan_Clears_State_Change_When_Plan_is_Null_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);

    Domain domain("My Domain");
    TaskQueueType plan;

    bctx->Init();

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Sequence>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<IEffect> effect1 =
        MakeSharedPtr<ActionEffect>("TestEffect1", FluidEffectType::PlanOnly, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasA, true, true, t); });
    task2->AddEffect(effect1);

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");
    SharedPtr<IEffect> effect2 =
        MakeSharedPtr<ActionEffect>("TestEffect2", FluidEffectType::PlanAndExecute, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasB, true, true, t); });
    task3->AddEffect(effect2);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task3");
    SharedPtr<IEffect> effect3 =
        MakeSharedPtr<ActionEffect>("TestEffect3", FluidEffectType::Permanent, [=](IContext &ctx, FluidEffectType t)
                                    { static_cast<BaseContextType &>(ctx).SetState(DomainTestState::HasC, true, true, t); });
    task4->AddEffect(effect3);

    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task4");
    SharedPtr<ICondition> condition = MakeSharedPtr<FuncCondition>("TestCondition", [=](IContext &ctx)
                                                                   {
        DomainTestContext& d = (DomainTestContext&)ctx;
        return (d.Done() == true); });
    task5->AddCondition(condition);

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);
    domain.Add(task1, task3);
    domain.Add(task1, task4);
    domain.Add(task1, task5);
    auto status = domain.FindPlan(*bctx, plan);
    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 0);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 0);
    EXPECT_TRUE(bctx->GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasA) == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasB) == 0);
    EXPECT_TRUE(bctx->GetWorldState().GetState(DomainTestState::HasC) == 0);
    EXPECT_TRUE(plan.size() == 0);
}

/**
 *  Test : Find Plan if MTRs are Equal then return Null Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Find_Plan_if_MTRs_are_Equal_then_return_Null_Plan)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("Test");
    TaskQueueType plan;
    bctx->Init();
    ctx->LastMTR().Add(1);

    // Root is a Selector that branch off into task1 selector or task2 sequence.
    // MTR only tracks decomposition of compound tasks, so our MTR is only 1 layer deep here,
    // Since both compound tasks decompose into primitive tasks.
    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Sequence>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<ICondition> condition = MakeSharedPtr<FuncCondition>("TestCondition", [=](IContext &ctx)
                                                                   {
        DomainTestContext& d = (DomainTestContext&)ctx;
        return (d.Done() == true); });
    task3->AddCondition(condition);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");

    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");
    SharedPtr<ICondition> condition2 = MakeSharedPtr<FuncCondition>("TestCondition", [=](IContext &ctx)
                                                                    {
        DomainTestContext& d = (DomainTestContext&)ctx;
        return (d.Done() == true); });
    task5->AddCondition(condition);

    domain.Add(domain.Root(), task1);
    domain.Add(domain.Root(), task2);
    domain.Add(task1, task3);
    domain.Add(task2, task4);
    domain.Add(task2, task5);
    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx->MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx->MethodTraversalRecord()[0] == ctx->LastMTR()[0]);
}

/**
 *  Test : Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Pause_Plan_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("My Domain");
    TaskQueueType plan;
    bctx->Init();

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Sequence>("My Compound Task");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");

    SharedPtr<ITask> task4 = MakeSharedPtr<PausePlanTask>();

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);
    domain.Add(task1, task4);
    domain.Add(task1, task3);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx->HasPausedPartialPlan());
    EXPECT_TRUE(ctx->PartialPlanQueue().size() == 1);
    auto tx = StaticCastPtr<ITask>(task1);
    ITask *t1ptr = tx.get();
    ITask *t2ptr = ctx->PartialPlanQueue().front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(2, ctx->PartialPlanQueue().front().TaskIndex);
}

/**
 *  Test : Continue Paused Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Continue_Paused_Plan_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("My Domain");
    TaskQueueType plan;
    bctx->Init();

    SharedPtr<CompoundTask> task1 = MakeSharedPtr<Sequence>("Sequence Task 1");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");

    SharedPtr<ITask> task4 = MakeSharedPtr<PausePlanTask>();

    domain.Add(domain.Root(), task1);
    domain.Add(task1, task2);
    domain.Add(task1, task4);
    domain.Add(task1, task3);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx->HasPausedPartialPlan());
    EXPECT_TRUE(ctx->PartialPlanQueue().size() == 1);
    auto tx = StaticCastPtr<ITask>(task1);
    ITask *t1ptr = tx.get();
    ITask *t2ptr = ctx->PartialPlanQueue().front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(2, ctx->PartialPlanQueue().front().TaskIndex);

    status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive Sub-task2", plan.front()->Name());
}

/**
 *  Test : Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Nested_Pause_Plan_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("My Domain");
    TaskQueueType plan;
    bctx->Init();
    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("My Sequence");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("My Selector");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("My Sequence");

    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("My Primitive My Primitive Sub-task4");
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("My Primitive My Primitive Sub-task3");
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("My Primitive My Primitive Sub-task2");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("My Primitive My Primitive Sub-task1");
    SharedPtr<ITask> pausePlan = MakeSharedPtr<PausePlanTask>();

    domain.Add(domain.Root(), task);
    domain.Add(task, task2);
    domain.Add(task, subtask4);

    domain.Add(task2, task3);
    domain.Add(task2, subtask3);

    domain.Add(task3, subtask1);
    domain.Add(task3, pausePlan);
    domain.Add(task3, subtask2);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive My Primitive Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx->HasPausedPartialPlan());
    EXPECT_TRUE(ctx->PartialPlanQueue().size() == 2);

    auto theQueue = ctx->PartialPlanQueue();

    ITask *t1ptr = task3.get();
    ITask *t2ptr = theQueue.front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(2, theQueue.front().TaskIndex);

    theQueue.pop();
    t1ptr = task.get();
    t2ptr = theQueue.front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(1, theQueue.front().TaskIndex);
}

/**
 *  Test : Continue Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Continue_Nested_Pause_Plan_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("My Domain");
    TaskQueueType plan;
    bctx->Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Sequence Task 1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Selector Task 2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Sequence Task 3");

    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task4");
    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task3");
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<ITask> pausePlan = MakeSharedPtr<PausePlanTask>();

    domain.Add(domain.Root(), task);
    domain.Add(task, task2);
    domain.Add(task, subtask4);

    domain.Add(task2, task3);
    domain.Add(task2, subtask3);

    domain.Add(task3, subtask1);
    domain.Add(task3, pausePlan);
    domain.Add(task3, subtask2);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx->HasPausedPartialPlan());
    EXPECT_TRUE(ctx->PartialPlanQueue().size() == 2);

    PartialPlanQueueType queueCopy = ctx->PartialPlanQueue();
    ITask *t1ptr = task3.get();
    ITask *t2ptr = queueCopy.front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(2, queueCopy.front().TaskIndex);

    queueCopy.pop();
    t1ptr = task.get();
    t2ptr = queueCopy.front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(1, queueCopy.front().TaskIndex);

    status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("My Primitive Sub-task2", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("My Primitive Sub-task4", plan.front()->Name());
}

/**
 *  Test : Continue Multi-Nested Pause Plan - Validated
 *
 *  Description :
 *
 */

TEST(DomainTests, Continue_Multi_Nested_Pause_Plan_Validated)
{
    auto bctx = MakeSharedPtr<DomainTestContext>();
    SharedPtr<IContext> ctx = StaticCastPtr<IContext>(bctx);
    Domain domain("My Domain");
    TaskQueueType plan;
    bctx->Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Sequence>("Sequence Task 1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Selector Task 2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Sequence>("Sequence Task 3");
    SharedPtr<CompoundTask> task4 = MakeSharedPtr<Sequence>("Sequence Task 4");

    domain.Add(domain.Root(), task);
    SharedPtr<PrimitiveTask> subtask1 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task1");
    SharedPtr<ITask> pausePlan1 = MakeSharedPtr<PausePlanTask>();
    SharedPtr<PrimitiveTask> subtask2 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task2");
    domain.Add(task3, subtask1);
    domain.Add(task3, pausePlan1);
    domain.Add(task3, subtask2);

    SharedPtr<PrimitiveTask> subtask3 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task3");
    domain.Add(task2, task3);
    domain.Add(task2, subtask3);

    SharedPtr<PrimitiveTask> subtask5 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task5");
    SharedPtr<ITask> pausePlan2 = MakeSharedPtr<PausePlanTask>();
    SharedPtr<PrimitiveTask> subtask6 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task6");
    domain.Add(task4, subtask5);
    domain.Add(task4, pausePlan2);
    domain.Add(task4, subtask6);

    domain.Add(task, task2);
    SharedPtr<PrimitiveTask> subtask4 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task4");
    domain.Add(task, subtask4);
    domain.Add(task, task4);
    SharedPtr<PrimitiveTask> subtask7 = MakeSharedPtr<PrimitiveTask>("My Primitive Sub-task7");
    domain.Add(task, subtask7);

    auto status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("My Primitive Sub-task1", plan.front()->Name());
    EXPECT_TRUE(ctx->HasPausedPartialPlan());
    EXPECT_TRUE(ctx->PartialPlanQueue().size() == 2);

    PartialPlanQueueType queueCopy = ctx->PartialPlanQueue();

    ITask *t1ptr = task3.get();
    ITask *t2ptr = queueCopy.front().Task.get();
    EXPECT_EQ(t1ptr, t2ptr);
    EXPECT_EQ(2, queueCopy.front().TaskIndex);
    queueCopy.pop();
    t1ptr = task.get();
    EXPECT_EQ(t1ptr, queueCopy.front().Task.get());
    EXPECT_EQ(1, queueCopy.front().TaskIndex);

    status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Partial);
    EXPECT_TRUE(plan.size() == 3);
    EXPECT_EQ("My Primitive Sub-task2", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("My Primitive Sub-task4", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("My Primitive Sub-task5", plan.front()->Name());

    status = domain.FindPlan(*bctx, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 2);
    EXPECT_EQ("My Primitive Sub-task6", plan.front()->Name());
    plan.pop();
    EXPECT_EQ("My Primitive Sub-task7", plan.front()->Name());
}

/**
 *  Test :
 *
 *  Description :
 *
 */

// TEST(DomainTests, TBD)
// {
//
// }
