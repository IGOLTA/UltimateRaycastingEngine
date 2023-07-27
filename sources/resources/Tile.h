#pragma once

#include <map>

#include "Texture.h"

class Tile
{
private:
	static const std::filesystem::path TilesPath;
	static std::vector<Tile*> LoadedTiles;
	
	std::string name;
	Texture* baseTexture;

	Tile(std::filesystem::path path);
public:
	static void LoadTiles();
	static void UnloadTiles();
	static const Tile* GetTile(int id);

	static int GetTileNumber();

	~Tile();

	const Texture* getBaseTexture() const;
	std::string  getName() const;
};

