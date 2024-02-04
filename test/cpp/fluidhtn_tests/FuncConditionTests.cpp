#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Conditions/Condition.h"
#include "DomainContextTester.h"

/**
 *  Test : FuncCondition sets Name
 *
 *  Description :
 *
 */

TEST(FuncConditionTests, FuncCondition_sets_Name)
{
    auto c = MakeSharedPtr<FuncCondition>("Name", nullptr);

    EXPECT_EQ("Name", c->Name());
}

/**
 *  Test : Is Valid Fails without Function PTR
 *
 *  Description :
 *
 */

TEST(FuncConditionTests, Is_Valid_Fails_without_Function_PTR)
{
    auto ctx = MakeSharedPtr<DomainTestContext>();
    auto c = MakeSharedPtr<FuncCondition>("Name", nullptr);

    auto result = c->IsValid(*ctx);

    EXPECT_EQ(false, result);
}

/**
 *  Test : Is Valid Calls Internal Function PTR - Validated
 *
 *  Description : TBD
 *
 */

TEST(FuncConditionTests, Is_Valid_Calls_Internal_Function_PTR_Validated)
{
    DomainTestContext ctx;
    auto c = MakeSharedPtr<FuncCondition>("Name",
                                          [](IContext &ctx)
                                          { return (static_cast<DomainTestContext &>(ctx).Done() == false); });
    auto result = c->IsValid(ctx);

    EXPECT_EQ(true, result);
}