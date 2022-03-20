#pragma once
#include "components.hpp"

class TextManager {
public:
	TextManager() {};
	~TextManager() {};

	void initFonts();
	std::map<char, Glyph>& getGlyphs() { return this->glyphs; }
	std::map<char, Glyph>& getItalicGlyphs() { return this->italic_glyphs; }
	std::map<char, Glyph>& getBoldGlyphs() { return this->bold_glyphs; }

private:
	std::map<char, Glyph> glyphs;
	std::map<char, Glyph> italic_glyphs;
	std::map<char, Glyph> bold_glyphs;
};
