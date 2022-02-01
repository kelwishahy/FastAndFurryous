#include "hpp/texture_manager.hpp"
#include "../ext/stb_image/stb_image.h"

#include <string>

void TextureManager::loadTextures(
	const std::array<std::string, textureCount>& texturePaths,
	std::array<GLuint, textureCount>& textures,
	std::array<glm::ivec2, textureCount>& textureDimensions) {

	glGenTextures((GLsizei)textures.size(), textures.data());

	for (GLuint i = 0; i < texturePaths.size() - 1; i++) {
		const std::string& path = getTexturePath(texturePaths[i]);
		glm::ivec2& dimensions = textureDimensions[i];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		// Image failed to load
		if (data == NULL) {
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s\n", message.c_str());
			assert(false);
		}

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glHasError();

		stbi_image_free(data);
	}
	glHasError();
}
