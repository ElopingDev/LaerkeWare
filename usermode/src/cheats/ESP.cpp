
#include "ESP.h"
#include <memory>
#include "../imgui/imgui.h"
#include <algorithm>

ESP::ESP():
	enemyColor(new ImColor(255, 0,  0, 255))
{

}
void ESP::drawESP(Player& player, std::vector<std::shared_ptr<Entity>>& entities, float viewMatrix[16])
{
	if (!enabled)
		return;
	for (auto& entity : entities) {
		if (entity->Health <= 0)
			continue;
		vec3 headPos = entity->pos + vec3(0, 0, 40);
		vec3 feetPos = entity->pos - vec3(0, 0, 40);
		vec3 headScreenPos = DirectXWorldToScreen(headPos, viewMatrix, windowWidth, windowHeight);
		vec3 feetScreenPos = DirectXWorldToScreen(feetPos, viewMatrix, windowWidth, windowHeight);

		headScreenPos.y += offsetY;
		feetScreenPos.y += offsetY;
		headScreenPos.x += offsetX;
		feetScreenPos.x += offsetX;

		if (feetScreenPos.y < 0 || headScreenPos.z < 0)
			continue;

		float height = abs(headScreenPos.y - feetScreenPos.y);
		float width = height / 4;

		ImVec2 topLeft(feetScreenPos.x - width, headScreenPos.y);
		ImVec2 topRight(feetScreenPos.x + width, headScreenPos.y);
		
		ImVec2 bottomLeft(feetScreenPos.x - width, feetScreenPos.y);
		ImVec2 bottomRight(feetScreenPos.x + width, feetScreenPos.y);

		ImGui::GetBackgroundDrawList()->AddQuad(topLeft, topRight, bottomRight, bottomLeft, *enemyColor);
		drawCenteredText(entity->getEntityName(), bottomLeft.x, bottomRight.x, feetScreenPos.y + 5);
		drawScalingBarVertical(bottomLeft.x - 6.0f, topRight.y, bottomLeft.x - 2.0f, bottomLeft.y, 1.0f, ImVec4(0.0f, 1.0f, 0.0f, 1.0f), entity->Health, entity->maxHealth);
	}
}

void ESP::drawSettings()
{
	ImGui::Checkbox("Enable ESP", &enabled);
	ImGui::ColorEdit4("Enemy Color", (float*)enemyColor);
}

void ESP::drawCenteredText(std::string text, float x1, float x2, float y)
{
	float availableWidth = x2 - x1;
	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

	if (textSize.x <= 0)
		return;
	float scaleFactor = availableWidth / textSize.x;
	if (scaleFactor == 0)
		return;
	if (scaleFactor < 0)
		scaleFactor *= -1;
	scaleFactor == std::max(.01f, scaleFactor);

	if (textSize.y * scaleFactor < 6.0)
		return;

	//ImGui::PushFont(ImGui::GetFont());
	//ImGui::SetWindowFontScale(scaleFactor);

	float textWidth = textSize.x * scaleFactor;
	float center = (x1 + x2) / 2;

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(center - textWidth / 2, y), IM_COL32(255, 255, 255, 255), text.c_str());

	//ImGui::SetWindowFontScale(1.0f);
	//ImGui::PopFont();
}

void ESP::drawScalingBarVertical(float x1, float y1, float x2, float y2, float width, ImVec4 color, float value, float maxValue)
{
	float widthDiff = x2 - x1;
	float heightDiff = y2 - y1;
	value = std::min(value, maxValue);
	float scaledHeight = heightDiff * (value / maxValue);

	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(255, 255, 255, 255), 0, 0, width);
	ImGui::GetBackgroundDrawList()->AddRectFilled(
		ImVec2(x1, y2 - scaledHeight), // Start drawing from the bottom
		ImVec2(x2, y2),
		ImGui::GetColorU32(color)
	);
}
