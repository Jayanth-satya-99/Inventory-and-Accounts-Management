#pragma once

#include "BaseEntity.hpp"
#include "LineItem.hpp"
#include "Party.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

// PurchaseOrder — shop buying stock from a Company or Distributor
// Payment_Type: Spot (pay on delivery) or Loan (pay later in installments)
// Locked for item changes once status == Paid
class PurchaseOrder : public BaseEntity
{
private:
    std::vector<LineItem*> items_;
    Party*       supplier_;
    Source_Type  sourceType_;
    DateTime     orderDate_;
    Order_Status status_;
    double       amountPaid_;
    Payment_Type paymentType_;   // Spot or Loan only
    Payment_Mode paymentMode_;   // how the last payment was made
    std::string  note_;

public:
    PurchaseOrder(Party*       supplier,
                  Source_Type  sourceType,
                  DateTime     orderDate,
                  Payment_Type paymentType,
                  const std::string& note = "")
        : supplier_(supplier),
          sourceType_(sourceType),
          orderDate_(orderDate),
          status_(Order_Status::Pending),
          amountPaid_(0.0),
          paymentType_(paymentType),
          paymentMode_(Payment_Mode::Cash),
          note_(note)
    {
        if (!supplier_)
            throw std::invalid_argument("PurchaseOrder: supplier cannot be null");
        if (paymentType_ == Payment_Type::Advance)
            throw std::invalid_argument("PurchaseOrder: Advance not allowed, use Spot or Loan");
    }

    // Restore from storage
    PurchaseOrder(int id, const DateTime& c, const DateTime& u,
                  Party*       supplier,
                  Source_Type  sourceType,
                  DateTime     orderDate,
                  Order_Status status,
                  double       amountPaid,
                  Payment_Type paymentType,
                  Payment_Mode paymentMode,
                  const std::string& note)
        : BaseEntity(id, c, u),
          supplier_(supplier),
          sourceType_(sourceType),
          orderDate_(orderDate),
          status_(status),
          amountPaid_(amountPaid),
          paymentType_(paymentType),
          paymentMode_(paymentMode),
          note_(note)
    {
        if (!supplier_)
            throw std::invalid_argument("PurchaseOrder: supplier cannot be null");
    }

    // ── Line item management ──────────────────────────────────────────────────
    void addLineItem(LineItem* item)
    {
        if (status_ == Order_Status::Paid)
            throw std::runtime_error("PurchaseOrder: cannot add items to a Paid order");
        if (!item)
            throw std::invalid_argument("PurchaseOrder: LineItem cannot be null");
        items_.push_back(item);
    }

    void updateLineItem(int lineItemId, int newQty, double newPrice)
    {
        if (status_ == Order_Status::Paid)
            throw std::runtime_error("PurchaseOrder: cannot modify a Paid order");
        for (LineItem* item : items_) {
            if (item->getId() == lineItemId) {
                if (newQty <= 0)
                    throw std::invalid_argument("PurchaseOrder: quantity must be positive");
                if (newPrice < 0)
                    throw std::invalid_argument("PurchaseOrder: price cannot be negative");
                *item = LineItem(item->getProductVaraint(), newQty, newPrice);
                return;
            }
        }
        throw std::runtime_error("PurchaseOrder: LineItem id not found");
    }

    // ── Financials ────────────────────────────────────────────────────────────
    double getTotalAmount() const
    {
        double total = 0.0;
        for (LineItem* item : items_)
            total += item->getTotal();
        return total;
    }

    double getAmountDue() const
    {
        return getTotalAmount() - amountPaid_;
    }

    void makePayment(double amount, Payment_Mode mode)
    {
        if (amount <= 0)
            throw std::invalid_argument("PurchaseOrder: payment amount must be positive");
        if (amountPaid_ + amount > getTotalAmount())
            throw std::runtime_error("PurchaseOrder: payment exceeds total amount due");
        amountPaid_  += amount;
        paymentMode_  = mode;
        status_ = (amountPaid_ >= getTotalAmount()) ? Order_Status::Paid : Order_Status::Partial;
    }

    // ── Getters ───────────────────────────────────────────────────────────────
    Party*                          getSupplier()    const { return supplier_; }
    int                             getSupplierId()  const { return supplier_->getId(); }
    Source_Type                     getSourceType()  const { return sourceType_; }
    const DateTime&                 getOrderDate()   const { return orderDate_; }
    Order_Status                    getStatus()      const { return status_; }
    double                          getAmountPaid()  const { return amountPaid_; }
    Payment_Type                    getPaymentType() const { return paymentType_; }
    Payment_Mode                    getPaymentMode() const { return paymentMode_; }
    const std::string&              getNote()        const { return note_; }
    const std::vector<LineItem*>&   getItems()       const { return items_; }

    void setNote(const std::string& note) { note_ = note; MarkUpdated(); }

    // ── BaseEntity interface ──────────────────────────────────────────────────
    std::string GetEntityType() const override { return "PurchaseOrder"; }

    void Display() const override
    {
        std::string statusStr, ptStr, pmStr;
        switch(status_) {
            case Order_Status::Pending: statusStr = "Pending"; break;
            case Order_Status::Partial: statusStr = "Partial"; break;
            case Order_Status::Paid:    statusStr = "Paid";    break;
        }
        switch(paymentType_) {
            case Payment_Type::Spot:    ptStr = "Spot";    break;
            case Payment_Type::Loan:    ptStr = "Loan";    break;
            case Payment_Type::Advance: ptStr = "Advance"; break;
        }
        switch(paymentMode_) {
            case Payment_Mode::Cash:          pmStr = "Cash";          break;
            case Payment_Mode::UPI:           pmStr = "UPI";           break;
            case Payment_Mode::Card:          pmStr = "Card";          break;
            case Payment_Mode::Bank_Transfer: pmStr = "Bank Transfer"; break;
            case Payment_Mode::Cheque:        pmStr = "Cheque";        break;
        }

        std::cout << "[PurchaseOrder] id="  << getId()
                  << "  Supplier: "         << supplier_->getName()
                  << "  Date: "             << orderDate_.ToDateString()
                  << "  Status: "           << statusStr
                  << "  Type: "             << ptStr
                  << "  Mode: "             << pmStr
                  << "\n"
                  << "  Total=₹"            << getTotalAmount()
                  << "  Paid=₹"             << amountPaid_
                  << "  Due=₹"              << getAmountDue();
        if (!note_.empty()) std::cout << "  Note: " << note_;
        std::cout << "\n  Items:\n";
        for (LineItem* item : items_)
            item->Display();
    }

    std::string Serialize() const override
    {
        return "PurchaseOrder"
             + std::string("|") + std::to_string(getId())
             + "|" + std::to_string(getSupplierId())
             + "|" + std::to_string(static_cast<int>(sourceType_))
             + "|" + orderDate_.ToDateString()
             + "|" + std::to_string(static_cast<int>(status_))
             + "|" + std::to_string(amountPaid_)
             + "|" + std::to_string(static_cast<int>(paymentType_))
             + "|" + std::to_string(static_cast<int>(paymentMode_))
             + "|" + note_;
    }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (!supplier_)
            throw std::runtime_error("PurchaseOrder: supplier is null");
        if (items_.empty())
            throw std::runtime_error("PurchaseOrder: no line items");
        if (amountPaid_ < 0)
            throw std::runtime_error("PurchaseOrder: amountPaid is negative");
        if (amountPaid_ > getTotalAmount())
            throw std::runtime_error("PurchaseOrder: amountPaid exceeds total");
        if (paymentType_ == Payment_Type::Advance)
            throw std::runtime_error("PurchaseOrder: Advance payment type not allowed");
    }

    // ── Destructor — does NOT own LineItems ───────────────────────────────────
    ~PurchaseOrder() = default;
};