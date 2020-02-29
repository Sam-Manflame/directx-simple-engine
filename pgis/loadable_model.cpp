#include "loadable_model.h"

bool LoadableModel::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	return ModelClass::Initialize(device, textureFilename);
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

bool LoadableModel::LoadModel(const char* filename)
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
