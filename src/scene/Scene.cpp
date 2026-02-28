#include "Scene.h"
#include "src/controller/IController.h"
#include "src/view/IView.h"

void Scene::Init(std::shared_ptr<ThemeModel>)
{
}

void Scene::ProcessEvents(const sf::Event& event, const sf::RenderWindow& window)
{
	for (auto& view : m_views)
	{
		view->HandleEvent(event, window);
	}
}

void Scene::Update(float dt)
{
	for (auto& controller : m_controllers)
	{
		controller->Update(dt);
	}
}

void Scene::Render(sf::RenderWindow& window) const
{
	for (auto& view : m_views)
	{
		view->Render(window);
	}
}

void Scene::AddView(std::shared_ptr<IView> view)
{
	m_views.push_back(std::move(view));
}

void Scene::AddController(std::shared_ptr<IController> controller)
{
	m_controllers.push_back(std::move(controller));
}

void Scene::OnException(const std::exception&)
{
}