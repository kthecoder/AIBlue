#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Domain.h"
#include "Planners/Planner.h"
#include "Tasks/CompoundTasks/Selector.h"
#include "Tasks/PrimitiveTasks/PrimitiveTask.h"
#include "DomainContextTester.h"

/**
 *  Test : Add Condition - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Add_Condition_Validated)
{
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
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

TEST(SelectorTests, Add_Subtask_Validated)
{
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
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

TEST(SelectorTests, Is_Valid_Fails_Without_Subtasks_Validated)
{
    DomainTestContext ctx;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");

    EXPECT_FALSE(task->IsValid(ctx));
}

/**
 *  Test : Is Valid - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Is_Valid_Validated)
{
    DomainTestContext ctx;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task");
    task->AddSubTask(task2);

    EXPECT_TRUE(task->IsValid(ctx));
}

/**
 *  Test : Decompose With No Subtasks - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_With_No_Subtasks_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
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

TEST(SelectorTests, Decompose_With_Subtasks_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task1", plan.front()->Name());
}

/**
 *  Test : Decompose With Subtasks 2 - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_With_Subtasks_2_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Sub-task1");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
}

/**
 *  Test : Decompose With Subtasks 3 - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_With_Subtasks_3_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });

    task2->AddCondition(c);

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
}

/**
 *  Test : Decompose MTR Fails - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_MTR_Fails_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task2->AddCondition(c);

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    ctx.LastMTR().Add(0);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_EQ(-1, ctx.MethodTraversalRecord()[0]);
}

/**
 *  Test : Decompose Debug MTR Fails - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Debug_MTR_Fails_Validated)
{
    MyDebugContext ctx;
    TaskQueueType plan;
    ctx.Init();

    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task2->AddCondition(c);
    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task->AddSubTask(task3);

    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MTRDebug().size() == 1);
    EXPECT_TRUE(ctx.MTRDebug()[0].find("REPLAN FAIL") != StringType::npos);
    EXPECT_TRUE(ctx.MTRDebug()[0].find("Sub-task2") != StringType::npos);
}

/**
 *  Test : Decompose MTR Succeeds When Equal - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_MTR_Succeeds_When_Equal_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<PrimitiveTask> task2 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task2->AddCondition(c);

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task2");

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    ctx.LastMTR().Add(1);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 0);
    EXPECT_TRUE(plan.size() == 1);
}

/**
 *  Test : Decompose Compound Subtask Succeeds - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Succeeds_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");

    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);
    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task2->AddSubTask(task4);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task->AddSubTask(task5);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
}

/**
 *  Test : Decompose Compound Subtask Fails - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Fails_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");

    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    SharedPtr<ICondition> c2 = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                            { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task4->AddCondition(c2);

    task2->AddSubTask(task3);
    task2->AddSubTask(task4);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task->AddSubTask(task5);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task3", plan.front()->Name());
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 0);
}

/**
 *  Test : Decompose Nested Compound Subtask Fails - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Nested_Compound_Subtask_Fails_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task4->AddCondition(c);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task5->AddCondition(c);

    task3->AddSubTask(task4);
    task3->AddSubTask(task5);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> task6 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task6->AddCondition(c);
    task2->AddSubTask(task6);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task7 = MakeSharedPtr<PrimitiveTask>("Sub-task4");
    task->AddSubTask(task7);

    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task4", plan.front()->Name());
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 0);
}

/**
 *  Test : Decompose Compound Subtask Beats Last MTR - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Beats_Last_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);

    task2->AddSubTask(task3);
    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task2->AddSubTask(task4);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task->AddSubTask(task5);

    ctx.LastMTR().Add(1);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
}

/**
 *  Test : Decompose Compound Subtask Equal To Last MTR - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Equal_To_Last_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);
    task2->AddSubTask(task3);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task2->AddSubTask(task4);

    task->AddSubTask(task2);
    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task->AddSubTask(task5);

    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
    EXPECT_TRUE(plan.size() == 1);
    EXPECT_EQ("Sub-task2", plan.front()->Name());
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
}

/**
 *  Test : Decompose Compound Subtask Lose To Last MTR - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Lose_To_Last_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<PrimitiveTask> task3 = MakeSharedPtr<PrimitiveTask>("Sub-task1");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    task3->AddCondition(c);
    task2->AddSubTask(task3);

    SharedPtr<PrimitiveTask> task4 = MakeSharedPtr<PrimitiveTask>("Sub-task2");
    task2->AddSubTask(task4);

    SharedPtr<PrimitiveTask> task5 = MakeSharedPtr<PrimitiveTask>("Sub-task3");
    task5->AddCondition(c);

    task->AddSubTask(task5);
    task->AddSubTask(task2);

    ctx.LastMTR().Add(0);
    auto status = task->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == -1);
}

/**
 *  Test : Decompose Compound Subtask Win Over Last MTR - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Win_Over_Last_MTR_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> rootTask = MakeSharedPtr<Selector>("Root");
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> subTask31 = MakeSharedPtr<PrimitiveTask>("Sub-task3-1");
    SharedPtr<PrimitiveTask> subTask32 = MakeSharedPtr<PrimitiveTask>("Sub-task3-2");
    SharedPtr<PrimitiveTask> subTask21 = MakeSharedPtr<PrimitiveTask>("Sub-task2-1");
    SharedPtr<PrimitiveTask> subTask22 = MakeSharedPtr<PrimitiveTask>("Sub-task2-2");
    SharedPtr<PrimitiveTask> subTask11 = MakeSharedPtr<PrimitiveTask>("Sub-task1-1");
    SharedPtr<ICondition> ctrue = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                               { return (static_cast<DomainTestContext &>(ctx).Done() == true); });
    SharedPtr<ICondition> cfalse = MakeSharedPtr<FuncCondition>("Done == false", [](IContext &ctx)
                                                                { return (static_cast<DomainTestContext &>(ctx).Done() == false); });

    subTask31->AddCondition(ctrue);
    task3->AddSubTask(subTask31);
    task3->AddSubTask(subTask32);

    subTask21->AddCondition(ctrue);
    task2->AddSubTask(subTask21);
    task2->AddSubTask(subTask22);

    task->AddSubTask(task2);
    task->AddSubTask(task3);
    subTask11->AddCondition(cfalse);
    task->AddSubTask(subTask11);

    rootTask->AddSubTask(task);

    ctx.LastMTR().Add(0);
    ctx.LastMTR().Add(1);
    ctx.LastMTR().Add(0);

    // In this test, we prove that [0, 0, 1] beats [0, 1, 0]
    auto status = rootTask->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Succeeded);
}

/**
 *  Test : Decompose Compound Subtask Lose To Last MTR2 - Validated
 *
 *  Description :
 *
 */

TEST(SelectorTests, Decompose_Compound_Subtask_Lose_To_Last_MTR2_Validated)
{
    DomainTestContext ctx;
    TaskQueueType plan;
    SharedPtr<CompoundTask> rootTask = MakeSharedPtr<Selector>("Root");
    SharedPtr<CompoundTask> task = MakeSharedPtr<Selector>("Test1");
    SharedPtr<CompoundTask> task2 = MakeSharedPtr<Selector>("Test2");
    SharedPtr<CompoundTask> task3 = MakeSharedPtr<Selector>("Test3");
    SharedPtr<PrimitiveTask> subTask21 = MakeSharedPtr<PrimitiveTask>("Sub-task2-1");
    SharedPtr<PrimitiveTask> subTask11 = MakeSharedPtr<PrimitiveTask>("Sub-task1-1");
    SharedPtr<ICondition> ctrue = MakeSharedPtr<FuncCondition>("Done == true", [](IContext &ctx)
                                                               { return (static_cast<DomainTestContext &>(ctx).Done() == true); });

    subTask21->AddCondition(ctrue);
    task2->AddSubTask(subTask21);

    subTask11->AddCondition(ctrue);

    task->AddSubTask(subTask11);
    task->AddSubTask(task);

    rootTask->AddSubTask(task);

    ctx.LastMTR().Add(0);
    ctx.LastMTR().Add(1);
    ctx.LastMTR().Add(0);

    // We expect this test to be rejected, because [0,1,1] shouldn't beat [0,1,0]
    auto status = rootTask->Decompose(ctx, 0, plan);

    EXPECT_TRUE(status == DecompositionStatus::Rejected);
    EXPECT_TRUE(plan.size() == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord().size() == 3);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[0] == 0);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[1] == 1);
    EXPECT_TRUE(ctx.MethodTraversalRecord()[2] == -1);
}