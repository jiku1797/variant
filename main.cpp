#include "Variant.h"

#include <string>
#include <utility>
#include <iostream>

template<typename ...Ts>
void printSizes(const basic_pt_variant::BasicPtVariant<Ts...>& var)
{
	  auto idx = 0;
	  (..., (std::cout << "Size at " << idx++ << " = " << sizeof(Ts) << "\n"));
	  std::cout << "\nSize of variant = " << sizeof(var) << '\n';
};

struct Foo
{
	int i;
	int j;
	char c;
};

int main()
{
	basic_pt_variant::BasicPtVariant<int, double, char, float, Foo> var;
	printSizes(var);

	return 0;
}
