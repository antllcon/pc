#pragma once
#include "src/common/Types.h"
#include <string>

class IActor
{
public:
	virtual ~IActor() = default;

	virtual void Step() = 0;

	[[nodiscard]] virtual Money GetCashBalance() const = 0;
	[[nodiscard]] virtual Money GetBankBalance() const = 0;
	[[nodiscard]] virtual std::string GetName() const = 0;
};