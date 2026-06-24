#pragma once

#include "BaseEntity.hpp"
#include "Product.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <iomanip>

// ---------------------------------------------------------------------------
//  ProductVariant  –  a specific pack size / unit of a Product
//
//  This is what the shop actually buys, stocks, and sells.
//
//  Examples for product "Urea":
//    Variant 1:  packSize="1 kg",  buyPrice=110,  sellPrice=120,  barcode="892341"
//    Variant 2:  packSize="5 kg",  buyPrice=500,  sellPrice=550,  barcode="892342"
//    Variant 3:  packSize="50 kg", buyPrice=4400, sellPrice=4800, barcode=""  (no barcode)
//
//  Unit examples: kg, g, ml, L, packet, bottle, bag, piece
// ---------------------------------------------------------------------------
class ProductVariant : public BaseEntity {
public:

    ProductVariant(Product*           product,
                   const std::string& packSize,
                   const std::string& unit,
                   double             buyPrice,
                   double             sellPrice,
                   const std::string& barcode = "")   // optional
        : product_(product),
          packSize_(packSize),
          unit_(unit),
          buyPrice_(buyPrice),
          sellPrice_(sellPrice),
          barcode_(barcode),
          isActive_(true)
    {
        if (!product_)         throw std::invalid_argument("ProductVariant: product cannot be null");
        if (packSize_.empty()) throw std::invalid_argument("ProductVariant: packSize cannot be empty");
        if (buyPrice_  < 0)    throw std::invalid_argument("ProductVariant: buyPrice cannot be negative");
        if (sellPrice_ < 0)    throw std::invalid_argument("ProductVariant: sellPrice cannot be negative");
    }

    // Restore from storage
    ProductVariant(int id, const DateTime& c, const DateTime& u,
                   Product* product,
                   const std::string& packSize, const std::string& unit,
                   double buyPrice, double sellPrice,
                   const std::string& barcode, bool isActive)
        : BaseEntity(id, c, u),
          product_(product), packSize_(packSize), unit_(unit),
          buyPrice_(buyPrice), sellPrice_(sellPrice),
          barcode_(barcode), isActive_(isActive)
    {}

    //  Getters 
    Product*           getProduct()    const { return product_; }
    const std::string& getPackSize()   const { return packSize_; }
    const std::string& getUnit()       const { return unit_; }
    double             getBuyPrice()   const { return buyPrice_; }
    double             getSellPrice()  const { return sellPrice_; }
    const std::string& getBarcode()    const { return barcode_; }
    bool               hasBarcode()    const { return !barcode_.empty(); }
    bool               isActive()      const { return isActive_; }

    // Margin in percentage
    double getMarginPct() const {
        if (buyPrice_ == 0) return 0;
        return ((sellPrice_ - buyPrice_) / buyPrice_) * 100.0;
    }

    // Full display name: "Urea — 1 kg"
    std::string getDisplayName() const {
        return product_->getName() + " — " + packSize_;
    }

    // ── Setters ──────────────────────────────────────────────────────────────
    void setBuyPrice(double p) {
        if (p < 0) throw std::invalid_argument("buyPrice cannot be negative");
        buyPrice_ = p; MarkUpdated();
    }
    void setSellPrice(double p) {
        if (p < 0) throw std::invalid_argument("sellPrice cannot be negative");
        sellPrice_ = p; MarkUpdated();
    }
    void setBarcode(const std::string& b) { barcode_ = b; MarkUpdated(); }
    void setPackSize(const std::string& s) {
        if (s.empty()) throw std::invalid_argument("packSize cannot be empty");
        packSize_ = s; MarkUpdated();
    }
    void setActive(bool a) { isActive_ = a; MarkUpdated(); }

    // ── BaseEntity interface ─────────────────────────────────────────────────
    void Display() const override {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "[Variant] id=" << getId()
                  << "  " << getDisplayName()
                  << "  buy=₹"  << buyPrice_
                  << "  sell=₹" << sellPrice_
                  << "  margin=" << getMarginPct() << "%";
        if (hasBarcode()) std::cout << "  barcode=" << barcode_;
        else              std::cout << "  (no barcode)";
        if (!isActive_)   std::cout << "  [INACTIVE]";
        std::cout << "\n";
    }

    std::string Serialize() const override {
        return "ProductVariant|" + std::to_string(getId())
             + "|" + std::to_string(product_->getId())
             + "|" + packSize_
             + "|" + unit_
             + "|" + std::to_string(buyPrice_)
             + "|" + std::to_string(sellPrice_)
             + "|" + barcode_
             + "|" + (isActive_ ? "1" : "0");
    }

    std::string GetEntityType() const override { return "ProductVariant"; }

    void Validate() const override {
        BaseEntity::Validate();
        if (!product_)         throw std::runtime_error("ProductVariant: product is null");
        if (packSize_.empty()) throw std::runtime_error("ProductVariant: packSize is empty");
        if (sellPrice_ < buyPrice_)
            std::cout << "[WARN] ProductVariant id=" << getId()
                      << " is selling below buy price!\n";
    }

private:
    Product*    product_;
    std::string packSize_;    // e.g. "1 kg", "500 ml", "50 kg"
    std::string unit_;        // e.g. "kg", "ml", "L", "packet"
    double      buyPrice_;
    double      sellPrice_;
    std::string barcode_;     // empty string = no barcode
    bool        isActive_;
};