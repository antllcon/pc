#include "MainScene.h"
#include "../../controller/simulation/SimulationController.h"
#include "../../controller/theme/ThemeController.h"
#include "../../model/simulation/SimulationModel.h"
#include "../../view/simulation/SimulationView.h"
#include "../../view/theme/ThemeView.h"
#include "../../view/toast/ToastView.h"

void MainScene::Init(std::shared_ptr<ThemeModel> themeModel)
{
	auto themeController = std::make_shared<ThemeController>(themeModel);
	AddController(themeController);

	auto themeView = std::make_shared<ThemeView>(themeModel, themeController);
	themeModel->RegisterObserver(themeView);
	AddView(themeView);

	auto simulationModel = std::make_shared<SimulationModel>();
	AddModel(simulationModel);

	auto simulationController = std::make_shared<SimulationController>(simulationModel);
	AddController(simulationController);

	auto simulationView = std::make_shared<SimulationView>(simulationModel, simulationController);
	simulationModel->RegisterObserver(simulationView);
	AddView(simulationView);

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