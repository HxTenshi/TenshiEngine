#pragma once

#include "Using.h"

ref class InspectorData{
public:
	virtual ~InspectorData(){}
	virtual void CreateInspector(DockPanel^ dockPanel) = 0;
};