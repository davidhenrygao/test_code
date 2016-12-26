#include <iostream>

class Base
{
public:
	Base (int x) : m_x(x) {
		std::cout << "Base::Constructor" << std::endl;
	}
	Base (Base &bObj) {
		std::cout << "Base::Copy Constructor" << std::endl;
		m_x = bObj.m_x;
	}
	virtual void print() {
		std::cout << "Base::m_x: " << m_x << std::endl;
	}
	int getX() const {
		return m_x;
	}

private:
	int m_x;
};

class Derive : public Base
{
public:
	Derive (int x, int y) : Base(x), m_y(y) {
		std::cout << "Derive::Constructor" << std::endl;
	}
	Derive (Derive &dObj) : Base(dObj) {
		std::cout << "Derive::Copy Constructor" << std::endl;
		m_y = dObj.m_y;

	}
	virtual void print() {
		std::cout << "Derive::m_x: " << getX() << std::endl;
		std::cout << "Derive::m_y: " << m_y << std::endl;
	}
	int getY() const {
		return m_y;
	}

private:
	int m_y;
};

//test content: Copy constructions and Assignment operations from derived 
//              class object to base class object, or from derived class 
//              object to base class object reference.
int main(int argc, char *argv[])
{
	Base bObj(1);
	Derive dObj(2,3);

	//Derive &dObjRef = dObj;
	//bObj = dObjRef;
	bObj = dObj;  // assign derived class object to base class object.
	bObj.print();

	Derive dObj2(4,5);
	Base & bObjRef = dObj2;
	bObjRef = dObj;  // assign to base class object reference.
	dObj2.print();

	Base bObj2 = dObj2;  // copy construction from derived class object.
	bObj2.print();

	Derive dObj3 = dObj;
	dObj3.print();
	return 0;
}
