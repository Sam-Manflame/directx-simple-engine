#pragma once

#include "modelclass.h"
#include <list>

class FractalTetrahedron : public ModelClass
{
public:
	struct Tetr
	{
		D3DXVECTOR3 a, b, c, d;
	};

	FractalTetrahedron(float);

	bool Initialize(ID3D11Device*, WCHAR*);
	void Frame(ID3D11Device*);

private:
	std::list<D3DXVECTOR3> first;
	std::list<D3DXVECTOR3> second;

	Tetr prevTetr;
	int step = 0;

	void nextStep(ID3D11Device*);
	void genVertices();
};



