#include "gtest/gtest.h"

#include "Variant.h"

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(test_variant, dummy)
{
	basic_pt_variant::BasicPtVariant<int, double> var;
	ASSERT_EQ(1, 1);
}