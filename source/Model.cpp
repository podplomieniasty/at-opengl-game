#include <Model.h>

Model::Model(const char* file) {

	std::cout << "MODEL: wczytywanie modelu: " << file << "\n";

	std::string text = getFileContents(file);
	m_json = json::parse(text);

	Model::file = file;
	data = getData();

	traverseNode(0);
}

void Model::loadMesh(unsigned int indMesh) {

	std::cout << "MODEL: Ladowanie mesh #" << indMesh << '\n';

	// pobierz atrybuty akcesorami jsona
	unsigned int posAccInd = m_json["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normAccInd = m_json["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = m_json["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = m_json["meshes"][indMesh]["primitives"][0]["indices"];

	std::vector<float> positionVec = getFloats(m_json["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(positionVec);

	std::vector<float> normalVec = getFloats(m_json["accessors"][normAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);

	std::vector<float> textureVec = getFloats(m_json["accessors"][texAccInd]);
	std::vector<glm::vec2> texs = groupFloatsVec2(textureVec);

	std::vector<Vertex> vertices = assembleVertices(positions, normals, texs);
	std::vector<GLuint> indices = getIndices(m_json["accessors"][indAccInd]);
	std::vector<Texture> textures = getTextures();

	meshes.push_back(Mesh(vertices, indices, textures));

}

void Model::draw(Shader& shader, Camera& camera) {

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Mesh::draw(shader, camera, matricesMeshes[i]);
	}
}

std::vector<unsigned char> Model::getData() {

	std::string textInBytes;
	std::string uri = m_json["buffers"][0]["uri"];

	std::cout << "MODEL: uri [" << Model::file << "]: [" << uri << "]\n";

	std::string fileString = std::string(file);
	std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);
	textInBytes = getFileContents((fileDirectory + uri).c_str());

	std::vector<unsigned char> data(textInBytes.begin(), textInBytes.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor) {

	std::vector<float> floats;

	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = m_json["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("MODEL: Niepoprawny typ accessora: " + type);


	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	unsigned int length = count * 4 * numPerVert;

	for (unsigned int i = beginningOfData; i < beginningOfData + length; i) {

		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float val;
		std::memcpy(&val, bytes, sizeof(float));
		floats.push_back(val);
	}

	return floats;
}

std::vector<GLuint> Model::getIndices(json accessor) {

	std::vector<GLuint> indices;

	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = m_json["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int beginningOfData = byteOffset + accessorByteOffset;

	if (componentType == 5125) {

		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 4; i) {

			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int val;
			std::memcpy(&val, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)val);
		}
	}
	else if (componentType == 5123) {

		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i) {

			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short val;
			std::memcpy(&val, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)val);
		}
	}
	else if (componentType == 5122) {

		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i) {

			unsigned char bytes[] = { data[i++], data[i++] };
			short val;
			std::memcpy(&val, bytes, sizeof(short));
			indices.push_back((GLuint)val);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures() {

	std::vector<Texture> textures;

	std::string fileString = std::string(file);
	std::string fileDirectory = fileString.substr(0, fileString.find_last_of('/') + 1);

	for (unsigned int i = 0; i < Model::m_json["images"].size(); i++) {

		std::string texPath = Model::m_json["images"][i]["uri"];

		bool skip = false;
		for (unsigned int j = 0; j < loadedTextureName.size(); j++) {

			if (loadedTextureName[j] == texPath) {

				textures.push_back(loadedTexture[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {

			if (texPath.find("baseColor") != std::string::npos) {

				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTexture.size());
				textures.push_back(diffuse);
				loadedTexture.push_back(diffuse);
				loadedTextureName.push_back(texPath);
			}
			else if (texPath.find("metallicRoughness") != std::string::npos) {

				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTexture.size());
				textures.push_back(specular);
				loadedTexture.push_back(specular);
				loadedTextureName.push_back(texPath);

			}
		}
	}

	return textures;
}

std::vector<Vertex> Model::assembleVertices(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> textUVs) {

	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++) {

		vertices.push_back(
			Vertex{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				textUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec) {

	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i += 2) {

		vectors.push_back(glm::vec2(floatVec[i + 1], floatVec[i]));
	}
	return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec) {

	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i += 3) {

		vectors.push_back(glm::vec3(floatVec[i + 2], floatVec[i + 1], floatVec[i]));
	}
	return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec) {

	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i += 4) {

		vectors.push_back(glm::vec4(floatVec[i + 3], floatVec[i + 2], floatVec[i + 1], floatVec[i]));
	}
	return vectors;
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix) {

	json node = m_json["nodes"][nextNode];
	std::cout << "MODEL: Przechodzenie przez node #" << nextNode << '\n';

	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end()) {

		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++) {
			transValues[i] = (node["translation"][i]);
		}
		translation = glm::make_vec3(transValues);
	}

	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end()) {

		float rotValues[4] = {
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2],
		};
		rotation = glm::make_quat(rotValues);
	}

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end()) {

		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++) {
			scaleValues[i] = (node["scale"][i]);
		}
		scale = glm::make_vec3(scaleValues);
	}

	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end()) {

		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++) {
			matValues[i] = (node["matrix"][i]);
		}
		matNode = glm::make_mat4(matValues);
	}

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	if (node.find("mesh") != node.end()) {

		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

	if (node.find("children") != node.end()) {

		for (unsigned int i = 0; i < node["children"].size(); i++) {
			traverseNode(node["children"][i], matNextNode);
		}
	}
}