#include "Canvas.h"

#include "Engine/Inspector.h"

float Canvas::m_Width = 1920.0f;
float Canvas::m_Height = 1080.0f;

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
}

#ifdef _ENGINE_MODE
void Canvas::CreateInspector()
{
	Inspector ins("Canvas", NULL);

	Vector2 vec2(&m_Width, &m_Height);
	ins.Add("Size", &vec2, [&](Vector2 v) {
		m_Width = v.x;
		m_Height = v.y;
	});

	ins.Complete();
}
#endif

void Canvas::_ExportData(I_ioHelper * io, bool childExport)
{
	(void)childExport;

#define _KEY_COMPEL(x) io->func( x , #x,true)

	_KEY_COMPEL(m_Width);
	_KEY_COMPEL(m_Height);

#undef _KEY_COMPEL
}

void Canvas::_ImportData(I_ioHelper * io)
{
#define _KEY_COMPEL(x) io->func( x , #x,true)

	_KEY_COMPEL(m_Width);
	_KEY_COMPEL(m_Height);

#undef _KEY_COMPEL
}
