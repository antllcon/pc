#pragma once
#include "../../model/life/LifeModel.h"
#include "../../model/theme/ThemeModel.h"
#include "../IView.h"
#include "SFML/Graphics/VertexArray.hpp"
#include "src/system/Observer.h"
#include <memory>

class LifeController;

class LifeView final
	: public IView
	, public IObserver<LifeData>
	, public IObserver<ThemeData>
{
public:
	LifeView(
		std::shared_ptr<LifeModel> lifeModel,
		std::shared_ptr<ThemeModel> themeModel,
		std::shared_ptr<LifeController> controller);

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
	void Render(sf::RenderWindow& window) const override;

	void Update(const LifeData& data, IObservable<LifeData>* subject) override;
	void Update(const ThemeData& data, IObservable<ThemeData>* subject) override;

private:
	void RebuildGeometry(const LifeData& data);
	void UpdateColors();

	std::shared_ptr<LifeController> m_controller;
	sf::VertexArray m_vertices;
	LifeData m_lastLifeData;
	ThemeData m_lastThemeData;
};