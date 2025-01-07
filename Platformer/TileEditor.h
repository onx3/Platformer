#pragma once
#include "AstroidsPrivate.h"

class TileEditor
{
public: 
	TileEditor(int width, int height, int tileSize);
	~TileEditor();

	void Initalize();
	void RenderEditor();
	void SaveTilemap(const std::string & filePath);
	void LoadTilemap(const std::string & filePath);

	void SetTile(int x, int y, int tileID);
	int GetTile(int x, int y) const;

private:
	int mWidth;
	int mHeight;
	int mTileSize;
	std::vector<std::vector<int>> mTilemap;
	int mSelectedTileId;
};