#include "ToastView.h"
#include "src/system/AppConfig.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

class ThemeModel;
namespace
{
constexpr float PADDING_X = 20.0f;
constexpr float PADDING_Y = 10.0f;
constexpr float OFFSET_BOTTOM = 30.0f;
constexpr float CORNER_RADIUS = 12.0f;
constexpr unsigned int POINTS_PER_CORNER = 8;

void AssertIsFontLoaded(bool isLoaded)
{
	if (!isLoaded)
	{
		throw std::runtime_error("Не удалось загрузить шрифт для Toast");
	}
}

void SetupRoundedRectangle(sf::ConvexShape& shape, const sf::Vector2f& size, float radius)
{
	shape.setPointCount(POINTS_PER_CORNER * 4);
	radius = std::min({radius, size.x / 2.0f, size.y / 2.0f});

	const sf::Vector2f centers[4] = {
		{size.x - radius, size.y - radius},
		{radius, size.y - radius},
		{radius, radius},
		{size.x - radius, radius}
	};

	for (unsigned int c = 0; c < 4; ++c)
	{
		float startAngle = static_cast<float>(c) * std::numbers::pi_v<float> / 2.0f;

		for (unsigned int i = 0; i < POINTS_PER_CORNER; ++i)
		{
			float angle = startAngle + static_cast<float>(i) / static_cast<float>(POINTS_PER_CORNER - 1) * (std::numbers::pi_v<float> / 2.0f);
			float px = centers[c].x + radius * std::cos(angle);
			float py = centers[c].y + radius * std::sin(angle);
			shape.setPoint(c * POINTS_PER_CORNER + i, {px, py});
		}
	}
}
}

ToastView::ToastView(std::shared_ptr<ToastModel> toastModel, std::shared_ptr<ThemeModel> themeModel)
	: m_text(m_font)
	, m_isVisible(false)
{
	AssertIsFontLoaded(m_font.openFromFile(AppConfig::FONT_PATH));

	m_text.setFont(m_font);
	m_text.setCharacterSize(AppConfig::FONT_SIZE);
	m_text.setFillColor(sf::Color::White);

	m_background.setFillColor(sf::Color(50, 50, 50, 230));

	if (toastModel)
	{
		Update(toastModel->GetData(), nullptr);
	}

	if (themeModel)
	{
		Update(themeModel->GetData(), nullptr);
	}
}

void ToastView::HandleEvent(const sf::Event&, const sf::RenderWindow&)
{
}

void ToastView::Render(sf::RenderWindow& window) const
{
	if (m_isVisible)
	{
		window.draw(m_background);
		window.draw(m_text);
	}
}

void ToastView::Update(const ToastData& data, IObservable<ToastData>*)
{
	m_isVisible = data.isVisible;

	if (m_isVisible)
	{
		SetupVisuals(data.message);
	}
}

void ToastView::Update(const ThemeData& data, IObservable<ThemeData>*)
{
	m_text.setFillColor(data.primaryText);
	m_background.setFillColor(data.surfaceBackground);
	m_background.setOutlineColor(data.surfaceOutline);
}

void ToastView::SetupVisuals(const std::string& message)
{
	m_text.setString(AppConfig::ToUtf8(message));

	sf::FloatRect textBounds = m_text.getLocalBounds();
	float bgWidth = textBounds.size.x + PADDING_X * 2.0f;
	float bgHeight = textBounds.size.y + PADDING_Y * 2.0f;

	SetupRoundedRectangle(m_background, {bgWidth, bgHeight}, CORNER_RADIUS);

	float posX = (static_cast<float>(AppConfig::WINDOW_WIDTH) - bgWidth) / 2.0f;
	float posY = static_cast<float>(AppConfig::WINDOW_HEIGHT) - bgHeight - OFFSET_BOTTOM;

	m_background.setPosition({posX, posY});

	m_text.setPosition({
		posX + PADDING_X,
		posY + PADDING_Y - textBounds.position.y
	});
}