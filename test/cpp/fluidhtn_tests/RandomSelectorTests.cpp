#include <gtest/gtest.h>

#include "precompiled.h"
#include "Tasks/CompoundTasks/RandomSelector.h"
#include "BaseDomainBuilder.h"
#include "DomainContextTester.h"

/**
 *  Test : Random Select - Validated
 *
 *  Description : The Context starts in the Executing State
 *
 */

TEST(RandomSelectorTests, Random_Select_ExpectedBehavior)
{
    BaseDomainBuilder builder("tests");
    DomainTestContext ctx;
    ctx.Init();

    builder.AddRandomSelector("random");

    builder.AddAction("get a");
    builder.AddCondition("has not A", [](IContext &ctx)
                         { return (static_cast<DomainTestContext &>(ctx).HasStateOneParam(DomainTestState::HasA) == false); });
    builder.AddOperator([](IContext &)
                        { return TaskStatus::Success; });
    builder.End();
    builder.AddAction("get b");
    builder.AddCondition("has not B", [](IContext &ctx)
                         { return (static_cast<DomainTestContext &>(ctx).HasStateOneParam(DomainTestState::HasB) == false); });
    builder.AddOperator([](IContext &)
                        { return TaskStatus::Success; });
    builder.End();
    builder.AddAction("get c");
    builder.AddCondition("has not C", [](IContext &ctx)
                         { return (static_cast<DomainTestContext &>(ctx).HasStateOneParam(DomainTestState::HasC) == false); });
    builder.AddOperator([](IContext &)
                        { return TaskStatus::Success; });
    builder.End();
    builder.End();
    auto domain = builder.Build();

    int aCount = 0;
    int bCount = 0;
    int cCount = 0;
    for (int i = 0; i < 1000; i++)
    {
        TaskQueueType plan;
        auto status = domain->FindPlan(ctx, plan);
        EXPECT_TRUE(status == DecompositionStatus::Succeeded);
        EXPECT_TRUE(plan.size() == 1);

        auto name = plan.front()->Name();
        if (name == "get a"s)
            aCount++;
        if (name == "get b"s)
            bCount++;
        if (name == "get c"s)
            cCount++;

        EXPECT_TRUE(name == "get a" || name == "get b" || name == "get c");
        plan = TaskQueueType();
    }

    // With 1000 iterations, the chance of any of these counts being 0 is suuuper slim.
    EXPECT_TRUE(aCount > 0 && bCount > 0 && cCount > 0);
}