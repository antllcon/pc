#pragma once
#include "../../model/theme/ThemeModel.h"
#include "../IView.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "src/system/Observer.h"
#include <memory>

class ThemeController;

class ThemeView final
	: public IView
	, public IObserver<ThemeData>
{
public:
	ThemeView(std::shared_ptr<ThemeModel> model, std::shared_ptr<ThemeController> controller);

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
	void Render(sf::RenderWindow& window) const override;
	void Update(const ThemeData& data, IObservable<ThemeData>* subject) override;

private:
	std::shared_ptr<ThemeController> m_controller;
	sf::RectangleShape m_background;
};