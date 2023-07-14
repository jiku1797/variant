#ifndef VARIANT_TEST_VARIANTTEST_H
#define VARIANT_TEST_VARIANTTEST_H

#include "gtest/gtest.h"
#include "Variant.h"

struct DefaultConstructible
{
private:
	int i_;
};

struct OnlyConstructibleFromInt
{
	explicit OnlyConstructibleFromInt(int i)
		: i_(i) {}

private:
	int i_;
};

class VariantTest : public ::testing::Test
{
protected:
	void SetUp() override
	{

	}

	basic_variant::BasicVariant<int, double> int_or_double_;
	basic_variant::BasicVariant<DefaultConstructible, OnlyConstructibleFromInt> v1_;
};

#endif //VARIANT_TEST_VARIANTTEST_H
