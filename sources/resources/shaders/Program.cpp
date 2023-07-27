#include "Program.h"

const std::filesystem::path Program::ShadersPath = std::filesystem::path("shaders");
std::map<std::string, Program*> Program::LoadedShaders = std::map<std::string, Program*>();

Program::Program() {
	this->path = "";
}

Program::~Program() {
	if (id != 0) free();
}

Program::Program(std::filesystem::path path) {
	this->path = path;
}

GLuint Program::getId() const {
	return id;
}

bool Program::loadShadersAndLink() {
	*LogStream << "Loading program " << path << std::endl;

	Shader vertex(path / "vert.glsl", Shader::ShaderType::VERTEX);
	Shader fragment(path / "frag.glsl", Shader::ShaderType::FRAGMENT);

	vertex.load();
	fragment.load();

	*LogStream << "Linking program" << std::endl;
	id = glCreateProgram();
	glAttachShader(id, vertex.getId());
	glAttachShader(id, fragment.getId());
	glLinkProgram(id);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check the program
	glGetProgramiv(id, GL_LINK_STATUS, &Result);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		*LogStream << &ProgramErrorMessage[0] << std::endl;

		free();
		return false;

	}
}

void Program::free() {
	glDeleteProgram(id);
	id = 0;
}

void Program::LoadShaders() {
	if (LoadedShaders.size() != 0) UnloadShaders();

	*LogStream << "Loading shaders from " << ShadersPath << std::endl;
	for (const auto& entry : std::filesystem::directory_iterator(ShadersPath)) {
		if (!std::filesystem::is_directory(entry.path())) continue;

		Program* prog = new Program(entry.path());
		prog->loadShadersAndLink();
		LoadedShaders[entry.path().filename().string()] = prog;

	}
		
}

void Program::UnloadShaders() {
	for (auto s : LoadedShaders) {
		delete s.second;
	}

	LoadedShaders = std::map<std::string, Program*>();
}

const Program* Program::GetShader(std::string name) {

	return LoadedShaders[name];
}

unsigned int Program::GetShaderId(std::string name) {

	return LoadedShaders[name]->id;
}