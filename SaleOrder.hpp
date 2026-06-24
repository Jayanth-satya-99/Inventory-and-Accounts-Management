#pragma once

#include "BaseEntity.hpp"
#include "LineItem.hpp"
#include "Party.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"
#include "Customer.hpp"
#include "PermanentCustomer.hpp"
#include "TemporaryCustomer.hpp"
#include "RetailShop.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

class SaleOrder : public BaseEntity
{
public:
    struct PaymentEntry {
        double       amount;
        Payment_Mode mode;
        Payment_Type type;
    };

private:
    std::vector<LineItem*>    items_;
    std::vector<PaymentEntry> payments_;
    Party*                    customer_;
    DateTime                  orderDate_;
    Order_Status              status_;
    bool                      checkedOut_;
    std::string               note_;

public:
    SaleOrder(Party* customer,
              DateTime orderDate,
              const std::string& note = "")
        : customer_(customer),
          orderDate_(orderDate),
          status_(Order_Status::Pending),
          checkedOut_(false),
          note_(note)
    {
        if (!customer_)
            throw std::invalid_argument("SaleOrder: customer cannot be null");
    }

    // Restore from storage
    SaleOrder(int id, DateTime& c, DateTime& u,
              std::vector<LineItem*>    items,
              std::vector<PaymentEntry> payments,
              Party*                    customer,
              DateTime                  orderDate,
              Order_Status              status,
              bool                      checkedOut,
              const std::string&        note)
        : BaseEntity(id, c, u),
          items_(items), payments_(payments),
          customer_(customer), orderDate_(orderDate),
          status_(status), checkedOut_(checkedOut), note_(note)
    {
        if (!customer_)
            throw std::invalid_argument("SaleOrder: customer cannot be null");
        if (checkedOut_ && items_.empty())
            throw std::invalid_argument("SaleOrder: checked-out order cannot have empty items");
    }

    // ── Line items ────────────────────────────────────────────────────────────
    void addLineItem(LineItem* item)
    {
        if (checkedOut_)
            throw std::runtime_error("SaleOrder: order already checked out");
        if (!item)
            throw std::invalid_argument("SaleOrder: LineItem cannot be null");
        items_.push_back(item);
        MarkUpdated();
    }

    // ── Financials ────────────────────────────────────────────────────────────
    double getTotalAmount() const
    {
        double total = 0.0;
        for (LineItem* item : items_)
            if (!item->isReturned()) total += item->getTotal();
        return total;
    }

    double getTotalPaid() const
    {
        double total = 0.0;
        for (const PaymentEntry& p : payments_)
            total += p.amount;
        return total;
    }

    double getAmountDue() const { return getTotalAmount() - getTotalPaid(); }

    // ── Payment ───────────────────────────────────────────────────────────────
    void addPayment(double amount, Payment_Mode mode, Payment_Type type)
    {
        if (checkedOut_)
            throw std::runtime_error("SaleOrder: order already checked out");
        if (amount <= 0)
            throw std::invalid_argument("SaleOrder: payment amount must be positive");
        if (getTotalPaid() + amount > getTotalAmount())
            throw std::invalid_argument("SaleOrder: payment exceeds amount due");

        Party_Type pt = customer_->getPartyType();

        if (pt == Party_Type::TemporaryCustomer && type != Payment_Type::Spot)
            throw std::invalid_argument("SaleOrder: TemporaryCustomer can only pay Spot");
        if (pt == Party_Type::RetailShop && type == Payment_Type::Advance)
            throw std::invalid_argument("SaleOrder: RetailShop cannot use Advance");

        if (type == Payment_Type::Advance) {
            auto* pc = dynamic_cast<PermanentCustomer*>(customer_);
            if (!pc || pc->getAdvanceBalance() < amount)
                throw std::invalid_argument("SaleOrder: insufficient advance balance");
            pc->useAdvance(amount);
        }
        else if (type == Payment_Type::Loan) {
            auto* pc = dynamic_cast<PermanentCustomer*>(customer_);
            if (!pc || !pc->canTakeLoan(amount))
                throw std::invalid_argument("SaleOrder: insufficient credit limit");
            pc->addLoanBalance(amount);
        }
        // Spot — no balance check needed

        payments_.push_back({amount, mode, type});
        status_ = (getAmountDue() == 0) ? Order_Status::Paid : Order_Status::Partial;
        MarkUpdated();
    }

    // ── Confirm ───────────────────────────────────────────────────────────────
    void confirm()
    {
        if (checkedOut_)
            throw std::runtime_error("SaleOrder: already checked out");
        if (items_.empty())
            throw std::runtime_error("SaleOrder: cannot confirm order with no items");

        Party_Type pt = customer_->getPartyType();

        // Update customer stats for individual customers
        if (pt == Party_Type::PermanentCustomer ||
            pt == Party_Type::TemporaryCustomer)
        {
            auto* c = dynamic_cast<Customer*>(customer_);
            c->setTotalBills();
            c->setTotalPurchase(c->getTotalPurchases() + getTotalPaid());
        }
        // RetailShop — no object fields to update

        status_     = Order_Status::Paid;
        checkedOut_ = true;
        MarkUpdated();
    }

    // ── Return item ───────────────────────────────────────────────────────────
    void returnItem(int lineItemId)
    {
        if (customer_->getPartyType() == Party_Type::RetailShop)
            throw std::runtime_error("SaleOrder: returns not allowed for RetailShop");

        for (LineItem* item : items_) {
            if (item->getId() == lineItemId) {
                if (item->isReturned())
                    throw std::runtime_error("SaleOrder: item already returned");
                item->markReturned();
                // PermanentCustomer — refund goes to advance wallet
                if (customer_->getPartyType() == Party_Type::PermanentCustomer)
                    dynamic_cast<PermanentCustomer*>(customer_)->addAdvance(item->getTotal());
                // TemporaryCustomer — owner handles manually, nothing automatic
                MarkUpdated();
                return;
            }
        }
        throw std::runtime_error("SaleOrder: LineItem id not found");
    }

    // ── Getters ───────────────────────────────────────────────────────────────
    Party*                           getCustomer()   const { return customer_; }
    int                              getCustomerId() const { return customer_->getId(); }
    const DateTime&                  getOrderDate()  const { return orderDate_; }
    Order_Status                     getStatus()     const { return status_; }
    bool                             isCheckedOut()  const { return checkedOut_; }
    const std::string&               getNote()       const { return note_; }
    const std::vector<LineItem*>&    getItems()      const { return items_; }
    const std::vector<PaymentEntry>& getPayments()   const { return payments_; }

    void setNote(const std::string& note) { note_ = note; MarkUpdated(); }

    // ── BaseEntity interface ──────────────────────────────────────────────────
    std::string GetEntityType() const override { return "SaleOrder"; }

    void Display() const override
    {
        std::string statusStr;
        switch (status_) {
            case Order_Status::Pending: statusStr = "Pending"; break;
            case Order_Status::Partial: statusStr = "Partial"; break;
            case Order_Status::Paid:    statusStr = "Paid";    break;
        }
        std::cout << "[SaleOrder] id=" << getId()
                  << "  Customer: "   << customer_->getName()
                  << "  Date: "       << orderDate_.ToDateString()
                  << "  Status: "     << statusStr
                  << "  CheckedOut: " << (checkedOut_ ? "Yes" : "No")
                  << "\n"
                  << "  Total=Rs"     << getTotalAmount()
                  << "  Paid=Rs"      << getTotalPaid()
                  << "  Due=Rs"       << getAmountDue();
        if (!note_.empty()) std::cout << "  Note: " << note_;
        std::cout << "\n  Items:\n";
        for (LineItem* item : items_) {
            std::cout << "    " << (item->isReturned() ? "[RETURNED] " : "");
            item->Display();
        }
        std::cout << "  Payments:\n";
        for (const PaymentEntry& p : payments_) {
            std::string typeStr, modeStr;
            switch (p.type) {
                case Payment_Type::Spot:    typeStr = "Spot";    break;
                case Payment_Type::Advance: typeStr = "Advance"; break;
                case Payment_Type::Loan:    typeStr = "Loan";    break;
            }
            switch (p.mode) {
                case Payment_Mode::Cash:          modeStr = "Cash";          break;
                case Payment_Mode::UPI:           modeStr = "UPI";           break;
                case Payment_Mode::Card:          modeStr = "Card";          break;
                case Payment_Mode::Bank_Transfer: modeStr = "Bank Transfer"; break;
                case Payment_Mode::Cheque:        modeStr = "Cheque";        break;
            }
            std::cout << "    Rs" << p.amount
                      << " " << typeStr
                      << " via " << modeStr << "\n";
        }
    }

    std::string Serialize() const override
    {
        return "SaleOrder"
             + std::string("|") + std::to_string(getId())
             + "|" + std::to_string(getCustomerId())
             + "|" + orderDate_.ToDateString()
             + "|" + std::to_string(static_cast<int>(status_))
             + "|" + (checkedOut_ ? "1" : "0")
             + "|" + note_;
    }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (!customer_)
            throw std::runtime_error("SaleOrder: customer is null");
        if (items_.empty())
            throw std::runtime_error("SaleOrder: no line items");
        if (checkedOut_ && getAmountDue() > 0.001)
            throw std::runtime_error("SaleOrder: checked out but amount still due");
    }

    ~SaleOrder() = default;
};