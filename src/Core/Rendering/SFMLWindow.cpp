#include "SFMLWindow.h"

SFMLWindow::SFMLWindow(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_), rt(rt_), view(*(new sf::View()))
{
	vcenter = sf::Vector2f(0, 0);
	prevVCenter = sf::Vector2f(0, 0);
	zoom = 1.0f;
	prevZoom = 1.0f;
	bgCol = sf::Color(0, 0, 0, 255);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	if (!rt.create(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, settings)) {
		LOG_CRITICAL("Failed to initialise the SFMLWindow render texture.");
		std::exit(2);
	}

	rt.setView(view);
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void SFMLWindow::SetBGColor(ImVec4 color) {
	bgCol = sf::Color((sf::Uint8)(color.x * 255), (sf::Uint8)(color.y * 255), (sf::Uint8)(color.z * 255), (sf::Uint8)(color.w * 255));
}

void SFMLWindow::InfoBarData() {
	std::stringstream posStream;
	posStream << "View: ("  << round(vcenter.x*100.0f)/100.0f << ", " << round(vcenter.y*100.0f)/100.0f << ")";
	ImGui::Text(posStream.str().c_str());

	ImGui::SameLine(0.0f, 25.0f);

	std::stringstream zoomStream;
	zoomStream << "Zoom: " << round(zoom*100.0f)/100.0f;
	ImGui::Text(zoomStream.str().c_str());
}

void SFMLWindow::ToolBarButtons() {
}


void SFMLWindow::ComponentRender() {

}

void SFMLWindow::IMGUIRender() {

}

bool SFMLWindow::ProcessEvent(sf::Event& event) {
	return false;
}

void SFMLWindow::Render() {
	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	ImVec2 pos = GetPos();
	rt.clear(bgCol);

	ComponentRender();

	rt.display();

	float lerpSpeed = 0.7f;

	prevVCenter = sf::Vector2f(std::lerp(prevVCenter.x, vcenter.x, lerpSpeed), std::lerp(prevVCenter.y, vcenter.y, lerpSpeed));

	view.setCenter(sf::Vector2f(prevVCenter.x * contentAvail.x, prevVCenter.y * contentAvail.y) + sf::Vector2f(contentAvail.x / 2.0f, contentAvail.y / 2.0f));
	view.setSize(contentAvail.x, contentAvail.y);
	view.setViewport(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(contentAvail.x / (float)rt.getSize().x, contentAvail.y / (float)rt.getSize().y)));
	prevZoom = std::lerp(prevZoom, zoom, lerpSpeed);
	view.zoom(prevZoom);
	rt.setView(view);

	ImGui::Image(rt, sf::Vector2f((float)rt.getSize().x, (float)rt.getSize().y));
	IMGUIRender();

	InfoBar(25.0f);
	ToolBar(45.0f);

	prevPos = sf::Vector2i((int)pos.x, (int)pos.y);
	prevSize = sf::Vector2u((unsigned int)contentAvail.x, (unsigned int)contentAvail.y);
}