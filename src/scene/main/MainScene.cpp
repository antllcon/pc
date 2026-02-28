#include "MainScene.h"
#include "../../controller/theme/ThemeController.h"
#include "../../view/theme/ThemeView.h"
#include "../../view/toast/ToastView.h"
#include "src/controller/life/LifeController.h"
#include "src/view/life/LifeView.h"

void MainScene::Init(std::shared_ptr<ThemeModel> themeModel)
{
	auto themeController = std::make_shared<ThemeController>(themeModel);
	AddController(themeController);

	auto themeView = std::make_shared<ThemeView>(themeModel, themeController);
	themeModel->RegisterObserver(themeView);
	AddView(themeView);

	auto lifeModel = std::make_shared<LifeModel>();
	AddModel(lifeModel);

	auto lifeController = std::make_shared<LifeController>(lifeModel);
	AddController(lifeController);

	auto lifeView = std::make_shared<LifeView>(lifeModel, themeModel, lifeController);
	lifeModel->RegisterObserver(lifeView);
	themeModel->RegisterObserver(lifeView);
	AddView(lifeView);

	auto toastModel = std::make_shared<ToastModel>();
	AddModel(toastModel);

	m_toastController = std::make_shared<ToastController>(toastModel);
	AddController(m_toastController);

	auto toastView = std::make_shared<ToastView>(toastModel, themeModel);
	toastModel->RegisterObserver(toastView);
	themeModel->RegisterObserver(toastView);
	AddView(toastView);
}

void MainScene::OnException(const std::exception& e)
{
	if (m_toastController)
	{
		m_toastController->ShowError(e.what());
	}
}