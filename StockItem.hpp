#pragma once

#include "BaseEntity.hpp"
#include "DateTime.hpp"
#include "ProductVariant.hpp"
#include "Structs.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// StockItem — one per ProductVariant
// Owns its own ExpiryBatches (only populated for perishable variants)
class StockItem : public BaseEntity
{
private:
    ProductVariant*          variant_;
    std::vector<ExpiryBatch> batches_;
    int                      quantity_;       // total across all batches
    int                      minStockLevel_;

public:
    StockItem(ProductVariant* variant, int minStockLevel = 0)
        : variant_(variant), quantity_(0), minStockLevel_(minStockLevel)
    {
        if (!variant_) throw std::invalid_argument("StockItem: variant cannot be null");
        if (minStockLevel_ < 0) throw std::invalid_argument("StockItem: minStockLevel cannot be negative");
    }

    // Restore from storage
    StockItem(int id, DateTime& c, DateTime& u,
              ProductVariant* variant,
              std::vector<ExpiryBatch> batches,
              int minStockLevel)
        : BaseEntity(id, c, u),
          variant_(variant), batches_(batches),
          quantity_(0), minStockLevel_(minStockLevel)
    {
        if (!variant_) throw std::invalid_argument("StockItem: variant cannot be null");
        for (const ExpiryBatch& b : batches_)
            quantity_ += b.quantity_;
    }

    ~StockItem() = default;

    // ── Add stock — non-perishable: batchNo empty; perishable: batchNo given ──
    void addStock(int qty, int purchaseOrderId,
                  const std::string& batchNo = "",
                  DateTime expiryDate = DateTime())
    {
        if (qty <= 0)
            throw std::invalid_argument("StockItem: qty to add must be positive");

        quantity_ += qty;

        if (!batchNo.empty()) {
            ExpiryBatch batch;
            batch.batchNo_         = batchNo;
            batch.variant_         = variant_;
            batch.quantity_        = qty;
            batch.expiryDate_      = expiryDate;
            batch.purchaseOrderId_ = purchaseOrderId;
            batches_.push_back(batch);
        }
        MarkUpdated();
    }

    // ── Remove stock ────────────────────────────────────────────────────────
    // batchNo empty → non-perishable, just reduce total quantity_
    // batchNo given → reduce that specific batch (and total quantity_)
    void removeStock(int qty, const std::string& batchNo = "")
    {
        if (qty <= 0)
            throw std::invalid_argument("StockItem: qty to remove must be positive");
        if (qty > quantity_)
            throw std::runtime_error("StockItem: insufficient stock");

        if (batchNo.empty()) {
            quantity_ -= qty;
            return;
        }

        for (ExpiryBatch& b : batches_) {
            if (b.batchNo_ == batchNo) {
                if (qty > b.quantity_)
                    throw std::runtime_error("StockItem: batch '" + batchNo + "' has insufficient quantity");
                b.quantity_ -= qty;
                quantity_   -= qty;
                return;
            }
        }
        throw std::runtime_error("StockItem: batch '" + batchNo + "' not found");
    }

    // ── Lookup ──────────────────────────────────────────────────────────────
    ExpiryBatch* getBatchByNo(const std::string& batchNo)
    {
        for (ExpiryBatch& b : batches_)
            if (b.batchNo_ == batchNo) return &b;
        return nullptr;
    }

    std::vector<ExpiryBatch*> getExpiringBefore(DateTime date)
    {
        std::vector<ExpiryBatch*> list;
        for (ExpiryBatch& b : batches_)
            if (b.expiryDate_.IsSet() && b.expiryDate_ < date)
                list.push_back(&b);
        return list;
    }

    // ── Getters ─────────────────────────────────────────────────────────────
    bool isLowStock()              const { return quantity_ < minStockLevel_; }
    int  getQuantity()             const { return quantity_; }
    int  getMinStockLevel()        const { return minStockLevel_; }
    ProductVariant* getProductVariant() const { return variant_; }
    const std::vector<ExpiryBatch>& getBatches() const { return batches_; }

    void setMinStockLevel(int qty)
    {
        if (qty < 0) throw std::invalid_argument("StockItem: minStockLevel cannot be negative");
        minStockLevel_ = qty;
        MarkUpdated();
    }

    // ── BaseEntity interface ───────────────────────────────────────────────
    std::string GetEntityType() const override { return "StockItem"; }

    void Display() const override
    {
        std::cout << "[StockItem] id=" << getId()
                  << "  " << variant_->getDisplayName()
                  << "  Qty=" << quantity_
                  << "  MinLevel=" << minStockLevel_
                  << (isLowStock() ? "  [LOW STOCK]" : "")
                  << "\n";
        for (const ExpiryBatch& b : batches_) {
            std::cout << "    Batch " << b.batchNo_
                      << "  Qty=" << b.quantity_
                      << "  Expiry=" << b.expiryDate_.ToDateString()
                      << "\n";
        }
    }

    std::string Serialize() const override
    {
        return "StockItem|" + std::to_string(getId())
             + "|" + std::to_string(variant_->getId())
             + "|" + std::to_string(quantity_)
             + "|" + std::to_string(minStockLevel_)
             + "|batches:" + std::to_string(batches_.size());
    }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (!variant_)
            throw std::runtime_error("StockItem: variant is null");
        if (quantity_ < 0)
            throw std::runtime_error("StockItem: quantity is negative");
        int batchTotal = 0;
        for (const ExpiryBatch& b : batches_) batchTotal += b.quantity_;
        if (!batches_.empty() && batchTotal != quantity_)
            throw std::runtime_error("StockItem: batch quantities don't sum to total quantity");
    }
};