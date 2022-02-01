#include "GLUTCallbacks.h"
#include "HelloGL.h"

namespace GLUTCallbacks
{
	namespace //anon. namespace cannot be accessed by anything not in parent namespace
	{
		//init to null pointer
		HelloGL* helloGL = nullptr;
	}

	void Init(HelloGL* gl)
	{
		helloGL = gl;
	}

	void Display()
	{
		if (helloGL != nullptr)
		{
			helloGL->Display();
		}
	}
}