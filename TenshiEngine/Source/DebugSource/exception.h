#pragma once

struct _EXCEPTION_POINTERS;
namespace Debug {

	int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
}