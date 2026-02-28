#include "ToastModel.h"

ToastModel::ToastModel()
{
	m_data.isVisible = false;
}

void ToastModel::Show(const std::string& message)
{
	m_data.message = message;
	m_data.isVisible = true;
	NotifyObservers();
}

void ToastModel::Hide()
{
	if (m_data.isVisible)
	{
		m_data.isVisible = false;
		NotifyObservers();
	}
}

const ToastData& ToastModel::GetData() const
{
	return m_data;
}

ToastData ToastModel::GetChangedData() const
{
	return m_data;
}