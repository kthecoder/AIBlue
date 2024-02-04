#include <gtest/gtest.h>

#include "FluidHTN/BaseDomainBuilder.h"
// #include "DomainTestContext.h"

using namespace FluidHTN;

class DomainBuilder final : public BaseDomainBuilder
{
public:
    DomainBuilder(StringType n)
        : BaseDomainBuilder(n)
    {
    }
};

TEST(DomainBuilderTests, Build_ForgotEnd)
{
    DomainBuilder builder("Test"s);

    // Act
    auto ptr = builder.Pointer();
    auto domain = *(builder.Build());

    EXPECT_TRUE(domain.Root() != nullptr);
    EXPECT_TRUE(ptr == domain.Root());
    EXPECT_EQ("Test"s, domain.Root()->Name());
}