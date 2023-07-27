#include "Tile.h"

const std::filesystem::path Tile::TilesPath = std::filesystem::path("resources/tiles");
std::vector<Tile*> Tile::LoadedTiles = std::vector<Tile*>();


Tile::Tile(std::filesystem::path path) {
	this->baseTexture = new Texture(path / "base_color.png");
	name = path.filename().string();
}

Tile::~Tile() {
	delete this->baseTexture;
}

void Tile::LoadTiles() {
	if (LoadedTiles.size() != 0) UnloadTiles();

	*LogStream << "Loading Tiles from " << TilesPath << std::endl;
	for (const auto& entry : std::filesystem::directory_iterator(TilesPath)) {
		if (!std::filesystem::is_directory(entry.path())) continue;

		Tile* prog = new Tile(entry.path());
		LoadedTiles.push_back(prog);
	}
}

void Tile::UnloadTiles() {
	for (auto t : LoadedTiles) {
		delete t;
	}

	LoadedTiles = std::vector<Tile*>();
}

const Tile* Tile::GetTile(int id) {
	return LoadedTiles[id-1];
}

int Tile::GetTileNumber() {
	return LoadedTiles.size();
}

const Texture* Tile::getBaseTexture() const {
	return baseTexture;
}

std::string Tile::getName() const {
	return name;
}