#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture
{
public:
	Texture(const std::string& fileName, float newspeed, float newoffsetx, float newoffsety, float newz);

	//void Bind();

	virtual ~Texture();

	float speed, offsetx, offsety, z;
	void Bind();
protected:

private:
	Texture(const Texture& texture) {}
	void operator=(const Texture& texture) {}

	GLuint m_texture;
};

#endif
