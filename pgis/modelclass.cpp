#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

int* getVertIndices(std::string& str, int* r)
{
	std::string token = "";
	int i = 0;
	int t = 0;
	while (i < str.size())
	{
		if (str[i] == '/')
		{
			r[t++] = std::stoi(token);
			token = "";
			i++;
			continue;
		}
		token += str[i];
		i++;
	}
	r[t++] = std::stoi(token);
	return r;
}

bool ModelClass::LoadModel(const char* filename)
{
	std::ifstream fin;
	char input;
	int i;
	std::string line, t1, t2, t3, t4;
	std::vector<D3DXVECTOR3> vertices;
	std::vector<D3DXVECTOR3> normals;
	std::vector<D3DXVECTOR2> texcoords;
	std::vector<ModelType> allVertices;
	
	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	while (fin.eof() == false)
	{
		std::getline(fin, line);
		std::istringstream linestream(line);
		if (line.find("vt") == 0)
		{
			linestream >> t1 >> t2 >> t3 >> t4;
			texcoords.push_back(D3DXVECTOR2(std::stof(t2), std::stof(t3)));
		}
		else if (line.find("vn") == 0)
		{
			linestream >> t1 >> t2 >> t3 >> t4;
			normals.push_back(D3DXVECTOR3(std::stof(t2), std::stof(t3), std::stof(t4)));
		}
		else if (line.find("v") == 0)
		{
			linestream >> t1 >> t2 >> t3 >> t4;
			vertices.push_back(D3DXVECTOR3(std::stof(t2), std::stof(t3), std::stof(t4)));
		}
		else if (line.find("f") == 0)
		{
			linestream >> t1 >> t2 >> t3 >> t4;
			int* indices = new int[3];
			
			ModelType vert;
			indices = getVertIndices(t2, indices);
			vert.x = vertices[indices[0] - 1].x;
			vert.y = vertices[indices[0] - 1].y;
			vert.z = vertices[indices[0] - 1].z;
			vert.tu = texcoords[indices[1] - 1].x;
			vert.tv = texcoords[indices[1] - 1].y;
			vert.nx = normals[indices[2] - 1].x;
			vert.ny = normals[indices[2] - 1].y;
			vert.nz = normals[indices[2] - 1].z;
			allVertices.push_back(vert);

			ModelType vert1;
			indices = getVertIndices(t3, indices);
			vert1.x = vertices[indices[0] - 1].x;
			vert1.y = vertices[indices[0] - 1].y;
			vert1.z = vertices[indices[0] - 1].z;
			vert1.tu = texcoords[indices[1] - 1].x;
			vert1.tv = texcoords[indices[1] - 1].y;
			vert1.nx = normals[indices[2] - 1].x;
			vert1.ny = normals[indices[2] - 1].y;
			vert1.nz = normals[indices[2] - 1].z;
			allVertices.push_back(vert1);

			ModelType vert2;
			indices = getVertIndices(t4, indices);
			vert2.x = vertices[indices[0] - 1].x;
			vert2.y = vertices[indices[0] - 1].y;
			vert2.z = vertices[indices[0] - 1].z;
			vert2.tu = texcoords[indices[1] - 1].x;
			vert2.tv = texcoords[indices[1] - 1].y;
			vert2.nx = normals[indices[2] - 1].x;
			vert2.ny = normals[indices[2] - 1].y;
			vert2.nz = normals[indices[2] - 1].z;
			allVertices.push_back(vert2);

			delete indices;
			indices = 0;
		}
	}

	m_vertexCount = allVertices.size();
	m_indexCount = allVertices.size();
	m_model = new ModelType[allVertices.size()];
	std::copy(allVertices.begin(), allVertices.end(), m_model);

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}