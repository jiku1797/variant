#include "gtest/gtest.h"

#include "Variant.h"
#include "VariantTest.h"

using namespace basic_pt_variant;

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST_F(VariantTest, defaultCtor)
{
	ASSERT_TRUE(holds_alternative<int>(int_or_double_));
	ASSERT_EQ(int_or_double_.index(), 0);
}

TEST_F(VariantTest, valueCtorWithValidValue)
{
	BasicPtVariant<int, double> v = 3.14;
	ASSERT_TRUE(holds_alternative<double>(v));
	ASSERT_FALSE(holds_alternative<int>(v));
	ASSERT_EQ(v.index(), 1);
}

TEST_F(VariantTest, valueCtorWithSingleType)
{
	BasicPtVariant<OnlyConstructibleFromInt> v = OnlyConstructibleFromInt(42);
	ASSERT_TRUE(holds_alternative<OnlyConstructibleFromInt>(v));
	ASSERT_EQ(v.index(), 0);
}

TEST_F(VariantTest, copyCtor)
{
	using variant_t = BasicPtVariant<int, double>;

	// prepare a source variant
	variant_t src = 3.14;

	// construct another one from it
	variant_t dest = src;
	ASSERT_TRUE(holds_alternative<double>(dest));
	ASSERT_EQ(dest.index(), 1);
}

TEST_F(VariantTest, moveCtor)
{
	BasicPtVariant<int, double> dest = std::move(BasicPtVariant<int, double>(3.14));
	ASSERT_TRUE(holds_alternative<double>(dest));
	ASSERT_EQ(dest.index(), 1);
}
