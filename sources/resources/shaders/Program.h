#pragma once

#include <map>

#include "Shader.h"

extern std::ostream* LogStream;
extern std::ostream* ErrorStream;

class Program
{
private:
	static const std::filesystem::path ShadersPath;
	static std::map<std::string, Program*> LoadedShaders;

	std::filesystem::path path;
	GLuint id = 0;

public:
	static void LoadShaders();
	static void UnloadShaders();
	static const Program* GetShader(std::string name);
	static unsigned int GetShaderId(std::string name);

	Program();
	~Program();
	Program(std::filesystem::path path);
	bool loadShadersAndLink();
	void free();

	GLuint getId() const;
	operator GLuint() const { return getId(); };
};

