#pragma once

#include "Customer.hpp"

// TemporaryCustomer — walk-in / cash-only customer
// No credit, no loan. Phone is still required.
class TemporaryCustomer : public Customer
{
public:
    TemporaryCustomer(const std::string& name,
                      const std::string& phone,
                      const std::string& email  = "",
                      const std::string& address = "")
        : Customer(name, phone, email, address)
    {
        // Phone is always required for a customer
        validateMembershipFields(phone, address.empty() ? " " : address, "TemporaryCustomer");
        // address is optional for temp customers — we pass " " above just to
        // pass the non-empty check; override with actual value if provided
    }

    // Restore from storage
    TemporaryCustomer(int id, DateTime& c, DateTime& u,
                      const std::string& name,
                      const std::string& phone,
                      const std::string& email,
                      const std::string& address,
                      double totalPurchase,
                      int    totalBills)
        : Customer(id, c, u, name, phone, email, address, totalPurchase, totalBills)
    {}

    // ── Core virtual overrides ────────────────────────────────────────────────
    Party_Type  getCustomerType() const override { return Party_Type::TemporaryCustomer; }
    std::string GetEntityType()   const override { return "TemporaryCustomer"; }

    bool canBuyOnLoan() const override { return false; }

    // ── Display / Serialize / Validate ────────────────────────────────────────
    void Display() const override
    {
        Party::Display();
        std::cout << "  [Temporary/Cash-only]"
                  << "  TotalPurchases=₹" << getTotalPurchases()
                  << "  Bills="           << getTotalBills()
                  << "\n";
    }

    std::string Serialize() const override
    {
        return Party::Serialize()
             + "|Temporary"
             + "|" + std::to_string(getTotalPurchases())
             + "|" + std::to_string(getTotalBills());
    }

    void Validate() const override
    {
        Customer::Validate();
        if (phone_.empty())
            throw std::runtime_error("TemporaryCustomer: phone cannot be empty");
    }
};