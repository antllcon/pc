#include "ThemeModel.h"
#include "src/system/AppConfig.h"

namespace
{
ThemeData CreateDarkTheme()
{
	ThemeData data;
	data.isDark = true;
	data.windowBackground = AppConfig::DarkTheme::WINDOW_BG;
	data.surfaceBackground = AppConfig::DarkTheme::SURFACE_BG;
	data.surfaceOutline = AppConfig::DarkTheme::SURFACE_OUTLINE;
	data.primaryText = AppConfig::DarkTheme::PRIMARY_TEXT;

	return data;
}

ThemeData CreateLightTheme()
{
	ThemeData data;
	data.isDark = false;
	data.windowBackground = AppConfig::LightTheme::WINDOW_BG;
	data.surfaceBackground = AppConfig::LightTheme::SURFACE_BG;
	data.surfaceOutline = AppConfig::LightTheme::SURFACE_OUTLINE;
	data.primaryText = AppConfig::LightTheme::PRIMARY_TEXT;

	return data;
}
}
ThemeModel::ThemeModel()
	: m_data(CreateDarkTheme())
{
}

void ThemeModel::Invert()
{
	m_data = m_data.isDark ? CreateLightTheme() : CreateDarkTheme();
	NotifyObservers();
}

const ThemeData& ThemeModel::GetData() const
{
	return m_data;
}

ThemeData ThemeModel::GetChangedData() const
{
	return m_data;
}