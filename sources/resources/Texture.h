#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include "../stb/stb_image.h"

extern std::ostream* LogStream;
extern std::ostream* ErrorStream;

class Texture
{
private:
	std::filesystem::path path;
	GLuint id = 0;

public:
	Texture();
	~Texture();
	Texture(std::filesystem::path path);
	bool load();
	void free();

	GLuint getId() const;
	operator GLuint() const { return getId(); };
};

