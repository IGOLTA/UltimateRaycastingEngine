#include "Shader.h"

Shader::Shader(std::filesystem::path path, ShaderType type) {
	this->path = path;
	this->type = type;
}

GLuint Shader::getId() const {
	return id;
}

Shader::ShaderType Shader::getType() const {
	return type;
}

bool Shader::load() {
	switch (type) {
	case ShaderType::VERTEX:
		id = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ShaderType::FRAGMENT:
		id = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	}

	*LogStream << "Reading shader " << path  << std::endl;


	std::string code;
	std::ifstream stream(path, std::ios::in);
	if (stream.is_open()) {
		std::stringstream sstr;
		sstr << stream.rdbuf();
		code = sstr.str();
		stream.close();
	}
	else {
		*ErrorStream << "Failed to read the shader file " << path << std::endl;
		this->free();
		return false;
	}

	*LogStream << "Compiling shader " << path  << std::endl;
	char const* source = code.c_str();
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> errorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(id, InfoLogLength, NULL, &errorMessage[0]);
		*ErrorStream << &errorMessage[0] << std::endl;
		this->free();
		return false;
	}

	return true;
}

void Shader::free() {
	glDeleteShader(id);
	id = 0;
}