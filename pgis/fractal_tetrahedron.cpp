#include "fractal_tetrahedron.h"
#include <math.h>

FractalTetrahedron::FractalTetrahedron(float initialSize)
{
	prevTetr.a = D3DXVECTOR3(0, 0, -initialSize);
	prevTetr.b = D3DXVECTOR3(-sqrt(3) / 2 * initialSize, 0, initialSize / 2);
	prevTetr.c = D3DXVECTOR3(sqrt(3) / 2 * initialSize, 0, initialSize / 2);
	prevTetr.d = D3DXVECTOR3(0, sqrt(2) * initialSize, 0);

	first.push_back(D3DXVECTOR3(0, 0, 0));

	genVertices();
}

bool FractalTetrahedron::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	return ModelClass::Initialize(device, textureFilename);
}

void FractalTetrahedron::Frame(ID3D11Device* device)
{
	static int counter = 0;
	counter++;
	if (counter > 360)
	{
		counter = 0;
		nextStep(device);
	}
	ModelClass::Frame(device);
}

void FractalTetrahedron::nextStep(ID3D11Device* device)
{
	Tetr newTetr = prevTetr;
	newTetr.a /= 2;
	newTetr.b /= 2;
	newTetr.c /= 2;
	newTetr.d /= 2;

	while (!first.empty())
	{
		D3DXVECTOR3 offset, offsetA, offsetB, offsetC, offsetD;
		// popping offset from the first list
		offset = first.front();
		first.pop_front();

		// magic of breaking offset into four new ones goes here
		offsetA = offset + newTetr.a;
		offsetB = offset + newTetr.b;
		offsetC = offset + newTetr.c;
		offsetD = offset + newTetr.d;

		// pushing new four offset made from popped one to the second list
		second.push_back(offsetA);
		second.push_back(offsetB);
		second.push_back(offsetC);
		second.push_back(offsetD);
	}

	// swaping lists so all offsets are always in the first list to be used
	first.swap(second);

	step = step + 1;
	prevTetr = newTetr;

	ShutdownBuffers();
	genVertices();
	InitializeBuffers(device);
}

void genTriangle(ModelClass::ModelType* m_model, int& i, D3DXVECTOR3& p1, D3DXVECTOR3& p2, D3DXVECTOR3& p3, D3DXVECTOR3& offset)
{
	// normal to this triangle
	D3DXVECTOR3 u(p2 - p1);
	D3DXVECTOR3 v(p3 - p1);
	D3DXVECTOR3 normal(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y - v.x);

	// B
	m_model[i].x = p1.x + offset.x;
	m_model[i].y = p1.y + offset.y;
	m_model[i].z = p1.z + offset.z;

	m_model[i].tu = 0;
	m_model[i].tv = 1;

	m_model[i].nx = normal.x;
	m_model[i].ny = normal.y;
	m_model[i].nz = normal.z;
	i++;
	// D
	m_model[i].x = p2.x + offset.x;
	m_model[i].y = p2.y + offset.y;
	m_model[i].z = p2.z + offset.z;

	m_model[i].tu = 0.5f;
	m_model[i].tv = 0;

	m_model[i].nx = normal.x;
	m_model[i].ny = normal.y;
	m_model[i].nz = normal.z;
	i++;
	// A
	m_model[i].x = p3.x + offset.x;
	m_model[i].y = p3.y + offset.y;
	m_model[i].z = p3.z + offset.z;

	m_model[i].tu = 1;
	m_model[i].tv = 1;

	m_model[i].nx = normal.x;
	m_model[i].ny = normal.y;
	m_model[i].nz = normal.z;
	i++;
}

void FractalTetrahedron::genVertices()
{
	if (m_model)
	{
		delete[] m_model;
	}

	// each offset is corresponding to one tetrahendron wich consist of 4 triangles
	m_vertexCount = m_indexCount = first.size() * 4 * 3;
	m_model = new ModelType[m_vertexCount];
	
	int i = 0;
	for (D3DXVECTOR3 offset : first)
	{
		genTriangle(m_model, i, prevTetr.b, prevTetr.d, prevTetr.a, offset);
		genTriangle(m_model, i, prevTetr.a, prevTetr.d, prevTetr.c, offset);
		genTriangle(m_model, i, prevTetr.c, prevTetr.d, prevTetr.b, offset);
		genTriangle(m_model, i, prevTetr.a, prevTetr.c, prevTetr.b, offset);
	}

}
