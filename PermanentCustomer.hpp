#pragma once
#include "Customer.hpp"
#include <stdexcept>
class PermanentCustomer : public Customer
{
private:
    double creditLimit_;
    double currentLoanBalance_;
    double advanceBalance_;

public:
    PermanentCustomer(const std::string &name, const std::string &phone,
                      const std::string &email, const std::string &address,
                      double creditLimit = 0.0)
        : Customer(name, phone, email, address),
          creditLimit_(creditLimit), currentLoanBalance_(0.0), advanceBalance_(0.0)
    {
        validateMembershipFields(phone, address, "PermanentCustomer");
        if (creditLimit_ < 0)
            throw std::invalid_argument("PermanentCustomer: creditLimit cannot be negative");
    }
    PermanentCustomer(int id, DateTime &c, DateTime &u,
                      const std::string &name, const std::string &phone,
                      const std::string &email, const std::string &address,
                      double totalPurchase, int totalBills,
                      double creditLimit, double currentLoanBalance, double advance)
        : Customer(id, c, u, name, phone, email, address, totalPurchase, totalBills),
          creditLimit_(creditLimit), currentLoanBalance_(currentLoanBalance), advanceBalance_(advance)
    {
        validateMembershipFields(phone, address, "PermanentCustomer");
        if (creditLimit_ < 0)
            throw std::invalid_argument("PermanentCustomer: creditLimit cannot be negative");
        if (currentLoanBalance_ < 0)
            throw std::invalid_argument("PermanentCustomer: loanBalance cannot be negative");
        if (advanceBalance_ < 0)
            throw std::invalid_argument("PermanentCustomer: advance cannot be negative");
    }
    double getCreditLimit() const { return creditLimit_; }
    double getCurrentLoanBalance() const { return currentLoanBalance_; }
    double getAvailableCredit() const { return creditLimit_ - currentLoanBalance_; }
    double getAdvanceBalance() const { return advanceBalance_; }
    Party_Type getCustomerType() const override { return Party_Type::PermanentCustomer; }
    std::string GetEntityType() const override { return "PermanentCustomer"; }
    bool canBuyOnLoan() const override { return true; }
    bool canTakeLoan(double amount) const
    {
        if (amount <= 0)
            return false;
        return (currentLoanBalance_ + amount) <= creditLimit_;
    }
    void setCreditLimit(double limit)
    {
        if (limit < 0)
            throw std::invalid_argument("PermanentCustomer: creditLimit cannot be negative");
        creditLimit_ = limit;
        MarkUpdated();
    }
    void addLoanBalance(double amount)
    {
        if (amount <= 0)
            throw std::invalid_argument("PermanentCustomer: loan amount must be positive");
        if (currentLoanBalance_ + amount > creditLimit_)
            throw std::runtime_error("PermanentCustomer: exceeds credit limit");
        currentLoanBalance_ += amount;
        MarkUpdated();
    }
    void repayLoan(double amount)
    {
        if (amount <= 0)
            throw std::invalid_argument("PermanentCustomer: repay amount must be positive");
        if (amount > currentLoanBalance_)
            throw std::runtime_error("PermanentCustomer: repay amount exceeds outstanding balance");
        currentLoanBalance_ -= amount;
        MarkUpdated();
    }
    void addAdvance(double amount)
    {
        if (amount <= 0)
            throw std::invalid_argument("PermanentCustomer: amount to addAdvance must be positive");
        advanceBalance_ += amount;
        MarkUpdated();
    }
    void useAdvance(double amount)
    {
        if (amount <= 0)
            throw std::invalid_argument("PermanentCustomer: should use positive amount");
        if (amount > advanceBalance_)
            throw std::invalid_argument("PermanentCustomer: advance use amount exceeds advanceBalance");
        advanceBalance_ -= amount;
        MarkUpdated();
    }
    
    void Display() const override
    {
        Party::Display();
        std::cout << "  [Permanent]"
                  << "  CreditLimit=Rs" << creditLimit_
                  << "  LoanBalance=Rs" << currentLoanBalance_
                  << "  AvailableCredit=Rs" << getAvailableCredit()
                  << "  AdvanceBalance=Rs" << advanceBalance_
                  << "  TotalPurchases=Rs" << getTotalPurchases()
                  << "  Bills=" << getTotalBills() << "\n";
    }
    std::string Serialize() const override
    {
        return Party::Serialize() + "|Permanent" + "|" + std::to_string(creditLimit_) + "|" + std::to_string(currentLoanBalance_) + "|" + std::to_string(advanceBalance_) + "|" + std::to_string(getTotalPurchases()) + "|" + std::to_string(getTotalBills());
    }
    void Validate() const override
    {
        Customer::Validate();
        if (phone_.empty())
            throw std::runtime_error("PermanentCustomer: phone cannot be empty");
        if (phone_.size() != 10)
            throw std::runtime_error("PermanentCustomer: phone must be 10 digits");
        for (size_t i = 0; i < 10; i++)
            if (phone_[i] > '9' || phone_[i] < '0')
                throw std::runtime_error("PermanentCustomer: phone digits 0-9 only");
        if (address_.empty())
            throw std::runtime_error("PermanentCustomer: address is empty");
        if (creditLimit_ < 0)
            throw std::runtime_error("PermanentCustomer: creditLimit is negative");
        if (currentLoanBalance_ < 0)
            throw std::runtime_error("PermanentCustomer: loanBalance is negative");
        if (currentLoanBalance_ > creditLimit_)
            throw std::runtime_error("PermanentCustomer: loanBalance exceeds creditLimit");
        if (advanceBalance_ < 0)
            throw std::runtime_error("PermanentCustomer: advanceBalance is negative");
    }
};