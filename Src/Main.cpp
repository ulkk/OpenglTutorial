/**
* @file Main.cpp
*/

#include <Windows.h>
#include"GLFWEW.h"
#include <GL/GL.h>
#pragma comment(lib,"opengl32.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#include<iostream>
#include<vector>
#include"Texture.h"
#include"OffscreenBuffer.h"
#include"Shader.h"

///3D�x�N�^�[�^@@pyoo9
struct Vector3
{
	float x, y, z;
};

///RGBA�J���[�^
struct Color
{
	float r, g, b, a;
};

///���_�f�[�^�^
struct Vertex
{
	glm::vec3 position; ///<���W
	glm::vec4 color; ///<�F
	glm::vec2 texCoord; ///<�e�N�X�`�����W
};

///���_�f�[�^
const Vertex vertices[] = {
	{{-0.5f,-0.3f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f}},
	{{ 0.3f,-0.3f, 0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 0.0f}},
	{{ 0.3f, 0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f}},

	{{-0.3f, 0.3f, 0.1f},{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f}},
	{{-0.3f,-0.5f, 0.1f},{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f}},
	{{ 0.5f,-0.5f, 0.1f},{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f}},
	{{ 0.5f,-0.5f, 0.1f},{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f}},
	{{ 0.5f, 0.3f, 0.1f},{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f}},
	{{-0.3f, 0.3f, 0.1f},{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f}},

	{{-1.0f,-1.0f, 0.5f},{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f}},
	{{ 1.0f,-1.0f, 0.5f},{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f}},
	{{ 1.0f, 1.0f, 0.5f},{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f}},
	{{-1.0f, 1.0f, 0.5f},{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f}},


};

///�C���f�b�N�X�f�[�^
const GLuint indices[] = {
	0,1,2,2,3,0,
	4,5,6,7,8,9,
	10,11,12,12,13,10,
};

/**
* �����`��f�[�^
*/
struct RenderingPart
{
	GLsizei size; ///<�`�悷��C���f�b�N�X��
	GLvoid* offset; ///<�`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g
};

/**
* RenderingPart���쐬����
*
* @param size �`�悷��C���f�b�N�X��
* @param offset �`��J�n�C���f�b�N�X�̃I�t�Z�b�g�i�C���f�b�N�X�P�ʁj
*
* @return �쐬���������`��I�u�W�F�N�g
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{size, reinterpret_cast<GLvoid*>(offset * sizeof(GLuint))
};
}

/**
* �����`��f�[�^���X�g
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12,0),
	MakeRenderingPart(6,12)
};
/**
*Vertex Buffer Object���쐬����
*
*@param size�@���_�f�[�^�̃T�C�Y
*@param data ���_�f�[�^�ւ̃|�C���^
*
*@return �쐬����VBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid*data)
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}

/**
*Index Buffer Object���쐬����
*
*@param size �C���f�b�N�X�f�[�^�̃T�C�Y
*@param data �C���f�b�N�X�f�[�^�ւ̃|�C���^
*
*@return �쐬����IBO
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}
/**���_�A�g���r���[�g��ݒ肷��
*
*@param index	���_�A�g���r���[�g�̃C���f�b�N�X
*@param cls		���_�f�[�^�^��
*@param mbr		���_�A�g���r���[�g�ɐݒ肷��cls�̃����o�ϐ���
*/
#define SetVertexAttribPointer(index,cls,mbr)SetVertexAttribPointerI(\
	index,\
	sizeof(cls::mbr)/sizeof(float),\
	sizeof(cls),\
	reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexAttribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}
/**
* Vertex Array Object���쐬����
*
*@param vbo VAO�Ɋ֘A�t������VBO
*@param ibo VAO�Ɋ֘A�t������IBO
*
*@return �쐬����VAO
*/
GLuint CreateVAO(GLuint vbo,GLuint ibo)
{

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	return vao;
}


/**
* GLFW����̃G���[�񍐂���������B
*
* @param error �G���[�ԍ��B
* @param desc �G���[�̓��e�B
*/
void ErrorCallback(int error, const char* desc)
{
	std::cerr << "ERROR:" << desc << std::endl;
}

///�G���g���[�|�C���g
int main()
{
	GLFWEW::Window&window = GLFWEW::Window::Instance();
	if (!window.Init(800, 600, "OpenGL Tutorial")) {
		return 1;
	}
	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
	const GLuint ibo = CreateIBO(sizeof(indices), indices);
	const GLuint vao = CreateVAO(vbo,ibo);
	const GLuint shaderProgram =
		Shader::CreateProgramFromFile("Res\Tutorial.vert", "Res/Tutorial.frag");
	if (!vbo || !ibo || !vao || !shaderProgram) {
		return 1;
	}
	//�e�N�X�`���f�[�^
	static const uint32_t textureData[] = {
		0xffffffff,0xffcccccc,0xffffffff,0xffcccccc,0xffffffff,
		0xff888888,0xffffffff,0xff888888,0xffffffff,0xff888888,
		0xffffffff,0xff444444,0xffffffff,0xff444444,0xffffffff,
		0xff000000,0xffffffff,0xff000000,0xffffffff,0xff000000,
		0xffffffff,0xff000000,0xffffffff,0xff000000,0xffffffff,
	};

//	TexturePtr tex = Texture::Create(5, 5, GL_RGBA8, GL_RGBA, textureData);
	TexturePtr tex = Texture::LoadFromFile("Res/Sample.bmp");
	if (!tex) {
		return 1;
	}
	glEnable(GL_DEPTH_TEST);
	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800, 600);

	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());

		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static float degree = 0.0f;
		degree += 0.1f;
		if (degree >= 360.0f) { degree -= 360.0f; }
		const glm::vec3 viewPos = glm::rotate(
			glm::mat4(), glm::radians(degree), glm::vec3(0, 1, 0))*glm::vec4(2, 3, 3, 1);
		const GLint colorSamplerLoc = glGetUniformLocation(shaderProgram, "colorSampler");
		if (colorSamplerLoc >= 0) {
			glUniform1i(colorSamplerLoc, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->Id());
		}
		glUseProgram(shaderProgram);
		const GLint matMVPLoc = glGetUniformLocation(shaderProgram, "matMVP");
		if (matMVPLoc >= 0){
			const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
			const glm::mat4x4 matView =
				glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			const glm::mat4x4 matMVP = matProj * matView;
			glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);
		}
		glBindVertexArray(vao);
		glDrawElements(
			GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
			GL_UNSIGNED_INT, reinterpret_cast < const GLvoid* > (0));
		window.SwapBuffers();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (colorSamplerLoc >= 0) {
		glBindTexture(GL_TEXTURE_2D, offscreen->GetTexture());
	}
	glDrawElements(
		GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
		GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	return 0;
}