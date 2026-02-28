#pragma once
#include "src/system/Observer.h"
#include "src/system/threadPool/ThreadPool.h"
#include <cstdint>
#include <unordered_set>
#include <vector>

struct LifeData
{
	std::vector<uint8_t> field;
	unsigned int width;
	unsigned int height;
	bool isPaused;
	bool isGameOver;
};

class LifeModel final : public CObservable<LifeData>
{
public:
	LifeModel();

	void Update(float dt);
	void ToggleCell(unsigned int x, unsigned int y);
	void TogglePause();
	void Clear();
	void Randomize(double probability = 0.3);

	const LifeData& GetData() const;

protected:
	LifeData GetChangedData() const override;

private:
	void PerformSimulationStep();
	void CheckEndGameConditions(bool hasChanged);
	void SaveStateToHistory();

	LifeData m_data;
	std::vector<uint8_t> m_nextField;
	ThreadPool m_threadPool;
	std::unordered_set<size_t> m_history;
	float m_timer;
};