#include "TileEditor.h"
#include "imgui.h"
#include <fstream>

TileEditor::TileEditor(int width, int height, int tileSize)
	: mWidth(width)
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
}


//------------------------------------------------------------------------------------------------------------------------

void TileEditor::RenderEditor()
{
	ImGui::Begin("TileEditor");

	ImGui::Text("TilePalette");
	for (int ii = 0; ii < 10; ++ii)
	{
		std::string textureIDLabel = "Tile" + std::to_string(ii);
		if (ImGui::ImageButton(textureIDLabel.c_str(), (ImTextureID)(intptr_t)ii, ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
		{
			mSelectedTileId = ii;
		}
		if ((ii + 1) % 4 != 0)
		{
			ImGui::SameLine();
		}
	}

	ImGui::Separator();
	ImGui::Text("Tilemap");

	for (int yy = 0; yy < mHeight; ++yy)
	{
		for (int xx = 0; xx < mWidth; ++xx)
		{
			ImGui::PushID(yy * mWidth + xx);
			std::string gridTileLabel = "GridTile" + std::to_string(xx) + "_" + std::to_string(yy);
			ImGui::ImageButton(gridTileLabel.c_str(), (ImTextureID)(intptr_t)mTilemap[yy][xx], ImVec2(float(mTileSize), float(mTileSize)), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				SetTile(xx, yy, mSelectedTileId);
			}

			ImGui::PopID();
			if (xx < mWidth - 1)
			{
				ImGui::SameLine();
			}
		}
	}
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