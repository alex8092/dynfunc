#include "Function.hpp"
#include <iostream>
#include <exception>

int		test_func(int c) {
	std::cout << "test: " << c << std::endl;
	return c + 1;
}

class Test
{
public:
	explicit Test() {
		MemberFunction<Test, int, int> func_test(this, &Test::test_func2);
		Function<void>	*test_func_container = dynamic_cast<Function<void>*>(&func_test);
		test_func_container->call<int>(6);
	}

	int	test_func2(int c) {
		std::cout << "testing: " << c << std::endl;
		return 0;
	}
};

int	main(void) {
	Test t;
	Function<int, int>	test(&test_func);
	Function<void>	*test_func_container = dynamic_cast<Function<void>*>(&test);
	try {
		std::cout << test_func_container->call<int>(4, "test") << std::endl;
	} catch (const std::exception& e) {
		std::cout << "error: " << e.what() << std::endl;
	}
	std::cout << test_func_container->call<int>(10) << std::endl;
}
