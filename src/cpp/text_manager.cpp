#include <iostream>
#include <hpp/text_manager.hpp>
#include "freetype/freetype.h"
#include "glad/glad.h"
#include "../project_path.hpp"

void TextManager::initFonts() {
	std::string path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System.ttf";
	std::string italic_path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System_Italic.ttf";
	std::string bold_path = std::string(PROJECT_SOURCE_DIR) + "assets/fonts/SF_Atarian_System_Bold.ttf";
	std::vector<std::string> fontpaths = { path, italic_path, bold_path };


	//The fonts NEED to be initialized properly otherwise the anything subsequent in the program starts breaking
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	FT_Face italic_face;
	FT_Face bold_face;
	std::vector<FT_Face*> faces = { &face, &italic_face, &bold_face };

	//Not character size, this size is just for initial processing

	//Storing texture id of the first 128 ascii characters
	int acc = 0;
	for (FT_Face* face_element : faces) {
		// load character glyph
		if (FT_New_Face(ft, fontpaths[acc].c_str(), 0, face_element)) {
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}
		FT_Set_Pixel_Sizes(*face_element, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(*face_element, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// generate texture
			FT_Face fe = *face_element;
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				fe->glyph->bitmap.width,
				fe->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				fe->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Glyph glyph = {
				texture,
				glm::ivec2(fe->glyph->bitmap.width, fe->glyph->bitmap.rows),
				glm::ivec2(fe->glyph->bitmap_left, fe->glyph->bitmap_top),
				fe->glyph->advance.x
			};

			if (acc == 0) {
				glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
			else if (acc == 1) {
				italic_glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
			else {
				bold_glyphs.insert(std::pair<char, Glyph>(c, glyph));
			}
		}
		FT_Done_Face(*face_element);
		acc++;
	}

	//Enabling blending for EVERYTHING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	FT_Done_FreeType(ft);
	this->capitalSize = glyphs[65].size.y;
}
