#pragma once
#include "src/system/Observer.h"
#include <string>

struct ToastData
{
	std::string message;
	bool isVisible;
};

class ToastModel final : public CObservable<ToastData>
{
public:
	ToastModel();

	void Show(const std::string& message);
	void Hide();
	const ToastData& GetData() const;

protected:
	ToastData GetChangedData() const override;

private:
	ToastData m_data;
};