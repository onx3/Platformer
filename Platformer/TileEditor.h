#pragma once
#include "AstroidsPrivate.h"
#include <unordered_map>
#include "imgui.h"
#include "ResourceManager.h"

class TileEditor : public BaseManager
{
public: 
	TileEditor(GameManager * pGameManager, int width, int height, int tileSize);
	~TileEditor();

	void Initalize();
	void RenderEditor();
	void SaveTilemap(const std::string & filePath);
	void LoadTilemap(const std::string & filePath);

	void SetTile(int x, int y, int tileID);
	int GetTile(int x, int y) const;

private:
	void LoadTileTextures();

	ImTextureID LoadTextureFromFile(const std::string & filePath);

	int mWidth;
	int mHeight;
	int mTileSize;
	int mSelectedTileId;
	std::vector<std::vector<int>> mTilemap;
	std::unordered_map<int, ResourceId> mTileTextures;
};