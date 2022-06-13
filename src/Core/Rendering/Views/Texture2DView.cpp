#include "Texture2DView.h"

Texture2DView::Texture2DView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
	: SFMLWindow(main_, rt_, name_, flags_)
{
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	float mul = 0.6f;
	SetBGColor(ImVec4(bgColor.x * mul, bgColor.y * mul, bgColor.z * mul, 1));
	initialRect = sf::FloatRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
	focusedGraph = nullptr;
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

sf::Vector2f Texture2DView::pixelToGraph(sf::Vector2i pixel) {
	return rt.mapPixelToCoords(sf::Vector2i(pixel.x - (int)prevPos.x, pixel.y - (int)prevPos.y));
}

float remapRange3(float value, float low1, float high1, float low2, float high2) {
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

void Texture2DView::InfoBarData() {
	std::stringstream zoomStream;
	zoomStream << "Zoom: " << round(zoom * 100.0f) / 100.0f;
	ImGui::Text(zoomStream.str().c_str());

	if (selectedNode > focusedGraph->nodes.size() - 1) { return; }
	if (focusedGraph == nullptr || selectedNode == -1 || !focusedGraph->nodes[selectedNode]->evaluated) { return; }
	ImGui::SameLine(0.0f, 15.0f);
	std::stringstream nameStream;
	nameStream << "Class: '" << focusedGraph->nodes[selectedNode]->nodeClass << "'";
	ImGui::Text(nameStream.str().c_str());
}

void Texture2DView::ToolBarButtons() {
	if (ImGui::ImageButton(*ImageCache::images["icon-save"], sf::Vector2f(25, 25), 5)) {
		if (focusedGraph == nullptr) { return; }
		if (selectedNode == -1) { return; }
		if (focusedGraph->nodes[selectedNode]->evaluated == false) { return; }

		nfdchar_t* outPath;
		nfdfilteritem_t filterItem[1] = { { "Portable Network Graphics file", "png" } };
		nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, "", "");
		if (result == NFD_OKAY) {
			if (focusedGraph->nodes[selectedNode]->displayTexture.copyToImage().saveToFile(outPath)) {
				LOG_INFO("Successfully saved image to '{0}'", outPath);
			}
			else {
				LOG_ERROR("Error saving image");
			}
		}
		else {
			if (result == NFD_CANCEL) {
				LOG_WARN("User canceled saving image");
			}
			else {
				LOG_ERROR("Error saving image");
			}
		}
	};
}

// RENDER STEP FOR THIS VIEW
void Texture2DView::ComponentRender() {
	sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
	if (moving) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}

	if (focusedGraph == nullptr) { return; }
	if (focusedGraph->nodes.size() <= selectedNode) { selectedNode = -1; }
	if (focusedGraph->selectedNode > -1) {
		selectedNode = focusedGraph->selectedNode;
	}

	if (selectedNode > -1 && selectedNode < focusedGraph->nodes.size()) {
		if (focusedGraph->nodes[selectedNode]->evaluated == false) { return; }

		auto& displayTexture = focusedGraph->nodes[selectedNode]->displayTexture;

		sf::Vector2i texSize = focusedGraph->texSize;

		sf::Sprite transparentSprite;
		transparentSprite.setTexture(*ImageCache::images["transparencyPreview"]);
		sf::Vector2u trSize = ImageCache::images["transparencyPreview"]->getSize();
		auto displayScaleTr = sf::Vector2f(prevSize.y / trSize.y, prevSize.y / trSize.y);
		transparentSprite.setOrigin(sf::Vector2f(trSize.x / 2, trSize.y / 2));
		transparentSprite.setScale(displayScaleTr);
		transparentSprite.setPosition(sf::Vector2f(prevSize.x / 2, prevSize.y / 2));
		transparentSprite.setColor(sf::Color(255, 255, 255, 255));
		rt.draw(transparentSprite);

		sf::Sprite displaySprite;
		displaySprite.setTexture(displayTexture);
		sf::Vector2u tSize = displayTexture.getSize();
		auto displayScale = sf::Vector2f(prevSize.y / tSize.y, prevSize.y / tSize.y);
		displaySprite.setOrigin(sf::Vector2f(tSize.x/2, tSize.y/2));
		displaySprite.setScale(displayScale);
		displaySprite.setPosition(sf::Vector2f(prevSize.x/2, prevSize.y/2));
		displaySprite.setColor(sf::Color(255, 255, 255, 255));
		rt.draw(displaySprite);

		int gridAlpha = (int)std::clamp(remapRange3(zoom, 0.01f, 0.06f, 255.f, 0.f), 0.f, 255.f);
		sf::Color gridCol = sf::Color(255, 0, 0, gridAlpha);

		for (int x = 0; x < texSize.x; x++) {
			float pixelHeight = (displaySprite.getGlobalBounds().height / texSize.y);
			sf::VertexArray horizLine = sf::VertexArray(sf::Lines, 2);
			float y = ((x + 1) * pixelHeight);
			horizLine[0].position = sf::Vector2f(displaySprite.getGlobalBounds().left, y + displaySprite.getGlobalBounds().top);
			horizLine[1].position = sf::Vector2f(displaySprite.getGlobalBounds().width + displaySprite.getGlobalBounds().left, y + displaySprite.getGlobalBounds().top);
			horizLine[0].color = gridCol;
			horizLine[1].color = gridCol;
			rt.draw(horizLine);
		}

		for (int y = 0; y < texSize.y; y++) {
			float pixelWidth = (displaySprite.getGlobalBounds().width / texSize.x);
			sf::VertexArray vertLine = sf::VertexArray(sf::Lines, 2);
			float x = ((y + 1) * pixelWidth);
			vertLine[0].position = sf::Vector2f(x + displaySprite.getGlobalBounds().left, displaySprite.getGlobalBounds().top);
			vertLine[1].position = sf::Vector2f(x + displaySprite.getGlobalBounds().left, displaySprite.getGlobalBounds().height + displaySprite.getGlobalBounds().top);
			vertLine[0].color = gridCol;
			vertLine[1].color = gridCol;
			rt.draw(vertLine);
		}
	}
}

void Texture2DView::IMGUIRender() {
	inFocus = ImGui::IsWindowFocused();
}

// SFML EVENTS FOR THIS VIEW
bool Texture2DView::ProcessEvent(sf::Event& event) {
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Middle) {
			moving = true;
			sf::Vector2i pos = sf::Mouse::getPosition();
			oldPos = sf::Vector2f((float)pos.x, (float)pos.y);
			return true; // gain handling priority
		}
		else if (event.mouseButton.button == sf::Mouse::Button::Left) {
			
		}
	} break;
	case  sf::Event::MouseButtonReleased:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Middle) {
			moving = false;
			return true; // lose handling priority
		}
		else if (event.mouseButton.button == sf::Mouse::Button::Left) {
			
		}
	} break;
	case sf::Event::MouseMoved:
	{
		if (moving)
		{
			sf::Vector2i pos = sf::Mouse::getPosition();
			sf::Vector2f newPos = sf::Vector2f((float)pos.x, (float)pos.y);
			sf::Vector2f deltaPos = oldPos - newPos;
			deltaPos = sf::Vector2f(deltaPos.x * zoom, deltaPos.y * zoom);
			vcenter += sf::Vector2f(deltaPos.x / prevSize.x, deltaPos.y / prevSize.y);

			oldPos = sf::Vector2f((float)pos.x, (float)pos.y);
		}
	} break;
	case sf::Event::MouseWheelScrolled:
	{
		if (moving)
			break;

		float zoomSens;
		float sensMin = 0.005f;
		float sensMax = 0.2f;
		zoomSens = remapRange3(zoom, 0.05f, 0.5f, sensMin, sensMax);
		zoomSens = std::clamp(zoomSens, sensMin, sensMax);

		if (event.mouseWheelScroll.delta <= -1)
			zoom = std::min(1.5f, zoom + zoomSens);
		else if (event.mouseWheelScroll.delta >= 1)
			zoom = std::max(.01f, zoom - zoomSens);
	} break;
	case sf::Event::KeyPressed:
	{
		switch (event.key.code)
		{
		default:
			break;
		}
	} break;
	}

	return false;
}