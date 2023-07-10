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

TEST_F(VariantTest, copyAssign)
{
	using variant_t = BasicPtVariant<int, double>;

	variant_t assignTo = 3.14;
	variant_t assignFrom = 42;

	// assign
	assignTo = assignFrom;

	ASSERT_TRUE(holds_alternative<int>(assignTo));
	ASSERT_EQ(assignTo.get<int>(), 42);
}

TEST_F(VariantTest, moveAssign)
{
	using variant_t = BasicPtVariant<int, double>;

	variant_t assignTo = 3.14;
	variant_t assignFrom = 42;

	// assign
	assignTo = std::move(assignFrom);

	ASSERT_TRUE(holds_alternative<int>(assignTo));
	ASSERT_EQ(assignTo.get<int>(), 42);
}

TEST_F(VariantTest, moveAssignByValue)
{
	using variant_t = BasicPtVariant<int, double>;

	variant_t assignTo = 3.14;

	const int val = 42;

	// assign
	assignTo = std::move(val);

	ASSERT_TRUE(holds_alternative<int>(assignTo));
	ASSERT_EQ(assignTo.get<int>(), 42);
}

TEST_F(VariantTest, getFromConstVariant)
{
	using variant_t = BasicPtVariant<int, double>;

	const variant_t v = 3.14;

	ASSERT_EQ(v.get<double>(), 3.14);
}

TEST_F(VariantTest, getWrongType)
{
	using variant_t = BasicPtVariant<int, double>;

	const variant_t v = 3.14;

	try
	{
		const auto val = v.get<int>();
		FAIL() << "Did not expect variant to hold an int";
	}
	catch(const detail::bad_variant_access& bva)
	{
		ASSERT_EQ(v.get<double>(), 3.14);
	}
}
