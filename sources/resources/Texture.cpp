#include "Texture.h"

Texture::Texture() {}

Texture::Texture(std::filesystem::path path) {
	this->path = path;
    load();
}

Texture::~Texture() {
	if (id != 0) free();
}

bool Texture::load() {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.string().c_str(), & width, & height, & nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        *ErrorStream << "Failed to load texture " << path << std::endl;
        free();
        return false;
    }
    stbi_image_free(data);
    *LogStream << "Loaded texture " << path << std::endl;
    return true;
}

void Texture::free() {
   glDeleteTextures(1, &id);
}

unsigned int Texture::getId() const {
    return id;
}