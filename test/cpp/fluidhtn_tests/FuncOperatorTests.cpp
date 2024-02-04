#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Operators/Operator.h"
#include "DomainContextTester.h"

/**
 *  Test : Update Does Nothing Without Function PTR - Validated
 *
 *  Description :
 *
 */

TEST(FuncOperatorTests, Update_Does_Nothing_Without_Function_PTR_Validated)
{
    DomainTestContext ctx;
    auto e = MakeSharedPtr<FuncOperator>(nullptr, nullptr);
    e->Update(ctx);
}

/**
 *  Test : Stop Does Nothing without Function PTR - Validated
 *
 *  Description :
 *
 */

TEST(FuncOperatorTests, Stop_Does_Nothing_without_Function_PTR_Validated)
{
    DomainTestContext ctx;
    auto e = MakeSharedPtr<FuncOperator>(nullptr, nullptr);

    e->Stop(ctx);
}

/**
 *  Test : Update Returns Status Internal Function PTR - Validated
 *
 *  Description :
 *
 */

TEST(FuncOperatorTests, Update_Returns_Status_Internal_Function_PTR_Validated)
{
    DomainTestContext ctx;
    auto e = MakeSharedPtr<FuncOperator>([=](IContext &)
                                         { return TaskStatus::Success; },
                                         nullptr);

    auto status = e->Update(ctx);

    EXPECT_EQ((int)TaskStatus::Success, (int)status);
}

/**
 *  Test : Stop Calls Internal Function PTR - Validated
 *
 *  Description :
 *
 */

TEST(FuncOperatorTests, Stop_Calls_Internal_Function_PTR_Validated)
{
    DomainTestContext ctx;
    auto e = MakeSharedPtr<FuncOperator>(nullptr, [](IContext &ctx)
                                         { static_cast<DomainTestContext &>(ctx).Done() = true; });

    e->Stop(ctx);

    EXPECT_EQ(true, ctx.Done());
}
