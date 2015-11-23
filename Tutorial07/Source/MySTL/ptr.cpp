#include "ptr.h"

sp_count::sp_count(sp_count const& sc)
	: pimpl(sc.pimpl)
{
	if (pimpl)pimpl->addref();
}

sp_count& sp_count::operator = (sp_count const& sc)
{
	if (sc.pimpl != pimpl)
	{
		if (sc.pimpl)sc.pimpl->addref();
		if (pimpl)pimpl->release();
		pimpl = sc.pimpl;
	}
	return *this;
}

sp_count::sp_count(wp_count const& c)
	: pimpl(c.pimpl)
{
	if (pimpl && !pimpl->addref())
	{
		pimpl = NULL;
	}
}

sp_count::~sp_count()
{
	if (pimpl)pimpl->release();
}