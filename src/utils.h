#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/common.hpp>
#include "./opengl_stuff.h"

using namespace glm;
using namespace std;

class Utils {
public:
	static void loadObjFile(const string fileName, std::vector<GLfloat>& vertices,
		std::vector<GLfloat>& normals, std::vector<GLuint>& indices, std::vector<GLfloat>& textures) {
		ifstream file("../assets/" + fileName);
		if (!file.good()) {
			cerr << "Fichier '" << fileName << "' invalide" << endl;
			return;
		}

		string line;
		string delimiter = " ";
		vector<vec3> tempNormals;
		vector<vec2> tempTextures;
		bool initIndices = false;
		for (std::string line; getline(file, line);) {
			size_t pos = 0;
			std::string token;
			vector<string> tokens;
			while ((pos = line.find(delimiter)) != std::string::npos) {
				token = line.substr(0, pos);
				tokens.push_back(token);
				line.erase(0, pos + delimiter.length());
			}
			tokens.push_back(line);
			if (tokens.empty())
				continue;

			string firstToken = tokens.at(0);

			if (firstToken.compare("#") == 0) // Commentaire
				continue;

			if (firstToken.compare("vt") == 0) { // texture
				tempTextures.push_back(vec2(stof(tokens[1]), stof(tokens[2])));
			}
			if (firstToken.compare("v") == 0) { // vertices
				for (auto it = tokens.begin()+1; it != tokens.end(); it++) {
					vertices.push_back(stof(*it));
				}
			}
			if (firstToken.compare("vn") == 0) { // normals
				tempNormals.push_back(vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
			}

			if (firstToken.compare("f") == 0) { // indices
				if (!initIndices) {
					textures = std::vector<GLfloat>(vertices.size() * 2, 0.0f);
					normals = std::vector<GLfloat>(vertices.size() * 3, 0.0f);

					initIndices = true;
				}
				for (auto it = tokens.begin()+1; it != tokens.end(); it++) {
					string vertex = *it;
					int idx;
					int currentVertexPointer;
					if (vertex.find("/") != std::string::npos) {
						pos = vertex.find("/");
						token = vertex.substr(0, pos);
						int idx = stoi(token) - 1;
						int currentVertexPointer = idx;
						indices.push_back(currentVertexPointer);

						vertex.erase(0, pos + delimiter.length());
						pos = vertex.find("/");
						token = vertex.substr(0, pos);
						idx = stoi(token) - 1;
						vec2 currentTex = tempTextures.at(idx);
						textures[currentVertexPointer * 2] = 1 - currentTex.x;
						textures[currentVertexPointer * 2 + 1] = 1 - currentTex.y;

						vertex.erase(0, pos + delimiter.length());
						pos = vertex.find("/");
						token = vertex.substr(0, pos);
						idx = stoi(token) - 1;
						vec3 currentNorm = tempNormals.at(idx);
						normals[currentVertexPointer * 3] = currentNorm.x;
						normals[currentVertexPointer * 3 + 1] = currentNorm.y;
						normals[currentVertexPointer * 3 + 2] = currentNorm.z;
					} else
						indices.push_back(stoi(vertex) - 1);
				}
			}
		}
	}

	static mat4 createTransformationMatrix(vec3 translation, vec3 rotation, float scale) {
		mat4 matrix = mat4();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, radians(rotation.x), vec3(1, 0, 0));
		matrix = glm::rotate(matrix, radians(rotation.y), vec3(0, 1, 0));
		matrix = glm::rotate(matrix, radians(rotation.z), vec3(0, 0, 1));
		matrix = glm::scale(matrix, vec3(scale, scale, scale));

		return matrix;
	}
};