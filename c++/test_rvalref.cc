#include <iostream>

void foo(int const &x)
{
	std::cout << "lvalue overloads: x= " << x << std::endl;
}

void foo(int &&x)
{
	std::cout << "rvalue overloads: x= " << x << std::endl;
}

int getX();
int& getXRef();

class Test
{
public:
	Test (int x) : m_x(x) {}
	Test (Test &t) {
                std::cout << "copy constructor." << std::endl;
		m_x = t.m_x;
	}
	Test (Test &&t) {
                std::cout << "move constructor." << std::endl;
		m_x = t.m_x;
	}
	void print() {
		std::cout << "object member value: " << m_x << std::endl;
	}

private:
	int m_x;
};

//Test content: base usage of rvalue reference.
int main(int argc, char *argv[])
{
	int a = 1;
	foo(a);
	foo(2);
	foo(getX());
	foo(getXRef());

	Test obj1(100);
	Test obj2 = obj1;
	Test obj3 = std::move(obj2);
	obj1.print();
	obj2.print();
	obj3.print();
	return 0;
}

int getX()
{
	static int i = 5;
	return i++;
}
int& getXRef()
{
	static int j = 10;
	j++;
	return j;
}
