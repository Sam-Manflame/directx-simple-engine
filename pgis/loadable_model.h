#pragma once

#include "modelclass.h"

class LoadableModel : public ModelClass
{
public:
	bool Initialize(ID3D11Device*, const char*, WCHAR*);
protected:
	bool LoadModel(const char*);
};