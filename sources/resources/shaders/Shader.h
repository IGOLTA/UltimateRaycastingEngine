#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

extern std::ostream* LogStream;
extern std::ostream* ErrorStream;

class Shader
{
public:
	enum ShaderType
	{
		VERTEX,
		FRAGMENT
	};

private:
	std::filesystem::path path;
	GLuint id = 0;
	ShaderType type;

public:

	Shader(std::filesystem::path path, ShaderType type);
	bool load();
	void free();

	ShaderType getType() const;
	GLuint getId() const;
};