#include "Application.h"
#include "../scene/main/MainScene.h"
#include "AppConfig.h"

namespace
{
void AssertIsWindowValid(const sf::RenderWindow& window)
{
	if (!window.isOpen())
	{
		throw std::runtime_error("Окно не инициализировано для применения системных настроек");
	}
}

void SetupWindowProperties(sf::RenderWindow& window)
{
	AssertIsWindowValid(window);
	AppConfig::SetTitleBarTheme(window, true);
	AppConfig::SetWindowIconColor(window, AppConfig::ICON_COLOR_LIGHT, AppConfig::ICON_SIZE);
}
}

Application::Application()
	: m_window(sf::VideoMode({AppConfig::WINDOW_WIDTH, AppConfig::WINDOW_HEIGHT}), AppConfig::WINDOW_NAME, AppConfig::WINDOW_STYLE)
	, m_themeModel(std::make_shared<ThemeModel>())
{
	auto refreshRate = AppConfig::GetMonitorRefreshRate();
	m_window.setFramerateLimit(refreshRate);
	SetupWindowProperties(m_window);
}

void Application::Init()
{
	m_themeModel->RegisterObserver(shared_from_this());
	LoadScene(std::make_unique<MainScene>());
}

void Application::Run()
{
	sf::Clock clock;
	while (m_window.isOpen())
	{
		auto dt = clock.restart();

		ProcessEvents();
		Update(dt.asSeconds());
		Render();
	}
}

void Application::Update(const ThemeData& data, IObservable<ThemeData>*)
{
	AppConfig::SetTitleBarTheme(m_window, data.isDark);
	auto iconColor = data.isDark ? AppConfig::ICON_COLOR_LIGHT : AppConfig::ICON_COLOR_DARK;
	AppConfig::SetWindowIconColor(m_window, iconColor, AppConfig::ICON_SIZE);
}

void Application::ProcessEvents()
{
	while (const auto event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}

		if (m_scene)
		{
			try
			{
				m_scene->ProcessEvents(*event, m_window);
			}
			catch (const std::exception& e)
			{
				m_scene->OnException(e);
			}
		}
	}
}

void Application::Update(float dt)
{
	if (m_scene)
	{
		try
		{
			m_scene->Update(dt);
		}
		catch (const std::exception& e)
		{
			m_scene->OnException(e);
		}
	}
}

void Application::Render()
{
	m_window.clear(m_themeModel->GetData().windowBackground);

	if (m_scene)
	{
		m_scene->Render(m_window);
	}

	m_window.display();
}

void Application::LoadScene(std::unique_ptr<Scene> scene)
{
	m_scene = std::move(scene);

	if (m_scene)
	{
		m_scene->Init(m_themeModel);
	}
}