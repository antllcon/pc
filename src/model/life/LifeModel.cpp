#include "LifeModel.h"

#include "src/system/AppConfig.h"

#include <random>
#include <stdexcept>
#include <string_view>

namespace
{
constexpr unsigned int FIELD_WIDTH = AppConfig::WINDOW_WIDTH / 10;
constexpr unsigned int FIELD_HEIGHT = AppConfig::WINDOW_HEIGHT / 10;
constexpr float DEFAULT_UPDATE_INTERVAL = 0.1f;

void AssertIsCoordinatesValid(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	if (x >= width || y >= height)
	{
		throw std::out_of_range("Координаты клетки выходят за пределы игрового поля");
	}
}

size_t CalculateHash(const std::vector<uint8_t>& field)
{
	std::string_view view(reinterpret_cast<const char*>(field.data()), field.size());
	return std::hash<std::string_view>{}(view);
}

unsigned int CountAliveNeighbors(const std::vector<uint8_t>& field, int x, int y, int width, int height)
{
	unsigned int count = 0;

	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			if (dx == 0 && dy == 0)
			{
				continue;
			}

			int nx = (x + dx + width) % width;
			int ny = (y + dy + height) % height;

			if (field[ny * width + nx])
			{
				count++;
			}
		}
	}

	return count;
}

bool GenerateLife(double probability)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::bernoulli_distribution dist(probability);
	return dist(gen);
}
} // namespace

LifeModel::LifeModel()
	: m_threadPool(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4)
	, m_timer(0.0f)
	, m_updateInterval(DEFAULT_UPDATE_INTERVAL)
{
	m_data.width = FIELD_WIDTH;
	m_data.height = FIELD_HEIGHT;
	m_data.isPaused = true;
	m_data.isGameOver = false;

	m_data.field.resize(m_data.width * m_data.height, 0);
	m_nextField.resize(m_data.width * m_data.height, 0);
}

void LifeModel::Update(float dt)
{
	if (m_data.isPaused || m_data.isGameOver)
	{
		return;
	}

	m_timer += dt;

	if (m_timer >= m_updateInterval)
	{
		m_timer = 0.0f;
		PerformSimulationStep();
		NotifyObservers();
	}
}

void LifeModel::ToggleCell(unsigned int x, unsigned int y)
{
	AssertIsCoordinatesValid(x, y, m_data.width, m_data.height);

	unsigned int index = y * m_data.width + x;
	m_data.field[index] = m_data.field[index] ? 0 : 1;
	m_data.isGameOver = false;

	NotifyObservers();
}

void LifeModel::TogglePause()
{
	if (!m_data.isGameOver)
	{
		m_data.isPaused = !m_data.isPaused;
		NotifyObservers();
	}
}

void LifeModel::Clear()
{
	std::ranges::fill(m_data.field, uint8_t{0});
	m_history.clear();
	m_data.isPaused = true;
	m_data.isGameOver = false;
	m_timer = 0.0f;

	NotifyObservers();
}

void LifeModel::Randomize(double probability)
{
	for (auto& cell : m_data.field)
	{
		cell = GenerateLife(probability) ? 1 : 0;
	}

	m_history.clear();
	m_data.isPaused = true;
	m_data.isGameOver = false;
	m_timer = 0.0f;

	NotifyObservers();
}

const LifeData& LifeModel::GetData() const
{
	return m_data;
}

LifeData LifeModel::GetChangedData() const
{
	return m_data;
}

void LifeModel::PerformSimulationStep()
{
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0)
	{
		numThreads = 4;
	}

	unsigned int rowsPerThread = m_data.height / numThreads;
	std::vector<std::future<bool>> futures;

	for (unsigned int i = 0; i < numThreads; ++i)
	{
		unsigned int startRow = i * rowsPerThread;
		unsigned int endRow = (i == numThreads - 1) ? m_data.height : startRow + rowsPerThread;

		futures.push_back(m_threadPool.Dispatch(
			[this, startRow, endRow]() -> bool {
				bool localChanged = false;
				int w = static_cast<int>(m_data.width);
				int h = static_cast<int>(m_data.height);

				for (unsigned int y = startRow; y < endRow; ++y)
				{
					for (unsigned int x = 0; x < m_data.width; ++x)
					{
						unsigned int aliveNeighbors = CountAliveNeighbors(m_data.field, static_cast<int>(x), static_cast<int>(y), w, h);
						unsigned int index = y * m_data.width + x;
						bool isAlive = m_data.field[index] != 0;

						if (isAlive)
						{
							if (aliveNeighbors == 2 || aliveNeighbors == 3)
							{
								m_nextField[index] = 1;
							}
							else
							{
								m_nextField[index] = 0;
								localChanged = true;
							}
						}
						else
						{
							if (aliveNeighbors == 3)
							{
								m_nextField[index] = 1;
								localChanged = true;
							}
							else
							{
								m_nextField[index] = 0;
							}
						}
					}
				}

				return localChanged;
			}));
	}

	bool hasChanged = false;

	for (auto& fut : futures)
	{
		if (fut.get())
		{
			hasChanged = true;
		}
	}

	m_data.field = m_nextField;
	CheckEndGameConditions(hasChanged);
}

void LifeModel::CheckEndGameConditions(bool hasChanged)
{
	if (!hasChanged)
	{
		m_data.isGameOver = true;
		m_data.isPaused = true;
		return;
	}

	bool allDead = true;

	for (uint8_t cell : m_data.field)
	{
		if (cell)
		{
			allDead = false;
			break;
		}
	}

	if (allDead)
	{
		m_data.isGameOver = true;
		m_data.isPaused = true;
		return;
	}

	size_t currentHash = CalculateHash(m_data.field);

	if (m_history.find(currentHash) != m_history.end())
	{
		m_data.isGameOver = true;
		m_data.isPaused = true;
		return;
	}

	SaveStateToHistory();
}

void LifeModel::SaveStateToHistory()
{
	m_history.insert(CalculateHash(m_data.field));
}