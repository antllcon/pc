#include "LifeView.h"
#include "../../controller/life/LifeController.h"
#include "SFML/Window/Event.hpp"
#include <stdexcept>

namespace
{
	constexpr float CELL_SIZE = 10.0f;
	constexpr unsigned int VERTICES_PER_QUAD = 6;

	void AssertIsControllerValid(const std::shared_ptr<LifeController>& controller)
	{
		if (!controller)
		{
			throw std::runtime_error("Указатель на контроллер игры не может быть пустым");
		}
	}

	void SetupQuadVertices(sf::Vertex* quad, float x, float y, float size, const sf::Color& color)
	{
		quad[0].position = {x, y};
		quad[1].position = {x + size, y};
		quad[2].position = {x, y + size};

		quad[3].position = {x, y + size};
		quad[4].position = {x + size, y};
		quad[5].position = {x + size, y + size};

		for (unsigned int i = 0; i < VERTICES_PER_QUAD; ++i)
		{
			quad[i].color = color;
		}
	}
}

LifeView::LifeView(
	std::shared_ptr<LifeModel> lifeModel,
	std::shared_ptr<ThemeModel> themeModel,
	std::shared_ptr<LifeController> controller)
	: m_controller(std::move(controller))
	, m_vertices(sf::PrimitiveType::Triangles)
{
	AssertIsControllerValid(m_controller);

	if (themeModel)
	{
		Update(themeModel->GetData(), nullptr);
	}

	if (lifeModel)
	{
		Update(lifeModel->GetData(), nullptr);
	}
}

void LifeView::HandleEvent(const sf::Event& event, const sf::RenderWindow&)
{
	if (const auto* mouseMove = event.getIf<sf::Event::MouseMoved>())
	{
		bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
		m_controller->OnMouseMoved(
			static_cast<float>(mouseMove->position.x),
			static_cast<float>(mouseMove->position.y),
			isLeftPressed
		);
	}
	else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouseButton->button == sf::Mouse::Button::Left)
		{
			m_controller->OnMousePressed(
				static_cast<float>(mouseButton->position.x),
				static_cast<float>(mouseButton->position.y)
			);
		}
	}
	else if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyEvent->code == sf::Keyboard::Key::Space)
		{
			m_controller->OnSpacePressed();
		}
		else if (keyEvent->code == sf::Keyboard::Key::R)
		{
			m_controller->OnRPressed();
		}
		else if (keyEvent->code == sf::Keyboard::Key::C)
		{
			m_controller->OnCPressed();
		}
	}
}

void LifeView::Render(sf::RenderWindow& window) const
{
	window.draw(m_vertices);
}

void LifeView::Update(const LifeData& data, IObservable<LifeData>*)
{
	m_lastLifeData = data;
	RebuildGeometry(m_lastLifeData);
}

void LifeView::Update(const ThemeData& data, IObservable<ThemeData>*)
{
	m_lastThemeData = data;
	UpdateColors();
}

void LifeView::RebuildGeometry(const LifeData& data)
{
	unsigned int aliveCount = 0;
	for (uint8_t cell : data.field)
	{
		if (cell)
		{
			aliveCount++;
		}
	}

	m_vertices.resize(aliveCount * VERTICES_PER_QUAD);

	unsigned int vertexIndex = 0;
	for (unsigned int y = 0; y < data.height; ++y)
	{
		for (unsigned int x = 0; x < data.width; ++x)
		{
			unsigned int cellIndex = y * data.width + x;
			if (data.field[cellIndex])
			{
				float posX = static_cast<float>(x) * CELL_SIZE;
				float posY = static_cast<float>(y) * CELL_SIZE;

				SetupQuadVertices(&m_vertices[vertexIndex], posX, posY, CELL_SIZE, m_lastThemeData.primaryText);
				vertexIndex += VERTICES_PER_QUAD;
			}
		}
	}
}

void LifeView::UpdateColors()
{
	for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
	{
		m_vertices[i].color = m_lastThemeData.primaryText;
	}
}