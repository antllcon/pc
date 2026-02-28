#pragma once
#include "SFML/Graphics/RenderWindow.hpp"

class IView
{
public:
	virtual ~IView() = default;

	virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;
	virtual void Render(sf::RenderWindow& window) const = 0;
};