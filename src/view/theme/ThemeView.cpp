#include "ThemeView.h"
#include "../../controller/theme/ThemeController.h"
#include "SFML/Window/Event.hpp"
#include "src/system/AppConfig.h"

ThemeView::ThemeView(std::shared_ptr<ThemeModel> model, std::shared_ptr<ThemeController> controller)
	: m_controller(std::move(controller))
{
	m_background.setSize({static_cast<float>(AppConfig::WINDOW_WIDTH), static_cast<float>(AppConfig::WINDOW_HEIGHT)});

	if (model)
	{
		Update(model->GetData(), nullptr);
	}
}

void ThemeView::HandleEvent(const sf::Event& event, const sf::RenderWindow&)
{
	if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyEvent->code == sf::Keyboard::Key::T)
		{
			m_controller->OnToggleClicked();
		}
	}
}

void ThemeView::Render(sf::RenderWindow& window) const
{
	window.draw(m_background);
}

void ThemeView::Update(const ThemeData& data, IObservable<ThemeData>*)
{
	m_background.setFillColor(data.windowBackground);
}