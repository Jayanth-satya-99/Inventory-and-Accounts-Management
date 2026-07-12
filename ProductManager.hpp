#pragma once

#include "Product.hpp"
#include "ProductVariant.hpp"
#include "Category.hpp"
#include "SubCategory.hpp"
#include "Company.hpp"
#include "Distributor.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

// ProductManager — Singleton
// Owns all Product and ProductVariant objects
class ProductManager
{
private:
    std::vector<Product*>        products_;
    std::vector<ProductVariant*> variants_;

    ProductManager() = default;

public:
    static ProductManager& getInstance()
    {
        static ProductManager instance;
        return instance;
    }
    ProductManager(const ProductManager&)            = delete;
    ProductManager& operator=(const ProductManager&) = delete;

    // ═══════════════════════════════════════════════════════════════
    //  Products
    // ═══════════════════════════════════════════════════════════════
    PerishableProduct* addPerishableProduct(
        const std::string& name,
        Category* cat, SubCategory* sub,
        Company* company, Source_Type src,
        Distributor* dist = nullptr)
    {
        auto* p = new PerishableProduct(name, cat, sub, company, src, dist);
        products_.push_back(p);
        return p;
    }

    NonPerishableProduct* addNonPerishableProduct(
        const std::string& name,
        Category* cat, SubCategory* sub,
        Company* company, Source_Type src,
        Distributor* dist = nullptr,
        int warrantyDays = 0)
    {
        auto* p = new NonPerishableProduct(name, cat, sub, company, src, dist, warrantyDays);
        products_.push_back(p);
        return p;
    }

    Product* getProductById(int id) const
    {
        for (Product* p : products_)
            if (p->getId() == id) return p;
        return nullptr;
    }

    std::vector<Product*> getProductsByName(const std::string& name) const
    {
        std::vector<Product*> result;
        for (Product* p : products_)
            if (p->getName() == name) result.push_back(p);
        return result;
    }

    std::vector<Product*> getProductsByCategory(int categoryId) const
    {
        std::vector<Product*> result;
        for (Product* p : products_)
            if (p->getCategory()->getId() == categoryId) result.push_back(p);
        return result;
    }

    const std::vector<Product*>& getAllProducts() const { return products_; }

    void setProductActive(int id, bool active)
    {
        Product* p = getProductById(id);
        if (!p) throw std::runtime_error("ProductManager: product not found");
        p->setActive(active);
    }

    void setProductName(int id, const std::string& name)
    {
        Product* p = getProductById(id);
        if (!p) throw std::runtime_error("ProductManager: product not found");
        p->setName(name);
    }

    // ═══════════════════════════════════════════════════════════════
    //  ProductVariants
    // ═══════════════════════════════════════════════════════════════
    ProductVariant* addVariant(
        Product* product,
        const std::string& packSize,
        const std::string& unit,
        double buyPrice,
        double sellPrice,
        const std::string& barcode = "")
    {
        if (!product)
            throw std::invalid_argument("ProductManager: product cannot be null");
        auto* v = new ProductVariant(product, packSize, unit, buyPrice, sellPrice, barcode);
        variants_.push_back(v);
        return v;
    }

    ProductVariant* getVariantById(int id) const
    {
        for (ProductVariant* v : variants_)
            if (v->getId() == id) return v;
        return nullptr;
    }

    std::vector<ProductVariant*> getVariantsByProduct(int productId) const
    {
        std::vector<ProductVariant*> result;
        for (ProductVariant* v : variants_)
            if (v->getProduct()->getId() == productId) result.push_back(v);
        return result;
    }

    const std::vector<ProductVariant*>& getAllVariants() const { return variants_; }

    void setVariantBuyPrice(int id, double price)
    {
        ProductVariant* v = getVariantById(id);
        if (!v) throw std::runtime_error("ProductManager: variant not found");
        v->setBuyPrice(price);
    }

    void setVariantSellPrice(int id, double price)
    {
        ProductVariant* v = getVariantById(id);
        if (!v) throw std::runtime_error("ProductManager: variant not found");
        v->setSellPrice(price);
    }

    void setVariantActive(int id, bool active)
    {
        ProductVariant* v = getVariantById(id);
        if (!v) throw std::runtime_error("ProductManager: variant not found");
        v->setActive(active);
    }

    // ═══════════════════════════════════════════════════════════════
    //  Display
    // ═══════════════════════════════════════════════════════════════
    void displayAllProducts() const
    {
        std::cout << "\n===== Products (" << products_.size() << ") =====\n";
        for (Product* p : products_) {
            p->Display();
            auto variants = getVariantsByProduct(p->getId());
            for (ProductVariant* v : variants) {
                std::cout << "    ";
                v->Display();
            }
        }
        std::cout << "==============================\n";
    }

    void displayAllVariants() const
    {
        std::cout << "\n===== All Variants (" << variants_.size() << ") =====\n";
        for (ProductVariant* v : variants_) v->Display();
        std::cout << "==============================\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  Destructor
    // ═══════════════════════════════════════════════════════════════
    ~ProductManager()
    {
        for (Product* p : products_)        delete p;
        for (ProductVariant* v : variants_) delete v;
    }
};