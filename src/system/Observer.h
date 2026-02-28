#pragma once

#include <functional>
#include <map>
#include <memory>

template <typename T>
class IObservable;

template <typename T>
class IObserver
{
public:
	virtual void Update(T const& data, IObservable<T>* subject) = 0;
	virtual ~IObserver() = default;
};

template <typename T>
class IObservable
{
public:
	using ObserverType = IObserver<T>;
	using ObserverPtr = std::shared_ptr<ObserverType>;

	virtual void RegisterObserver(const ObserverPtr& observer, uint64_t priority = 0) = 0;
	virtual void RemoveObserver(const ObserverPtr& observer) = 0;
	virtual void NotifyObservers() = 0;
	virtual ~IObservable() = default;
};

template <class T>
class CObservable : public IObservable<T>
{
public:
	using ObserverType = typename IObservable<T>::ObserverType;
	using ObserverPtr = typename IObservable<T>::ObserverPtr;
	using WeakObserverPtr = std::weak_ptr<ObserverType>;

	void RegisterObserver(const ObserverPtr& observer, uint64_t priority = 0) override
	{
		const auto [it, inserted] = m_priorityMap.insert({observer, priority});

		if (!inserted)
		{
			return;
		}

		try
		{
			m_observers.insert({priority, observer});
		}
		catch (...)
		{
			m_priorityMap.erase(it);
			throw;
		}
	}

	void RemoveObserver(const ObserverPtr& observer) override
	{
		const auto it = m_priorityMap.find(WeakObserverPtr(observer));
		if (it == m_priorityMap.end())
		{
			return;
		}

		const auto priority = it->second;
		m_priorityMap.erase(it);

		auto range = m_observers.equal_range(priority);
		for (auto itObs = range.first; itObs != range.second; ++itObs)
		{
			if (itObs->second.lock() == observer)
			{
				m_observers.erase(itObs);
				break;
			}
		}
	}

	void NotifyObservers() override
	{
		T data = GetChangedData();
		auto observersCopy = m_observers;
		std::vector<WeakObserverPtr> deadObservers;

		for (auto& weakObs : observersCopy)
		{
			if (auto obs = weakObs.second.lock())
			{
				obs->Update(data, this);
			}
			else
			{
				deadObservers.push_back(weakObs.second);
			}
		}

		if (!deadObservers.empty())
		{
			for (const auto& weakObs : deadObservers)
			{
				m_priorityMap.erase(weakObs);
			}
		}
	}

protected:
	virtual T GetChangedData() const = 0;

private:
	std::map<WeakObserverPtr, uint64_t, std::owner_less<WeakObserverPtr>> m_priorityMap;
	std::multimap<uint64_t, WeakObserverPtr, std::greater<uint64_t>> m_observers;
};