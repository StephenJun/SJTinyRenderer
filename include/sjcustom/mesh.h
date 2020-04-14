#ifndef MESH_H
#define MESH_H

#endif // !MESH_H


#include <glad/glad.h> //包含了所有的OpenGL的声明

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sjcustom/Shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
	//顶点位置
	glm::vec3 Position;
	//顶点法线
	glm::vec3 Normal;
	//纹理坐标
	glm::vec2 TexCoords;
	//切线
	glm::vec3 Tangent;
	//副切线
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {

public :
	/*网格数据*/
	vector<Vertex> vertices;
	vector<unsigned int> indices; // 三角形索引
	vector<Texture> textures;
	unsigned int VAO;

	/*方法*/
	//构造器
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		//拿到所有的顶点数据后就可以设置顶点buffer和属性指针了
		setupMesh();
	}

	void Draw(Shader shader) {
		//绑定分配的纹理
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			//对正确的纹理单元设置采样器
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			//最后绑定这个纹理
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		//绘制网格
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//恢复默认设置
		glActiveTexture(GL_TEXTURE0);
	}

private:
	/*渲染数据*/
	unsigned int VBO, EBO;

	/*方法*/
	void setupMesh() {
		//创建buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		//从顶点缓存中加载数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//结构体的内存布局是按照其内部字段有序的
		//因此我们可以简单地传递一个结构体的指针，之后这个结构体可以完美地被翻译成一个vec3/2的数组，然后再转换成3/2的float，最后转换换成一个字节数组。
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//设置顶点属性指针。
		//顶点位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		//顶点法线
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		//顶点纹理坐标
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		//顶点切线
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		//顶点副切线
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
