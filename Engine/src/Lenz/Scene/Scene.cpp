#include"Scene.h"

namespace lenz
{
	std::unordered_map<uint32_t, Shape> Storage::SHAPES;
	std::vector<Texture> Storage::TEXTURES;
	std::vector<std::string> lenz::Storage::TEXTURE_PATHS;
}