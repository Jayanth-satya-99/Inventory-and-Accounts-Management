#pragma once

#include "Party.hpp"
#include "SaleOrder.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"
#include "Structs.hpp"
#include "PermanentCustomer.hpp"
#include "RetailShop.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

class CustomerLedgerPage : public BaseEntity
{
private:
    Party*                   party_;
    int                      partyId_;
    std::vector<LedgerEntry> entries_;
    std::vector<SaleOrder*>  orders_;

    // helper: entry type to display string
    static std::string entryTypeStr(Ledger_Entry_Type t)
    {
        switch(t) {
            case Ledger_Entry_Type::SaleBill:       return "Sale Bill";
            case Ledger_Entry_Type::LoanRepayment:  return "Loan Repayment";
            case Ledger_Entry_Type::AdvanceDeposit: return "Advance Deposit";
            case Ledger_Entry_Type::AdvanceUsed:    return "Advance Used";
            case Ledger_Entry_Type::SupplierPayment:return "Spot Payment";
            case Ledger_Entry_Type::Return:         return "Return";
            default:                                return "Other";
        }
    }
    static std::string modeTypeStr(Payment_Mode p){
        switch(p){
            case Payment_Mode::Bank_Transfer:  return "Bank Transfer";
            case Payment_Mode::Card:           return "Card";
            case Payment_Mode::Cash:           return "Cash";
            case Payment_Mode::Cheque:         return "Cheque";
            case Payment_Mode::UPI:            return "UPI";
            default:                           return "Other"; 
        }
    }

public:
    CustomerLedgerPage(Party* party)
        : party_(party), partyId_(0)
    {
        if (!party) throw std::invalid_argument("CustomerLedgerPage: party cannot be null");
        partyId_ = party_->getId();
    }

    CustomerLedgerPage(int id, DateTime& c, DateTime& u,
                       Party* party,
                       std::vector<LedgerEntry> entries,
                       std::vector<SaleOrder*>  orders)
        : BaseEntity(id, c, u),
          party_(party), partyId_(0),
          entries_(entries), orders_(orders)
    {
        if (!party) throw std::invalid_argument("CustomerLedgerPage: party cannot be null");
        partyId_ = party_->getId();
    }

    ~CustomerLedgerPage() = default;

    // ── Add order — extracts all payment entries automatically ───────────────
    void addOrder(SaleOrder* order)
    {
        if (!order) throw std::invalid_argument("CustomerLedgerPage: order cannot be null");
        orders_.push_back(order);

        // Bill entry — positive (customer owes us)
        entries_.push_back({DateTime::Now(), Ledger_Entry_Type::SaleBill,
                            order->getTotalAmount(),Payment_Mode::Cash, order->getId()});

        // Payment entries — each payment recorded separately
        for (const auto& p : order->getPayments()) {
            Ledger_Entry_Type type;
            if      (p.type == Payment_Type::Advance) type = Ledger_Entry_Type::AdvanceUsed;
            else if (p.type == Payment_Type::Loan)    type = Ledger_Entry_Type::LoanRepayment;
            else                                      type = Ledger_Entry_Type::SupplierPayment;
            entries_.push_back({DateTime::Now(), type, p.amount,p.mode, order->getId()});
        }
        MarkUpdated();
    }

    // ── Deposit — auto-splits: loan first, then advance ──────────────────────
    void addDeposit(double amount, Payment_Mode mode)
    {
        if (amount <= 0)
            throw std::invalid_argument("CustomerLedgerPage: deposit amount must be positive");

        double loanDue = totalDue();

        if (loanDue > 0) {
            double towardLoan = std::min(amount, loanDue);
            if (party_->getPartyType() == Party_Type::PermanentCustomer)
                dynamic_cast<PermanentCustomer*>(party_)->repayLoan(towardLoan);
            entries_.push_back({DateTime::Now(), Ledger_Entry_Type::LoanRepayment,
                                towardLoan,mode, -1});
            amount -= towardLoan;
        }

        if (amount > 0) {
            if (party_->getPartyType() == Party_Type::PermanentCustomer)
                dynamic_cast<PermanentCustomer*>(party_)->addAdvance(amount);
            entries_.push_back({DateTime::Now(), Ledger_Entry_Type::AdvanceDeposit,
                                amount,mode, -1});
        }
        MarkUpdated();
    }
    double getAdvanceBalance()const{
        double total=0.0;
        for(const LedgerEntry& e:entries_){
            if(e.entryType == Ledger_Entry_Type::AdvanceDeposit) total += e.amount;
            if(e.entryType == Ledger_Entry_Type::AdvanceUsed)    total -= e.amount;
        }
            
        return total;
    }

    void addEntry(LedgerEntry entry) { entries_.push_back(entry); MarkUpdated(); }

    // ── Lookup ────────────────────────────────────────────────────────────────
    SaleOrder* getOrderById(int orderId) const
    {
        for (SaleOrder* o : orders_)
            if (o->getId() == orderId) return o;
        throw std::runtime_error("CustomerLedgerPage: order id not found");
    }

    // ── Totals — computed purely from entries ─────────────────────────────────
    double totalBilled() const
    {
        double total = 0.0;
        for (const LedgerEntry& e : entries_)
            if (e.entryType == Ledger_Entry_Type::SaleBill) total += e.amount;
        return total;
    }

    double totalReceived() const
    {
        double total = 0.0;
        for (const LedgerEntry& e : entries_)
            if (e.entryType == Ledger_Entry_Type::LoanRepayment  ||
                e.entryType == Ledger_Entry_Type::AdvanceUsed     ||
                e.entryType == Ledger_Entry_Type::SupplierPayment)
                total += e.amount;
        return total;
    }

    double totalDue() const { return totalBilled() - totalReceived(); }

    // ── Getters ───────────────────────────────────────────────────────────────
    int                              getPartyId()  const { return partyId_; }
    Party*                           getParty()    const { return party_; }
    const std::vector<SaleOrder*>&   getOrders()   const { return orders_; }
    const std::vector<LedgerEntry>&  getEntries()  const { return entries_; }

    // ── BaseEntity interface ──────────────────────────────────────────────────
    std::string GetEntityType() const override { return "CustomerLedgerPage"; }

    void Display() const override
    {
        std::cout << "\n=================================================================\n";
        std::cout << "Customer Ledger: " << party_->getName()
                  << "  (id=" << partyId_ << ")\n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left
                  << std::setw(12) << "Date"
                  << std::setw(20) << "Type"
                  << std::setw(12) << "Amount"
                  << std::setw(20) << "Mode"
                  << std::setw(8)  << "OrderId"
                  << "\n";
        std::cout << "-----------------------------------------------------------------\n";
        for (const LedgerEntry& e : entries_) {
            std::string sign = (e.entryType == Ledger_Entry_Type::SaleBill) ? "+" : "-";
            std::cout << std::setw(12) << e.date.ToDateString()
                      << std::setw(20) << entryTypeStr(e.entryType)
                      << std::setw(12) << (sign + "Rs" + std::to_string((int)e.amount))
                      << std::setw(20) << modeTypeStr(e.mode)
                      << std::setw(8)  << (e.orderId == -1 ? "-" : std::to_string(e.orderId))
                      << "\n";
        }
        std::cout << "=================================================================\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total Billed:   Rs" << totalBilled()   << "\n";
        std::cout << "Total Received: Rs" << totalReceived() << "\n";
        std::cout << "Outstanding:    Rs" << totalDue()      << "\n";
        std::cout << "=================================================================\n";
    }

    std::string Serialize() const override
    {
        return "CustomerLedgerPage|" + std::to_string(getId())
             + "|" + std::to_string(partyId_)
             + "|entries:" + std::to_string(entries_.size())
             + "|orders:"  + std::to_string(orders_.size());
    }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (!party_)
            throw std::runtime_error("CustomerLedgerPage: party is null");
        if (totalDue() < -0.001)
            throw std::runtime_error("CustomerLedgerPage: received more than billed");
    }
};