#include "SFMLWindow.h"

SFMLWindow::SFMLWindow(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_), rt(rt_), view(*(new sf::View()))
{
	vcenter = sf::Vector2f(0, 0);
	zoom = 1.0f;
	prevZoom = 1.0f;
	bgCol = sf::Color(0, 0, 0, 255);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	if (!rt.create(sf::VideoMode::getDesktopMode().width*1.0f, sf::VideoMode::getDesktopMode().height*1.0f, settings)) {
		LOG_CRITICAL("Failed to initialise the SFMLWindow render texture.");
		std::exit(2);
	}
	LOG_INFO("Initilised SFML Texture ({0}x{1})", rt.getSize().x, rt.getSize().y);

	rt.setView(view);
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void SFMLWindow::SetBGColor(ImVec4 color) {
	bgCol = sf::Color((sf::Uint8)(color.x * 255), (sf::Uint8)(color.y * 255), (sf::Uint8)(color.z * 255), (sf::Uint8)(color.w * 255));
}

void SFMLWindow::InfoBar(float height) {
	ImGui::SetNextWindowPos(ImVec2(prevPos.x, prevPos.y + prevSize.y - height));
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_AlwaysUseWindowPadding;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(bgColor.x * 1.0f, bgColor.y * 1.0f, bgColor.z * 1.0f, 1.0f));

	std::stringstream idStream;
	idStream << name << "INFOBAR";
	ImGui::BeginChild(idStream.str().c_str(), ImVec2(prevSize.x, height), false, window_flags);

	ImGui::PopStyleColor();

	std::stringstream posStream;
	posStream << "View: ("  << round(vcenter.x*100.0f)/100.0f << ", " << round(vcenter.y*100.0f)/100.0f << ")";
	ImGui::Text(posStream.str().c_str());

	ImGui::SameLine(0.0f, 25.0f);

	std::stringstream zoomStream;
	zoomStream << "Zoom: " << round(zoom*100.0f)/100.0f;
	ImGui::Text(zoomStream.str().c_str());

	ImGui::EndChild();
	ImGui::PopStyleVar(2);
}

void SFMLWindow::ComponentRender() {

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

	view.setCenter(sf::Vector2f(vcenter.x * contentAvail.x, vcenter.y * contentAvail.y) + sf::Vector2f(contentAvail.x / 2.0f, contentAvail.y / 2.0f));
	view.setSize(contentAvail.x, contentAvail.y);
	view.setViewport(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(contentAvail.x / (float)rt.getSize().x, contentAvail.y / (float)rt.getSize().y)));
	prevZoom = std::lerp(prevZoom, zoom, 0.8f);
	view.zoom(prevZoom);
	rt.setView(view);

	ImGui::Image(rt, sf::Vector2f((float)rt.getSize().x, (float)rt.getSize().y));
	InfoBar(25.0f);

	prevPos = sf::Vector2i((int)pos.x, (int)pos.y);
	prevSize = sf::Vector2u((unsigned int)contentAvail.x, (unsigned int)contentAvail.y);
}