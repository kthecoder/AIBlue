#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"

#include "Effects/Effect.h"
#include "Conditions/Condition.h"
#include "Operators/Operator.h"
#include "Tasks/PrimitiveTasks/PrimitiveTask.h"
#include "DomainContextTester.h"

/**
 *  Test : Add Condition - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Add_Condition_Validated)
{
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Name", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    bool bRet = task->AddCondition(c);

    EXPECT_TRUE(bRet);
    EXPECT_TRUE(task->Conditions().size() == 1);
}

/**
 *  Test : Add Executing Condition - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Add_Executing_Condition_Validated)
{
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Name", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });

    bool bRet = task->AddExecutingCondition(c);
    EXPECT_TRUE(bRet);
    EXPECT_TRUE(task->ExecutingConditions().size() == 1);
}

/**
 *  Test : Add Effect - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Add_Effect_Validated)
{
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<IEffect> e = MakeSharedPtr<ActionEffect>("Name", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType eff)
                                                       {
                (void)eff;
                static_cast<DomainTestContext&>(ctx).Done() = true; });

    bool bRet = task->AddEffect(e);
    EXPECT_TRUE(bRet);
    EXPECT_TRUE(task->Effects().size() == 1);
}

/**
 *  Test : Set Operator - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Set_Operator_Validated)
{
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<IOperator> o = MakeSharedPtr<FuncOperator>(nullptr, nullptr);

    task->SetOperator(o);

    EXPECT_TRUE(task->Operator() != nullptr);
}

/**
 *  Test : Set Operator Throws Exception If Already Set - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Set_Operator_Throws_Exception_If_Already_Set_Validated)
{
    auto task = MakeSharedPtr<PrimitiveTask>("Test");

    SharedPtr<IOperator> o = MakeSharedPtr<FuncOperator>(nullptr, nullptr);

    task->SetOperator(o);

    EXPECT_THROW({
        SharedPtr<IOperator> o2 = MakeSharedPtr<FuncOperator>(nullptr, nullptr);
        task->SetOperator(o2);
    },
                 std::exception);
}

/**
 *  Test : Apply Effects - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Apply_Effects_Validated)
{
    DomainTestContext ctx;
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<IEffect> e = MakeSharedPtr<ActionEffect>("Name", FluidEffectType::Permanent, [](IContext &ctx, FluidEffectType e)
                                                       {
                (void)e;
                static_cast<DomainTestContext&>(ctx).Done() = true; });

    task->AddEffect(e);
    task->ApplyEffects(ctx);

    EXPECT_EQ(true, ctx.Done());
}

/**
 *  Test : Stop With Valid Operator - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Stop_With_Valid_Operator_Validated)
{
    DomainTestContext ctx;
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<IOperator> o =
        MakeSharedPtr<FuncOperator>(nullptr, [](IContext &ctx)
                                    { static_cast<DomainTestContext &>(ctx).Done() = true; });

    task->SetOperator(o);
    task->Stop(ctx);

    EXPECT_TRUE(task->Operator() != nullptr);
    EXPECT_EQ(true, ctx.Done());
}

/**
 *  Test : Stop With Null Operator - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Stop_With_Null_Operator_Validated)
{
    DomainTestContext ctx;
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    task->Stop(ctx);
}

/**
 *  Test : Is Valid - Validated
 *
 *  Description :
 *
 */

TEST(PrimitiveTaskTests, Is_Valid_Validated)
{
    DomainTestContext ctx;
    auto task = MakeSharedPtr<PrimitiveTask>("Test");
    SharedPtr<ICondition> c = MakeSharedPtr<FuncCondition>("Name", [](IContext &ctx)
                                                           { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    SharedPtr<ICondition> c2 = MakeSharedPtr<FuncCondition>("Name", [](IContext &ctx)
                                                            { return (static_cast<DomainTestContext &>(ctx).Done() == true); });

    task->AddCondition(c);
    bool expectTrue = task->IsValid(ctx);

    task->AddCondition(c2);
    bool expectFalse = task->IsValid(ctx);

    EXPECT_TRUE(expectTrue);
    EXPECT_FALSE(expectFalse);
}