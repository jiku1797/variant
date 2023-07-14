#include "gtest/gtest.h"

#include "Variant.h"
#include "VariantTest.h"

using namespace basic_variant;

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
	BasicVariant<int, double> v = 3.14;
	ASSERT_TRUE(holds_alternative<double>(v));
	ASSERT_FALSE(holds_alternative<int>(v));
	ASSERT_EQ(v.index(), 1);
}

TEST_F(VariantTest, valueCtorWithSingleType)
{
	BasicVariant<OnlyConstructibleFromInt> v = OnlyConstructibleFromInt(42);
	ASSERT_TRUE(holds_alternative<OnlyConstructibleFromInt>(v));
	ASSERT_EQ(v.index(), 0);
}

TEST_F(VariantTest, copyCtor)
{
	using variant_t = BasicVariant<int, double>;

	// prepare a source variant
	variant_t src = 3.14;

	// construct another one from it
	const variant_t dest = src;
	ASSERT_TRUE(holds_alternative<double>(dest));
	ASSERT_EQ(dest.index(), 1);
}

TEST_F(VariantTest, moveCtor)
{
	BasicVariant<int, double> dest = std::move(BasicVariant<int, double>(3.14));
	ASSERT_TRUE(holds_alternative<double>(dest));
	ASSERT_EQ(dest.index(), 1);
}

TEST_F(VariantTest, copyAssign)
{
	using variant_t = BasicVariant<int, double>;

	variant_t assignTo = 3.14;
	variant_t assignFrom = 42;

	// assign
	assignTo = assignFrom;

	ASSERT_TRUE(holds_alternative<int>(assignTo));
	ASSERT_EQ(assignTo.get<int>(), 42);
}

TEST_F(VariantTest, moveAssign)
{
	using variant_t = BasicVariant<int, double>;

	variant_t assignTo = 3.14;
	variant_t assignFrom = 42;

	// assign
	assignTo = std::move(assignFrom);

	ASSERT_TRUE(holds_alternative<int>(assignTo));
	ASSERT_EQ(assignTo.get<int>(), 42);
}

TEST_F(VariantTest, moveAssignByValue)
{
	using variant_t = BasicVariant<int, double, std::string>;

	variant_t assignTo = 42;

	std::string testStr = "testing testing";

	// assign
	assignTo = std::move(testStr);

	ASSERT_TRUE(holds_alternative<std::string>(assignTo));
	ASSERT_EQ(assignTo.get<std::string>(), std::string("testing testing"));
}

TEST_F(VariantTest, getFromConstVariant)
{
	using variant_t = BasicVariant<int, double>;

	const variant_t v = 3.14;

	ASSERT_EQ(v.get<double>(), 3.14);
}

TEST_F(VariantTest, getWrongType)
{
	using variant_t = BasicVariant<int, double>;

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
