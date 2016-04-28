#pragma once

struct ForceMode
{
	enum Enum
	{
		eFORCE,				//!< parameter has unit of mass * distance/ time^2, i.e. a force
		eIMPULSE,			//!< parameter has unit of mass * distance /time
		eVELOCITY_CHANGE,	//!< parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
		eACCELERATION		//!< parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
	};
};