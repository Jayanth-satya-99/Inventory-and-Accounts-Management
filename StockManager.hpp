#pragma once

#include "BaseEntity.hpp"
#include "DateTime.hpp"
#include "ProductVariant.hpp"
#include "StockItem.hpp"
#include "Product.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class StockManager
{
    std::vector<StockItem *> stock_;
    StockManager() = default;

public:
    static StockManager &getInstance()
    {
        static StockManager instance;
        return instance;
    }
    StockManager(const StockManager &) = delete;
    StockManager &operator=(const StockManager &) = delete;
    StockItem *registerVariant(ProductVariant *variant, int minStockLevel = 0)
    {
        if (getStockItemByVariantId(variant->getId()))
            throw std::runtime_error("StockManager: variant already registered");
        stock_.push_back(new StockItem(variant, minStockLevel));
        return stock_.back();
    }
    StockItem *getStockItemByVariantId(int variantId) const
    {
        for (StockItem *item : stock_)
            if (item->getProductVariant()->getId() == variantId)
                return item;
        return nullptr;
    }
    StockItem *getStockItemById(int id) const
    {
        for (StockItem *item : stock_)
            if (item->getId() == id)
                return item;
        return nullptr;
    }
    std::vector<StockItem *> getStockItemsByProduct(Product *product) const
    {
        std::vector<StockItem *> list;
        for (StockItem *item : stock_)
            if (item->getProductVariant()->getProduct() == product)
                list.push_back(item);
        return list;
    }
    std::vector<StockItem *> getStockItemsByProductId(int productId)
    {
        std::vector<StockItem *> list;
        for (StockItem *item : stock_)
            if (item->getProductVariant()->getProduct()->getId() == productId)
                list.push_back(item);
        return list;
    }
    void addStock(int variantId, int qty, int purchaseOrderId,
                  const std::string &batchNo = "", DateTime expiryDate = DateTime())
    {
        StockItem *item = getStockItemByVariantId(variantId);
        if (!item)
            throw std::runtime_error("StockManager: no StockItem registered for this variant");
        item->addStock(qty, purchaseOrderId, batchNo, expiryDate);
    }
    void removeStock(int variantId, int qty, const std::string &batchNo = "")
    {
        StockItem *item = getStockItemByVariantId(variantId);
        if (!item)
            throw std::runtime_error("StockManager: no StockItem registered for this variant");
        item->removeStock(qty, batchNo);
    }
    std::vector<StockItem *> getLowStockItems()const
    {
        std::vector<StockItem *> list;
        for (int i = 0; i < stock_.size(); i++)
        {
            if (stock_[i]->isLowStock())
                list.push_back(stock_[i]);
        }
        return list;
    }
    std::vector<ExpiryBatch *> getExpiringBefore(DateTime date) const
    {
        std::vector<ExpiryBatch *> list;
        for (StockItem *item : stock_)
        {
            auto l = item->getExpiringBefore(date);
            list.insert(list.end(), l.begin(), l.end());
        }
        return list;
    }
    std::vector<std::vector<ExpiryBatch *>> getExpiringBefore(DateTime date, int c = 0)
    {
        std::vector<std::vector<ExpiryBatch *>> list;
        for (StockItem *item : stock_)
        {
            list.push_back(item->getExpiringBefore(date));
        }
        return list;
    }

     void displayAll() const
    {
        std::cout << "\n===================== Stock =====================\n";
        for (StockItem *item : stock_)
            item->Display();
        std::cout << "==================================================\n";
    }

    void displayLowStock() const
    {
        std::cout << "\n================ Low Stock Alert ================\n";
        for (StockItem *item : getLowStockItems())
            item->Display();
        std::cout << "==================================================\n";
    }

    // ── Destructor ──────────────────────────────────────────────────────────
    ~StockManager()
    {
        for (StockItem *item : stock_)
            delete item;
    }

   
    std::string GetEntityType() const { return "Stock Manager"; }
};