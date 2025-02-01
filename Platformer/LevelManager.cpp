#include "AstroidsPrivate.h"
#include "LevelManager.h"
#include <fstream>
#include <iostream>

LevelManager::LevelManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mTileData()
	, mWidth(0)
	, mHeight(0)
	, mTileWidth(0)
	, mTileHeight(0)
	, mTileTexture()
	, mTileSprites()
{

}

//------------------------------------------------------------------------------------------------------------------------

LevelManager::~LevelManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::LoadLevel(const std::string & filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		printf("Failed to open file %s, is already open.", filePath.c_str());
		return false;
	}

	json levelData;
	file >> levelData;

	mWidth = levelData["width"];
	mHeight = levelData["height"];
	mTileWidth = levelData["tilewidth"];
	mTileHeight = levelData["tileheight"];

	ParseTileData(levelData);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::Render(sf::RenderWindow & window)
{
	for (const auto & sprite : mTileSprites)
	{
		window.draw(sprite);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ClearLevel()
{
	mTileData.clear();
	mTileSprites.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ParseTileData(const json & levelData)
{
    mTileSprites.clear();
    mTileData.clear();

    if (!levelData.contains("layers") || !levelData["layers"].is_array())
    {
        printf("No Layers found");
        return;
    }

    for (const auto & layer : levelData["layers"])
    {
        if (layer.contains("type") && layer["type"] == "tilelayer" && layer.contains("data"))
        {
            const auto & data = layer["data"];
            if (!data.is_array())
            {
                std::cerr << "Invalid level file: Layer data is not an array!" << std::endl;
                return;
            }

            mTileData.resize(mHeight, std::vector<int>(mWidth));

            for (int y = 0; y < mHeight; ++y)
            {
                for (int x = 0; x < mWidth; ++x)
                {
                    int tileID = data[y * mWidth + x].get<int>();
                    mTileData[y][x] = tileID;

                    if (tileID > 0)
                    {
                        sf::Sprite sprite;
                        sprite.setTexture(mTileTexture);

                        int tilesetColumns = mTileTexture.getSize().x / mTileWidth;
                        int tileX = (tileID - 1) % tilesetColumns;
                        int tileY = (tileID - 1) / tilesetColumns;

                        sprite.setTextureRect(sf::IntRect(tileX * mTileWidth, tileY * mTileHeight, mTileWidth, mTileHeight));
                        sprite.setPosition(static_cast<float>(x * mTileWidth), static_cast<float>(y * mTileHeight));

                        mTileSprites.push_back(sprite);
                    }
                }
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------