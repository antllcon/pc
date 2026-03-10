#include "NelsonActor.h"

#include "src/common/Economy.h"

#include <random>

namespace
{
const std::string NELSON_NAME = "Нельсон";

Money GetRandomStealAmount()
{
	thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<Money> distribution(Economy::NelsonStealMin, Economy::NelsonStealMax);
	return distribution(generator);
}
} // namespace

NelsonActor::NelsonActor(
	Bank& bank,
	Wallet& bartWallet,
	Wallet& apuWallet,
	ILogger& logger)
	: BaseActor(NELSON_NAME, bank, std::nullopt, logger)
	, m_bartWallet(bartWallet)
	, m_apuWallet(apuWallet)
{
}

void NelsonActor::Step()
{
	const Money stealAmount = GetRandomStealAmount();

	try
	{
		m_bartWallet.TransferTo(m_wallet, stealAmount);
		m_logger.Log(GetName() + " украл у Барта наличные");
	}
	catch (const InsufficientFundsError&)
	{
	}

	try
	{
		m_wallet.TransferTo(m_apuWallet, Economy::NelsonCigarettes);
		m_logger.Log(GetName() + " купил сигареты у Апу");
	}
	catch (const InsufficientFundsError&)
	{
	}
}