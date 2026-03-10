#pragma once
#include <stdexcept>

using AccountId = unsigned long long;
using Money = long long;
using OperationsCount = unsigned long long;

class BankOperationError : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class AccountNotFoundError final : public BankOperationError
{
public:
	AccountNotFoundError()
		: BankOperationError("Счет не найден в системе")
	{
	}
};

class InsufficientFundsError final : public BankOperationError
{
public:
	InsufficientFundsError()
		: BankOperationError("Недостаточно средств для операции")
	{
	}
};

class InvalidAmountError final : public std::out_of_range
{
public:
	InvalidAmountError()
		: std::out_of_range("Сумма операции не может быть отрицательной")
	{
	}
};