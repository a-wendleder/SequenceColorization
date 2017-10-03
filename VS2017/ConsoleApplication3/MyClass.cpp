#include "MyClass.h"


MyClass::MyClass(int i)
{
	int_ptr = (int*)malloc(sizeof(int));
	*int_ptr = i;
}

MyClass::~MyClass()
{
	free(int_ptr);
}
