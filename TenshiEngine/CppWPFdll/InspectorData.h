#pragma once

#include "Using.h"

ref class InspectorData{
public:
	InspectorData(){}
	virtual ~InspectorData(){}
	virtual void CreateInspector(DockPanel^ dockPanel) = 0;
};