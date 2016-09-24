#include "Enabled.h"

#include "MySTL/ioHelper.h";

void Enabled::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(m_IsEnabled);
#undef _KEY
}