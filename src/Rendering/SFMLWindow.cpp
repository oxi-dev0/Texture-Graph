#include "SFMLWindow.h"

SFMLWindow::SFMLWindow(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_), rt(rt_), view(*(new sf::View()))
{
	vcenter = sf::Vector2f(0, 0);
	zoom = 1.0f;
	prevZoom = 1.0f;
	bgCol = sf::Color(0, 0, 0, 255);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	if (!rt.create(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, settings)) {
		LOG_CRITICAL("Failed to initialise the SFMLWindow render texture.");
		std::exit(2);
	}

	rt.setView(view);
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void SFMLWindow::SetBGColor(ImVec4 color) {
	bgCol = sf::Color(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
}

void SFMLWindow::ImGuiRender() {

}

void SFMLWindow::ProcessEvent(sf::Event& event) {

}

void SFMLWindow::Render() {
	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	rt.clear(bgCol);

	ImGuiRender();

	rt.display();

	view.setCenter(sf::Vector2f(vcenter.x * contentAvail.x, vcenter.y * contentAvail.y) + sf::Vector2f(contentAvail.x / 2.0f, contentAvail.y / 2.0f));
	view.setSize(contentAvail.x, contentAvail.y);
	view.setViewport(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(contentAvail.x / (float)main.getSize().x, contentAvail.y / (float)main.getSize().y)));
	prevZoom = std::lerp(prevZoom, zoom, 0.6f);
	view.zoom(prevZoom);
	rt.setView(view);

	ImGui::Image(rt, sf::Vector2f(rt.getSize().x, rt.getSize().y));

	ImVec2 pos = GetPos();
	prevPos = sf::Vector2i(pos.x, pos.y);
	prevSize = sf::Vector2u((unsigned int)contentAvail.x, (unsigned int)contentAvail.y);
}