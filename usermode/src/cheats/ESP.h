#pragma once
#include "../imgui/imgui.h"
#include "../genCode.h"
#include <string>
#include <vector>
#include <memory>

class ESP
{
public:
	ESP();
	void drawESP(Player& player, std::vector<std::shared_ptr<Entity>>& entities, float viewMatrix[16]);
	void drawSettings();

	bool isEnabled() const { return enabled; }
	void setWindowSize(int width, int height) { windowWidth = width; windowHeight = height; }
	void setWindowOffsets(int x, int y) { offsetX = x; offsetY = y; }
private:
	int windowWidth;
	int windowHeight;
	int offsetX;
	int offsetY;

	bool enabled = true;
	ImColor* enemyColor;

	void drawCenteredText(std::string text, float x1, float x2, float y);
	void drawScalingBarVertical(float x1, float y1, float x2, float y2, float width, ImVec4 color, float value, float maxValue);
};

