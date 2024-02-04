#include <gtest/gtest.h>

#include "precompiled.h"
#include "Contexts/BaseContext.h"
#include "Effects/Effect.h"

#include "DomainContextTester.h"

class TestContext : public BaseContext<DomainTestState, uint8_t, DomainTestWorldState>
{
    bool _Done = false;

public:
    bool &Done() { return _Done; }
};

/**
 *  Test : Action Effect Sets Name Validated
 *
 *  Description : Correctly Names the Action Effect
 *
 */

TEST(ActionEffectsTest, Action_Effect_Sets_Name_Validated)
{
    ActionEffect a("Name", FluidEffectType::PlanOnly, nullptr);

    EXPECT_EQ("Name", a.Name());
}

/**
 *  Test : Action Effect Sets Type Validated
 *
 *  Description : The PlanOnly Effect was correctly applied to the created Action Effect
 *
 */

TEST(ActionEffectsTest, Action_Effect_Sets_Type_Validated)
{
    ActionEffect e("Name", FluidEffectType::PlanOnly, nullptr);

    EXPECT_EQ(FluidEffectType::PlanOnly, e.Type());
}

/**
 *  Test : Action Effect Does Nothing Without Function Validated
 *
 *  Description : The Action Effect has no purpose without being attached to something
 *
 */

TEST(ActionEffectsTest, Action_Effect_Does_Nothing_Without_Function_Validated)
{
    TestContext ctx;
    ActionEffect e("Name", FluidEffectType::PlanOnly, nullptr);

    e.Apply(ctx);
}

/**
 *  Test : Action Effect Calls it's Function Validated
 *
 *  Description : The Action Effect correctly calls its function
 *
 */

TEST(ActionEffectsTest, Action_Effect_Calls_Its_Function_Validated)
{
    TestContext ctx;
    ActionEffect e("Name", FluidEffectType::PlanOnly, [=](IContext &c, FluidEffectType)
                   { static_cast<TestContext &>(c).Done() = true; });

    e.Apply(ctx);

    EXPECT_EQ(true, ctx.Done());
}