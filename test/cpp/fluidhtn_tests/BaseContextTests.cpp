#include <gtest/gtest.h>

#include "precompiled.h"
#include "Effects/EffectType.h"
#include "Contexts/BaseContext.h"
#include "DomainContextTester.h"

/**
 *  Test : Default State Executes - Validated
 *
 *  Description : The Context starts in the Executing State
 *
 */

TEST(BaseContextTests, Default_State_Executes_Validated)
{
    DomainTestContext ctx;
    EXPECT_TRUE(ctx.GetContextState() == ContextState::Executing);
}

/**
 *  Test : Context Init() Sets up Collections - Validated
 *
 *  Description : The Init() function correctly sets up collections
 *
 */

TEST(BaseContextTests, Init_Sets_Up_Colllections_Validated)
{
    DomainTestContext ctx;

    ctx.Init();

    EXPECT_EQ(false, ctx.DebugMTR());
    EXPECT_EQ(false, ctx.LogDecomposition());
    EXPECT_EQ(true, ctx.MTRDebug().size() == 0);
    EXPECT_EQ(true, ctx.LastMTRDebug().size() == 0);
    EXPECT_EQ(true, ctx.DecompositionLog().size() == 0);
}

/**
 *  Test : Context Init() Sets up Debug Collections - Validated
 *
 *  Description :
 *
 */

TEST(BaseContextTests, Init_Sets_Up_Debug_Colllections_Validated)
{
    MyDebugContext ctx;

    ctx.Init();

    EXPECT_TRUE(ctx.DebugMTR());
    EXPECT_TRUE(ctx.LogDecomposition());
}

/**
 *  Test : Context Has State - Validated
 *
 *  Description : Context has the added HasB state
 *
 */

TEST(BaseContextTests, Has_State_Validated)
{
    DomainTestContext ctx;
    ctx.Init();
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);

    EXPECT_EQ(false, ctx.HasStateOneParam(DomainTestState::HasA));
    EXPECT_EQ(true, ctx.HasStateOneParam(DomainTestState::HasB));
}

/**
 *  Test : Context set to Planning - Validated
 *
 *  Description : Context is set to planning state
 *
 */

TEST(BaseContextTests, Set_To_Planning_Validated)
{
    DomainTestContext ctx;

    ctx.Init();
    ctx.SetContextState(ContextState::Planning);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);

    EXPECT_TRUE((bool)ctx.GetStateDTS(DomainTestState::HasB));
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 0);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].top().First() == FluidEffectType::Permanent);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].top().Second() == 1);
    EXPECT_TRUE(ctx.GetWorldState().GetState(DomainTestState::HasB) == 0);
}

/**
 *  Test : Context Set State to Planning - Validated
 *
 *  Description : Context is set to executing state
 *
 */

TEST(BaseContextTests, Set_To_Executing_Validated)
{
    DomainTestContext ctx;

    ctx.Init();
    ctx.SetContextState(ContextState::Executing);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);

    EXPECT_TRUE(ctx.HasStateOneParam(DomainTestState::HasB));
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 0);
    EXPECT_TRUE(ctx.GetWorldState().GetState(DomainTestState::HasB) == 1);
}

/**
 *  Test : Get Context's State Planning Context - Validated
 *
 *  Description : Return the state of the domain during planning
 *
 */

TEST(BaseContextTests, Get_Planning_Context_Validated)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Planning);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);

    EXPECT_EQ(0, (int)ctx.GetStateDTS(DomainTestState::HasA));
    EXPECT_EQ(1, (int)ctx.GetStateDTS(DomainTestState::HasB));
}

/**
 *  Test : Get Context's State Executing Context - Validated
 *
 *  Description : Return the state of the domain during execution
 *
 */

TEST(BaseContextTests, Get_Execution_Context_Validated)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Executing);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);

    EXPECT_EQ(0, (int)ctx.GetStateDTS(DomainTestState::HasA));
    EXPECT_EQ(1, (int)ctx.GetStateDTS(DomainTestState::HasB));
}

/**
 *  Test : Get World State Change Depth - Validated
 *
 *  Description :
 *
 */

TEST(BaseContextTests, Get_World_State_Change_Depth_Validated)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Executing);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    auto changeDepthExecuting = ctx.GetWorldStateChangeDepth();

    ctx.SetContextState(ContextState::Planning);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    auto changeDepthPlanning = ctx.GetWorldStateChangeDepth();

    EXPECT_EQ(ctx.GetWorldStateChangeStack().size(), changeDepthExecuting.size());
    EXPECT_EQ(0, changeDepthExecuting[(int)DomainTestState::HasA]);
    EXPECT_EQ(0, changeDepthExecuting[(int)DomainTestState::HasB]);

    EXPECT_EQ(ctx.GetWorldStateChangeStack().size(), changeDepthPlanning.size());
    EXPECT_EQ(0, changeDepthPlanning[(int)DomainTestState::HasA]);
    EXPECT_EQ(1, changeDepthPlanning[(int)DomainTestState::HasB]);
}

/**
 *  Test : Trim for Execution - Validated
 *
 *  Description : Trim non-permanent effects, which are used only in planning
 *
 */

TEST(BaseContextTests, Trim_for_Execution_Validated)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Planning);
    ctx.SetStateDTS(DomainTestState::HasA, true, true, FluidEffectType::PlanAndExecute);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    ctx.SetStateDTS(DomainTestState::HasC, true, true, FluidEffectType::PlanOnly);
    ctx.TrimForExecution();

    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 0);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 0);
}

/**
 *  Test : Trim for Execution Invalid During Planning - Exception
 *
 *  Description : Trim for execution is used only to remove effects that are only valid for planning
 *                  and not valid during execution
 *
 */

TEST(BaseContextTests, Trim_for_Execution_Invalid_During_Planning)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Executing);
    EXPECT_THROW({
        ctx.TrimForExecution();
    },
                 std::exception);
}

/**
 *  Test : Trim to Stack Depth - Validated
 *
 *  Description :
 *
 */

TEST(BaseContextTests, Trim_to_Stack_Depth_Validated)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Planning);
    ctx.SetStateDTS(DomainTestState::HasA, true, true, FluidEffectType::PlanAndExecute);
    ctx.SetStateDTS(DomainTestState::HasB, true, true, FluidEffectType::Permanent);
    ctx.SetStateDTS(DomainTestState::HasC, true, true, FluidEffectType::PlanOnly);
    auto stackDepth = ctx.GetWorldStateChangeDepth();

    ctx.SetStateDTS(DomainTestState::HasA, false, true, FluidEffectType::PlanAndExecute);
    ctx.SetStateDTS(DomainTestState::HasB, false, true, FluidEffectType::Permanent);
    ctx.SetStateDTS(DomainTestState::HasC, false, true, FluidEffectType::PlanOnly);
    ctx.TrimToStackDepth(stackDepth);

    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasA].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasB].size() == 1);
    EXPECT_TRUE(ctx.GetWorldStateChangeStack()[(int)DomainTestState::HasC].size() == 1);
}

/**
 *  Test : Trim to Stack Depth Invalid in Wrong State - Exception
 *
 *  Description :
 *
 */

TEST(BaseContextTests, Trim_to_Stack_Depth_Invalid_in_Wrong_State)
{
    DomainTestContext ctx;
    ctx.Init();

    ctx.SetContextState(ContextState::Executing);
    auto stackDepth = ctx.GetWorldStateChangeDepth();
    EXPECT_THROW({
        ctx.TrimToStackDepth(stackDepth);
    },
                 std::exception);
}

/**
 *  Test :
 *
 *  Description :
 *
 */

// TEST(BaseContextTests, TBD)
// {
//     DomainTestContext ctx;
//     ctx.Init();
// }