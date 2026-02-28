#pragma once
#include "../../model/theme/ThemeModel.h"
#include "../../model/toast/ToastModel.h"
#include "../IView.h"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "src/system/Observer.h"
#include <memory>

class ToastView final
	: public IView
	, public IObserver<ToastData>
	, public IObserver<ThemeData>
{
public:
	ToastView(std::shared_ptr<ToastModel> toastModel, std::shared_ptr<ThemeModel> themeModel);

	void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
	void Render(sf::RenderWindow& window) const override;
	void Update(const ToastData& data, IObservable<ToastData>* subject) override;
	void Update(const ThemeData& data, IObservable<ThemeData>* subject) override;

private:
	void SetupVisuals(const std::string& message);

	sf::Font m_font;
	sf::Text m_text;
	sf::ConvexShape m_background;
	bool m_isVisible;
};