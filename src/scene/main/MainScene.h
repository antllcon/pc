#pragma once
#include "../../controller/toast/ToastController.h"
#include "../Scene.h"

class MainScene final : public Scene
{
public:
	void Init(std::shared_ptr<ThemeModel> themeModel) override;
	void OnException(const std::exception& e) override;

private:
	std::shared_ptr<ToastController> m_toastController;
};