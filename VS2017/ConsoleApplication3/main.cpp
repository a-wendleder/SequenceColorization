#include <iostream>
#include <string>
#include "MyClass.h"

int mainf()
{
	if (true) {
		MyClass myClassA(111);
		if (true) {
			MyClass myClassB = myClassA;
		}                    // myClassB's destructor gets called, freeing the memory at the int_ptr
	}                        // myClassA's destructor gets called, trying to free the memory again
}