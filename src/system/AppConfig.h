#pragma once
#include "Windows.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Window.hpp>
#include <dwmapi.h>
#include <vector>

namespace AppConfig
{
constexpr auto WINDOW_WIDTH = 800u;
constexpr auto WINDOW_HEIGHT = 800u;
constexpr auto WINDOW_NAME = "Life";
constexpr auto WINDOW_STYLE = sf::Style::Titlebar | sf::Style::Close;

constexpr auto FRAMERATE_LIMIT = 144u;
constexpr auto DWM_DARK_MODE_ATTRIBUTE = 20;

constexpr auto FONT_PATH = "static/font.ttf";
constexpr auto FONT_SIZE = 12u;

constexpr auto ICON_COLOR_DARK = sf::Color(32, 32, 32);
constexpr auto ICON_COLOR_LIGHT = sf::Color(240, 240, 240);
constexpr auto ICON_SIZE = 32u;

namespace DarkTheme
{
constexpr auto WINDOW_BG = sf::Color(30, 30, 30, 200);
constexpr auto SURFACE_BG = sf::Color(50, 50, 50, 230);
constexpr auto SURFACE_OUTLINE = sf::Color(80, 80, 80);
constexpr auto PRIMARY_TEXT = sf::Color::White;
}

namespace LightTheme
{
constexpr auto WINDOW_BG = sf::Color(240, 240, 240, 200);
constexpr auto SURFACE_BG = sf::Color(255, 255, 255, 230);
constexpr auto SURFACE_OUTLINE = sf::Color(200, 200, 200);
constexpr auto PRIMARY_TEXT = sf::Color::Black;
}

inline unsigned int GetMonitorRefreshRate()
{
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);

	if (::EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
	{
		return static_cast<unsigned int>(devMode.dmDisplayFrequency);
	}

	return FRAMERATE_LIMIT;
}

inline void SetTitleBarTheme(const sf::RenderWindow& window, bool isDark)
{
	HWND hwnd = window.getNativeHandle();
	BOOL useDarkMode = isDark ? TRUE : FALSE;

	DwmSetWindowAttribute(
		hwnd,
		DWM_DARK_MODE_ATTRIBUTE,
		&useDarkMode,
		sizeof(useDarkMode));
}

inline void SetWindowIconColor(sf::RenderWindow& window, sf::Color color, unsigned int size)
{
	std::vector<uint8_t> pixels(size * size * 4);

	float center = size / 2.0f;
	float outerRadius = size / 2.2f;
	float innerRadius = outerRadius / 2;

	for (unsigned int y = 0; y < size; ++y)
	{
		for (unsigned int x = 0; x < size; ++x)
		{
			float dx = x - center;
			float dy = y - center;
			float distance = std::sqrt(dx * dx + dy * dy);

			size_t index = (y * size + x) * 4;

			if (distance <= outerRadius && distance >= innerRadius)
			{
				pixels[index + 0] = color.r;
				pixels[index + 1] = color.g;
				pixels[index + 2] = color.b;
				pixels[index + 3] = color.a;
			}
			else
			{
				pixels[index + 0] = 0;
				pixels[index + 1] = 0;
				pixels[index + 2] = 0;
				pixels[index + 3] = 0;
			}
		}
	}

	window.setIcon({ size, size }, pixels.data());
}

inline sf::String ToUtf8(const std::string& str)
{
	return sf::String::fromUtf8(str.begin(), str.end());
}

inline sf::Vector2f GetWindowCenter()
{
	return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
}
}