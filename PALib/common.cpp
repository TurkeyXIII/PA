#include "pch.h"
#include "common.h"

void debugf(const char* format, ...)
{
#ifdef	_DEBUG
	va_list arglist;

	va_start(arglist, format);

	char formattedString[1024];

	vsprintf_s(formattedString, format, arglist);

	OutputDebugStringA(formattedString);

	va_end(arglist);
#endif // _DEBUG
}

void assert(bool test)
{
#ifdef _DEBUG
	if (!test)
	{
		// cause an exception
		int* pi = NULL;
		int i = *pi;
	}

#endif // _DEBUG
}