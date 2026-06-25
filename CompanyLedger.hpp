#pragma once

#include "Party.hpp"
#include "PurchaseOrder.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"
#include "Structs.hpp"
#include "Company.hpp"
#include "Distributor.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

class CompanyLedgerPage : public BaseEntity
{
private:
    Party*                       party_;
    int                          partyId_;
    std::vector<LedgerEntry>     entries_;
    std::vector<PurchaseOrder*>  orders_;
    double                       advanceBalance_;

    static std::string entryTypeStr(Ledger_Entry_Type t)
    {
        switch(t) {
            case Ledger_Entry_Type::PurchaseBill:   return "Purchase Bill";
            case Ledger_Entry_Type::SupplierPayment:return "Supplier Payment";
            case Ledger_Entry_Type::AdvanceDeposit: return "Advance to Supplier";
            case Ledger_Entry_Type::AdvanceUsed:    return "Advance Used";
            case Ledger_Entry_Type::Return:         return "Return to Supplier";
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
    CompanyLedgerPage(Party* party)
        : party_(party), partyId_(0), advanceBalance_(0.0)
    {
        if (!party) throw std::invalid_argument("CompanyLedgerPage: party cannot be null");
        partyId_ = party_->getId();
    }

    CompanyLedgerPage(int id, DateTime& c, DateTime& u,
                      Party* party,
                      std::vector<LedgerEntry>    entries,
                      std::vector<PurchaseOrder*> orders,
                      double advanceBalance)
        : BaseEntity(id, c, u),
          party_(party), partyId_(0),
          entries_(entries), orders_(orders),
          advanceBalance_(advanceBalance)
    {
        if (!party) throw std::invalid_argument("CompanyLedgerPage: party cannot be null");
        partyId_ = party_->getId();
    }

    ~CompanyLedgerPage() = default;

    // ── Add order — records bill + any payments already made ─────────────────
    void addOrder(PurchaseOrder* order)
    {
        if (!order) throw std::invalid_argument("CompanyLedgerPage: order cannot be null");
        orders_.push_back(order);

        // Bill entry — we owe supplier this much
        entries_.push_back({DateTime::Now(), Ledger_Entry_Type::PurchaseBill,
                            order->getTotalAmount(),Payment_Mode::Cash, order->getId()});

        // If already paid (Spot), record payment entry too
        if (order->getAmountPaid() > 0)
            entries_.push_back({DateTime::Now(), Ledger_Entry_Type::SupplierPayment,
                                order->getAmountPaid(),order->getPaymentMode(), order->getId()});
        MarkUpdated();
    }

    // ── Payment to supplier against a specific order ──────────────────────────
    void addPayment(double amount, Payment_Mode mode, int orderId = -1)
    {
        if (amount <= 0)
            throw std::invalid_argument("CompanyLedgerPage: payment amount must be positive");
        entries_.push_back({DateTime::Now(), Ledger_Entry_Type::SupplierPayment,
                            amount,mode, orderId});
        MarkUpdated();
    }

    // ── Advance to supplier — not tied to any order ───────────────────────────
    void addAdvance(double amount, Payment_Mode mode)
    {
        if (amount <= 0)
            throw std::invalid_argument("CompanyLedgerPage: advance amount must be positive");
        advanceBalance_ += amount;
        entries_.push_back({DateTime::Now(), Ledger_Entry_Type::AdvanceDeposit,
                            amount,mode, -1});
        MarkUpdated();
    }

    // Use advance against an order
    void useAdvance(double amount, int orderId)
    {
        if (amount <= 0)
            throw std::invalid_argument("CompanyLedgerPage: amount must be positive");
        if (amount > advanceBalance_)
            throw std::runtime_error("CompanyLedgerPage: insufficient advance balance");
        advanceBalance_ -= amount;
        entries_.push_back({DateTime::Now(), Ledger_Entry_Type::AdvanceUsed,
                            amount,Payment_Mode::Cash, orderId});
        MarkUpdated();
    }

    void addEntry(LedgerEntry entry) { entries_.push_back(entry); MarkUpdated(); }

    // ── Lookup ────────────────────────────────────────────────────────────────
    PurchaseOrder* getOrderById(int orderId) const
    {
        for (PurchaseOrder* o : orders_)
            if (o->getId() == orderId) return o;
        throw std::runtime_error("CompanyLedgerPage: order id not found");
    }

    // ── Totals — computed purely from entries ─────────────────────────────────
    double totalPurchased() const
    {
        double total = 0.0;
        for (const LedgerEntry& e : entries_)
            if (e.entryType == Ledger_Entry_Type::PurchaseBill) total += e.amount;
        return total;
    }

    double totalPaid() const
    {
        double total = 0.0;
        for (const LedgerEntry& e : entries_)
            if (e.entryType == Ledger_Entry_Type::SupplierPayment ||
                e.entryType == Ledger_Entry_Type::AdvanceUsed)
                total += e.amount;
        return total;
    }

    double totalDue()       const { return totalPurchased() - totalPaid(); }
    double getAdvanceBalance() const { return advanceBalance_; }

    // ── Getters ───────────────────────────────────────────────────────────────
    int                                  getPartyId()  const { return partyId_; }
    Party*                               getParty()    const { return party_; }
    const std::vector<PurchaseOrder*>&   getOrders()   const { return orders_; }
    const std::vector<LedgerEntry>&      getEntries()  const { return entries_; }

    // ── BaseEntity interface ──────────────────────────────────────────────────
    std::string GetEntityType() const override { return "CompanyLedgerPage"; }

    void Display() const override
    {
        std::cout << "\n=================================================================\n";
        std::cout << "Supplier Ledger: " << party_->getName()
                  << "  (id=" << partyId_ << ")\n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left
                  << std::setw(12) << "Date"
                  << std::setw(22) << "Type"
                  << std::setw(12) << "Amount"
                  << std::setw(20) << "Mode"
                  << std::setw(8)  << "OrderId"
                  << "\n";
        std::cout << "-----------------------------------------------------------------\n";
        for (const LedgerEntry& e : entries_) {
            std::string sign = (e.entryType == Ledger_Entry_Type::PurchaseBill) ? "+" : "-";
            std::cout << std::setw(12) << e.date.ToDateString()
                      << std::setw(22) << entryTypeStr(e.entryType)
                      << std::setw(12) << (sign + "Rs" + std::to_string((int)e.amount))
                      << std::setw(20) << modeTypeStr(e.mode)
                      << std::setw(8)  << (e.orderId == -1 ? "-" : std::to_string(e.orderId))
                      << "\n";
        }
        std::cout << "=================================================================\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total Purchased: Rs" << totalPurchased()  << "\n";
        std::cout << "Total Paid:      Rs" << totalPaid()       << "\n";
        std::cout << "Outstanding:     Rs" << totalDue()        << "\n";
        std::cout << "Advance Balance: Rs" << advanceBalance_   << "\n";
        std::cout << "=================================================================\n";
    }

    std::string Serialize() const override
    {
        return "CompanyLedgerPage|" + std::to_string(getId())
             + "|" + std::to_string(partyId_)
             + "|advanceBalance:" + std::to_string(advanceBalance_)
             + "|entries:" + std::to_string(entries_.size())
             + "|orders:"  + std::to_string(orders_.size());
    }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (!party_)
            throw std::runtime_error("CompanyLedgerPage: party is null");
        if (advanceBalance_ < 0)
            throw std::runtime_error("CompanyLedgerPage: advanceBalance is negative");
    }
};