#include "Variant.h"

#include <variant>
#include <string>

#include <iostream>

struct Foo
{
	int i;
};

struct Bar
{
	Bar() = default;
	Bar(short n) : d(n) {}
	short d;
};

struct Baz
{
	std::string name{"Jimmy"};
};


int main()
{
	basic_pt_variant::BasicPtVariant<Foo, Bar> var;

	if(basic_pt_variant::holds_alternative<Bar>(var))
	{
		std::cout << "Variant holds a Bar\n";
	}
	else if(basic_pt_variant::holds_alternative<Foo>(var))
	{
		std::cout << "Variant holds a Foo\n";
	}

	basic_pt_variant::BasicPtVariant<Foo, Bar> var2 = Bar(2);

	if(basic_pt_variant::holds_alternative<Bar>(var2))
	{
		std::cout << "Variant2 holds a Bar\n";
	}
	else if(basic_pt_variant::holds_alternative<Foo>(var2))
	{
		std::cout << "Variant2 holds a Foo\n";
	}

	return 0;
}
