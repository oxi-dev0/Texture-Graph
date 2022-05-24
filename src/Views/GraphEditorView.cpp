#include "GraphEditorView.h"

GraphEditorView::GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
    : SFMLWindow(main_, rt_, name_, flags_)
{
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	float mul = 0.6f;
	SetBGColor(ImVec4(bgColor.x * mul, bgColor.y * mul, bgColor.z * mul, 1));
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void GraphEditorView::Grid() {
	if (prevSize.x == 0) { return; }

	int lineSpacingX = 20;
	int lineSpacingY = 20;
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	float mul = 0.4f;
	ImVec4 col = ImVec4(bgColor.x * mul, bgColor.y * mul, bgColor.z * mul, 1);
	sf::Color lineColor = sf::Color(col.x * 255, col.y * 255, col.z * 255, col.w * 255);

	int xSpace = std::floor((float)rt.mapCoordsToPixel(sf::Vector2f(lineSpacingX, 0)).x - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).x);
	int ySpace = std::floor((float)rt.mapCoordsToPixel(sf::Vector2f(0, lineSpacingY)).y - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).y);

	sf::Vector2f offset = sf::Vector2f((float)((int)(vcenter.x*prevSize.x) % xSpace), (float)((int)(vcenter.y*prevSize.y) % ySpace)); // -sf::Vector2f(lineSpacingX * std::floor(((vcenter.x * prevSize.x) - (prevSize.x / 2)) / sf::Vector2f(rt.mapCoordsToPixel(sf::Vector2f(lineSpacingX, 0)) - rt.mapCoordsToPixel(sf::Vector2f(0, 0))).length()), lineSpacingY * std::floor(((vcenter.y * prevSize.y) - (prevSize.y / 2)) / sf::Vector2f(rt.mapCoordsToPixel(sf::Vector2f(0, lineSpacingY)) - rt.mapCoordsToPixel(sf::Vector2f(0, 0))).length()));

	int totalLinesX = prevSize.x / xSpace;
	int totalLinesY = prevSize.y / ySpace;
	sf::VertexArray lines(sf::Lines, 2+((totalLinesX) + (totalLinesY))*2);

	for (int x = 0; x <= totalLinesX; x+=1) {
		int i1 = x*2;
		int i2 = (x*2) + 1;

		// pixel space
		float xPos = ((float)rt.mapCoordsToPixel(sf::Vector2f(lineSpacingX, 0)).x - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).x) * (float)x;

		lines[i1].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(xPos, 0) - offset)));
		lines[i2].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(xPos, prevSize.y) - offset)));
		lines[i1].color = lineColor;
		lines[i2].color = lineColor;
	}

	for (int y = 0; y <= totalLinesY; y += 1) {
		int i1 = y * 2;
		int i2 = (y * 2) + 1;

		i1 += totalLinesX*2;
		i2 += totalLinesX*2;

		// pixel space
		float yPos = ((float)rt.mapCoordsToPixel(sf::Vector2f(0, lineSpacingY)).y - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).y) * (float)y;

		lines[i1].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(0, yPos) - offset)));
		lines[i2].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(prevSize.x, yPos) - offset)));
		lines[i1].color = lineColor;
		lines[i2].color = lineColor;
	}

	rt.draw(lines);
}

// RENDER STEP FOR THIS VIEW
void GraphEditorView::ImGuiRender() {
	if (moving) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}

	Grid();

    sf::RectangleShape shape2(sf::Vector2f(50.0f, 20.0f));
    shape2.setFillColor(sf::Color::Red);
    rt.draw(shape2);

	sf::RectangleShape shape3(sf::Vector2f(40.0f, 50.0f));
	shape3.setFillColor(sf::Color::Green);
	shape3.setPosition(sf::Vector2f(200.0f, 150.0f));
	rt.draw(shape3);
}

// SFML EVENTS FOR THIS VIEW
void GraphEditorView::ProcessEvent(sf::Event& event) {
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Right) {
			moving = true;
			sf::Vector2i pos = sf::Mouse::getPosition();
			oldPos = sf::Vector2f(pos.x, pos.y);
		}
		else if (event.mouseButton.button == sf::Mouse::Button::Left) {

		}
	} break;
	case  sf::Event::MouseButtonReleased:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Right) {
			moving = false;
		}
	} break;
	case sf::Event::MouseMoved:
	{
		if (!moving)
			break;

		sf::Vector2i pos = sf::Mouse::getPosition();
		sf::Vector2f newPos = sf::Vector2f(pos.x, pos.y);
		sf::Vector2f deltaPos = oldPos - newPos;
		//deltaPos = sf::Vector2f(deltaPos.x * zoom, deltaPos.y * zoom);
		vcenter += sf::Vector2f(deltaPos.x / prevSize.x, deltaPos.y / prevSize.y);

		oldPos = sf::Vector2f(pos.x, pos.y);
	} break;
	case sf::Event::MouseWheelScrolled:
	{
		if (moving)
			break;

		if (event.mouseWheelScroll.delta <= -1)
			zoom = std::min(2.f, zoom + .04f);
		else if (event.mouseWheelScroll.delta >= 1)
			zoom = std::max(.5f, zoom - .04f);
	} break;
    }
}