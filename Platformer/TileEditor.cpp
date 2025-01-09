#include "AstroidsPrivate.h"
#include "TileEditor.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

class GameManager;

TileEditor::TileEditor(GameManager * pGameManager, int width, int height, int tileSize)
	: BaseManager(pGameManager)
	, mWidth(width)
	, mHeight(height)
	, mTileSize(tileSize)
	, mSelectedTileId(0)
{
	mTilemap.resize(mHeight, std::vector<int>(mWidth, 0));
}

//------------------------------------------------------------------------------------------------------------------------

TileEditor::~TileEditor()
{
}

//------------------------------------------------------------------------------------------------------------------------

void TileEditor::Initalize()
{
	ImGuiIO & io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	LoadTileTextures();
}


//------------------------------------------------------------------------------------------------------------------------

void TileEditor::RenderEditor()
{
	ImGui::Begin("TileEditor");

	ImGui::Text("TilePalette");
	int count = 0;

	auto * pResourceManager = mpGameManager->GetManager<ResourceManager>();
	if (!pResourceManager)
	{
		ImGui::Text("ResourceManager not available!");
		ImGui::End();
		return;
	}

	for (auto & [tileId, resId] : mTileTextures)
	{
		auto pTexture = pResourceManager->GetTexture(resId);
		if (!pTexture)
		{
			ImGui::Text("Missing texture for Tile ID: %d", tileId);
			continue;
		}

		ImTextureID textureId = reinterpret_cast<ImTextureID>(pTexture.get());

		if (ImGui::ImageButton(std::to_string(tileId).c_str(), textureId, ImVec2(32, 32)))
		{
			mSelectedTileId = tileId;
		}

		if (++count % 4 != 0)
		{
			ImGui::SameLine();
		}
	}

	ImGui::Separator();
	ImGui::Text("Tilemap");
	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		SaveTilemap("TileMap.json");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LoadTilemap("TileMap.json");
	}
	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------

void TileEditor::SetTile(int x, int y, int tileID)
{
	if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
	{
		mTilemap[y][x] = tileID;
	}
}

//------------------------------------------------------------------------------------------------------------------------

int TileEditor::GetTile(int x, int y) const
{
	if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
	{
		return mTilemap[y][x];
	}
	return -1; // Invalid tile
}

//------------------------------------------------------------------------------------------------------------------------

void TileEditor::LoadTileTextures()
{
	namespace fs = std::filesystem;
	std::string folderPath = "Art/Tiles";

	if (!fs::exists(folderPath) || !fs::is_directory(folderPath))
	{
		std::cerr << "Tile folder does not exist or is not a directory: " << folderPath << std::endl;
		return;
	}

	int tileId = 0; // Start assigning tile IDs
	for (const auto & entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_regular_file())
		{
			const std::string filePath = entry.path().string();

			ResourceId resId(filePath);
			mTileTextures.emplace(tileId, resId);

			tileId++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

ImTextureID TileEditor::LoadTextureFromFile(const std::string & filePath)
{
	auto * pResourceManager = mpGameManager->GetManager<ResourceManager>();

	if (pResourceManager)
	{
		ResourceId resId(filePath);
		auto pTexture = pResourceManager->GetTexture(resId);

		if (!pTexture)
		{
			std::cerr << "Failed to load texture: " << filePath << std::endl;
			//return nullptr;
		}

		return reinterpret_cast<ImTextureID>(pTexture.get());
	}
}

//------------------------------------------------------------------------------------------------------------------------

void TileEditor::SaveTilemap(const std::string & filePath)
{
	//json j;
	//j["width"] = mWidth;
	//j["height"] = mHeight;
	//j["tilemap"] = mTilemap;

	//std::ofstream file(filePath);
	//if (file.is_open())
	//{
	//	file << j.dump(4); // Indent with 4 spaces for readability
	//	file.close();
	//}
}

//------------------------------------------------------------------------------------------------------------------------

void TileEditor::LoadTilemap(const std::string & filePath)
{
	/*std::ifstream file(filePath);
	if (file.is_open())
	{
		json j;
		file >> j;
		file.close();

		mWidth = j["width"];
		mHeight = j["height"];
		mTilemap = j["tilemap"].get<std::vector<std::vector<int>>>();
	}*/
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------