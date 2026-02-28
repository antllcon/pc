#pragma once
#include "SFML/Graphics/Color.hpp"
#include "src/system/Observer.h"

struct ThemeData
{
	bool isDark;
	sf::Color windowBackground;
	sf::Color surfaceBackground;
	sf::Color surfaceOutline;
	sf::Color primaryText;
};

class ThemeModel final : public CObservable<ThemeData>
{
public:
	ThemeModel();

	void Invert();
	const ThemeData& GetData() const;

protected:
	ThemeData GetChangedData() const override;

private:
	ThemeData m_data;
};