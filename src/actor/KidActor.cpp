#include "KidActor.h"

#include "src/common/Economy.h"

KidActor::KidActor(
	std::string name,
	Bank& bank,
	Wallet& apuWallet,
	ILogger& logger)
	: BaseActor(std::move(name), bank, std::nullopt, logger)
	, m_apuWallet(apuWallet)
{
}

void KidActor::Step()
{
	try
	{
		m_wallet.TransferTo(m_apuWallet, Economy::KidCandy);
		m_logger.Log(GetName() + " купил товары у Апу за наличные");
	}
	catch (const InsufficientFundsError&)
	{
	}
}