#include <gtest/gtest.h>

#include "precompiled.h"
#include "BaseDomainBuilder.h"

using namespace FluidHTN;

class DomainBuilder final : public BaseDomainBuilder
{
public:
    DomainBuilder(StringType n)
        : BaseDomainBuilder(n)
    {
    }
};

/**
 *  Test : Builder - Missing End
 *
 *  Description : The Domain needs to know when you have hit the end if its the end of the HTN or end of the Task Model
 *
 */

TEST(DomainBuilderTests, Builder_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    // Act
    auto ptr = builder.Pointer();
    auto domain = *(builder.Build());

    EXPECT_TRUE(domain.Root() != nullptr);
    EXPECT_TRUE(ptr == domain.Root());
    EXPECT_EQ("My Domain Tester", domain.Root()->Name());
}

/**
 *  Test : Builder Invalidates Pointer - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Builder_Invalidates_Pointer_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    auto ptr = builder.Pointer();
    auto domain = *(builder.Build());

    EXPECT_THROW({
        bool bRet = (builder.Pointer() == domain.Root());
        bRet;
    },
                 std::exception);
}

/**
 *  Test : Selector - Validated
 *
 *  Description : The Domain correctly adds a selector
 *
 */

TEST(DomainBuilderTests, Selector_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSelector("My Selector");
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Selector - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Selector_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSelector("My Selector");

    EXPECT_FALSE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SelectorCompoundTask));
}

/**
 *  Test : Selector - Missing End - Exception
 *
 *  Description : The Domain failed due to not including an End() since it's the end of the HTN Definition
 *
 */

TEST(DomainBuilderTests, Selector_Builder_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSelector("My Selector");

    EXPECT_THROW({
        auto domain = builder.Build();
    },
                 std::exception);
}

/**
 *  Test : Selector Compound Task - Validated
 *
 *  Description : Selector should be a compound task
 *
 */

TEST(DomainBuilderTests, Selector_Compound_Task_Validated)
{
    DomainBuilder builder("My Domain Tester");

    SharedPtr<CompoundTask> ctask = MakeSharedPtr<Selector>("compound task");
    builder.AddCompoundTask("compound task", ctask);

    EXPECT_FALSE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SelectorCompoundTask));
}

/**
 *  Test : Sequence - Validated
 *
 *  Description : The domain should correctly add a sequence task
 *
 */

TEST(DomainBuilderTests, Sequence_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSequence("My Sequence");
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Sequence - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Sequence_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSequence("Sequence test");

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SequenceCompoundTask));
}

/**
 *  Test : Sequence Compound Task - Validated
 *
 *  Description : The Sequence with a Compound Task inside, should be valid
 *
 */

TEST(DomainBuilderTests, Sequence_Compound_Task_Validated)
{
    DomainBuilder builder("My Domain Tester");

    SharedPtr<CompoundTask> ctask = MakeSharedPtr<Sequence>("My Sequence");
    builder.AddCompoundTask("My Compound Task", ctask);

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SequenceCompoundTask));
}

/**
 *  Test : Add Action to Domain - Validated
 *
 *  Description : The added action should add to the domain correctly
 *
 */

TEST(DomainBuilderTests, Add_Action_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Sequence");
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Add Action - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Add_Action_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Sequence");

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::PrimitiveTask));
}

/**
 *  Test : Adding Primitive Task - Validated
 *
 *  Description :
 *
 */

TEST(DomainBuilderTests, Add_Primitive_Task_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddPrimitiveTask("sequence test");

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::PrimitiveTask));
}

/**
 *  Test : Pause Plan Not Decomposed
 *
 *  Description : Pause Plan throws when pointer is not decomposeALl
 *
 */

TEST(DomainBuilderTests, Pause_Plan_Not_Decomposed)
{
    DomainBuilder builder("My Domain Tester");

    EXPECT_THROW({
        builder.PausePlan();
    },
                 std::exception);
}

/**
 *  Test : Pause Plan - Validated
 *
 *  Description : The Pause Plan correctly setup and functions
 *
 */

TEST(DomainBuilderTests, Pause_Plan_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSequence("My Sequence");
    builder.PausePlan();
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Pause Plan - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Pause_Plan_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSequence("sequence test");
    builder.PausePlan();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SequenceCompoundTask));
}

/**
 *  Test : Add Condition - Validated
 *
 *  Description : The Condition is correctly added to the domain
 *
 */

TEST(DomainBuilderTests, Add_Condition_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddCondition("Test Condition", [](IContext &)
                         { return true; });

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Executing Condition - Missing Primitive Task
 *
 *  Description : Executing condition throws due to it not being a child of a primitive task
 *
 */

TEST(DomainBuilderTests, Executing_Condition_Missing_Primitive_Task_Parent)
{
    DomainBuilder builder("My Domain Tester");

    EXPECT_THROW({
        builder.AddExecutingCondition("test", [](IContext &)
                                      { return true; });
    },
                 std::exception);
}

/**
 *  Test : Executing Condition - Validated
 *
 *  Description : Correctly executes condition in domain
 *
 */

TEST(DomainBuilderTests, Executing_Condition_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddExecutingCondition("My Condition Execution", [](IContext &)
                                  { return true; });
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Executing Condition - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Executing_Condition_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddExecutingCondition("My Condition Execution", [](IContext &)
                                  { return true; });

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::PrimitiveTask));
}

/**
 *  Test : Operator's Parent not Primitive
 *
 *  Description : Adding Operator must be child of Primitive Task
 *
 */

TEST(DomainBuilderTests, Operators_Parent_NOT_Primitive)
{
    DomainBuilder builder("My Domain Tester");

    EXPECT_THROW({
        builder.AddOperator([](IContext &) -> TaskStatus
                            { return TaskStatus::Success; });
    },
                 std::exception);
}

/**
 *  Test : Add Operator - Validated
 *
 *  Description : Operator correctly adds to the domain
 *
 */

TEST(DomainBuilderTests, Add_Operator_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddOperator([](IContext &) -> TaskStatus
                        { return TaskStatus::Success; });
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Operator - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Operator_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddOperator([](IContext &) -> TaskStatus
                        { return TaskStatus::Success; });

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::PrimitiveTask));
}

/**
 *  Test : Effect's Parent not Primitive
 *
 *  Description : Adding effect must be child of Primitive Task
 *
 */

TEST(DomainBuilderTests, Effects_Parent_NOT_Primitive)
{
    DomainBuilder builder("My Domain Tester");

    EXPECT_THROW({
        builder.AddEffect("My Effect", FluidEffectType::Permanent, [](IContext &, FluidEffectType) {});
    },
                 std::exception);
}

/**
 *  Test : Add Effect - Validated
 *
 *  Description : Adding Effect to domain is correct and valid
 *
 */

TEST(DomainBuilderTests, Add_Effect_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddEffect("My Effect", FluidEffectType::Permanent, [](IContext &, FluidEffectType) {});
    builder.End();

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Effect - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Effect_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");
    builder.AddEffect("My Effect", FluidEffectType::Permanent, [](IContext &, FluidEffectType) {});

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::PrimitiveTask));
}

/**
 *  Test : Splice Missing Compound Parent - Exception
 *
 *  Description : Requires Splice to be a child definition of a compound task
 *
 */

TEST(DomainBuilderTests, Splice_Missing_Compound_Parent)
{
    DomainBuilder builder("My Domain Tester");

    auto domain = *DomainBuilder("My Sub-Domain").Build();
    builder.AddAction("My Action");

    EXPECT_THROW({
        builder.Splice(domain);
    },
                 std::exception);
}

/**
 *  Test : Add Splice - Validated
 *
 *  Description : Correctly add splice to the domain defintion
 *
 */

TEST(DomainBuilderTests, Add_Splice_Validated)
{
    DomainBuilder builder("My Domain Tester");

    auto domain = *DomainBuilder("My Sub-Domain").Build();
    builder.Splice(domain);

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
}

/**
 *  Test : Splice - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Splice_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    auto domain = *DomainBuilder("My Sub-Domain").Build();
    builder.AddSelector("My Selector");
    builder.Splice(domain);

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SelectorCompoundTask));
}

/**
 *  Test : Slot Missing Compound Parent
 *
 *  Description : Requires Slot to be a child definition of a compound task
 *
 */

TEST(DomainBuilderTests, Slot_Missing_Compound_Parent)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddAction("My Action");

    EXPECT_THROW({
        builder.AddSlot(1);
    },
                 std::exception);
}

/**
 *  Test : Slot ID is Already Defined - Exception
 *
 *  Description : The Slot ID is already used in the domain prior
 *
 */

TEST(DomainBuilderTests, Slot_ID_Already_Defined)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSlot(1);

    EXPECT_THROW({
        builder.AddSlot(1);
    },
                 std::exception);
}

/**
 *  Test : Add Slot - Validated
 *
 *  Description : Add Slot to Domain is validated
 *
 */

TEST(DomainBuilderTests, Add_SLot_Validated)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSlot(1);
    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::TaskRoot));

    auto domain = *builder.Build();

    auto subDomain = *DomainBuilder("My Sub-Domain").Build();
    EXPECT_TRUE(domain.TrySetSlotDomain(1, subDomain)); // Its valid to add a sub-domain to a slot we have defined in our domain
                                                        // definition, and that is not currently occupied.
    EXPECT_TRUE(domain.TrySetSlotDomain(1, subDomain) ==
                false); // Need to clear slot before we can attach sub-domain to a currently occupied slot.
    EXPECT_TRUE(domain.TrySetSlotDomain(99, subDomain) ==
                false); // Need to define slotId in domain definition before we can attach sub-domain to that slot.

    EXPECT_TRUE(domain.Root()->Subtasks().size() == 1);
    EXPECT_TRUE(domain.Root()->Subtasks()[0]->IsTypeOf(ITaskDerivedClassName::Slot));

    auto slot = StaticCastPtr<Slot>(domain.Root()->Subtasks()[0]);
    EXPECT_TRUE(slot->Subtask() != nullptr);
    EXPECT_TRUE(slot->Subtask()->IsTypeOf(ITaskDerivedClassName::TaskRoot));
    EXPECT_TRUE(slot->Subtask()->Name() == "My Sub-Domain");

    domain.ClearSlot(1);
    EXPECT_TRUE(slot->Subtask() == nullptr);
}

/**
 *  Test : Slot - Missing End
 *
 *  Description : Requires an End() to tell the HTN we are at the end of this task or end of HTN Definition
 *
 */

TEST(DomainBuilderTests, Slot_Missing_End)
{
    DomainBuilder builder("My Domain Tester");

    builder.AddSelector("test");
    builder.AddSlot(1);

    EXPECT_TRUE(builder.Pointer()->IsTypeOf(ITaskDerivedClassName::SelectorCompoundTask));
}

/**
 *  Test :
 *
 *  Description :
 *
 */

// TEST(DomainBuilderTests, TBD)
// {
//     DomainBuilder builder("My Domain Tester");
// }