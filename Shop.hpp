#pragma once

#include "CategoryManager.hpp"
#include "PartyManager.hpp"
#include "ProductManager.hpp"
#include "StockManager.hpp"
#include "ReportEngine.hpp"
#include "Product.hpp"
#include "ProductVariant.hpp"
#include "LineItem.hpp"
#include "SaleOrder.hpp"
#include "PurchaseOrder.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Shop — Singleton facade
// CLI only talks to Shop. Shop coordinates all managers.
class Shop
{
private:
    CategoryManager& cm_ = CategoryManager::getInstance();
    PartyManager&    pm_ = PartyManager::getInstance();
    ProductManager&  prm_= ProductManager::getInstance();
    StockManager&    sm_ = StockManager::getInstance();
    ReportEngine&    re_ = ReportEngine::getInstance();

    Shop() = default;

public:
    static Shop& getInstance()
    {
        static Shop instance;
        return instance;
    }
    Shop(const Shop&)            = delete;
    Shop& operator=(const Shop&) = delete;

    // ═══════════════════════════════════════════════════════════════
    //  Category Management
    // ═══════════════════════════════════════════════════════════════
    Category* addCategory(const std::string& name, const std::string& desc = "")
    {
        return cm_.addCategory(name, desc);
    }

    SubCategory* addSubCategory(const std::string& name, int categoryId,
                                const std::string& desc = "")
    {
        return cm_.addSubCategory(name, categoryId, desc);
    }

    void displayCategories() const { cm_.displayAll(); }

    // ═══════════════════════════════════════════════════════════════
    //  Party Management
    // ═══════════════════════════════════════════════════════════════
    Company* addCompany(const std::string& name, const std::string& phone,
                        const std::string& email, const std::string& address,
                        bool hasDealership, DateTime dealershipExpiry,
                        const std::string& gst)
    {
        return pm_.addCompany(name, phone, email, address,
                              hasDealership, dealershipExpiry, gst);
    }

    Distributor* addDistributor(const std::string& name, const std::string& phone,
                                const std::string& email, const std::string& address,
                                const std::string& area,
                                std::vector<Category*> cats = {})
    {
        return pm_.addDistributor(name, phone, email, address, area, cats);
    }

    PermanentCustomer* addPermanentCustomer(const std::string& name,
                                            const std::string& phone,
                                            const std::string& email,
                                            const std::string& address,
                                            double creditLimit = 0.0)
    {
        return pm_.addPermanentCustomer(name, phone, email, address, creditLimit);
    }

    TemporaryCustomer* addTemporaryCustomer(const std::string& name,
                                            const std::string& phone,
                                            const std::string& email = "",
                                            const std::string& address = "")
    {
        return pm_.addTemporaryCustomer(name, phone, email, address);
    }

    RetailShop* addRetailShop(const std::string& name, const std::string& phone,
                              const std::string& email, const std::string& address,
                              const std::string& gst = "")
    {
        return pm_.addRetailShop(name, phone, email, address, gst);
    }

    // ── Edit Party ────────────────────────────────────────────────
    void editPartyName(int id, const std::string& name)
    {
        Party* p = pm_.getPartyById(id);
        if (!p) throw std::runtime_error("Shop: party not found");
        p->setName(name);
    }

    void editPartyPhone(int id, const std::string& phone)
    {
        Party* p = pm_.getPartyById(id);
        if (!p) throw std::runtime_error("Shop: party not found");
        p->setPhone(phone);
    }

    void editPartyEmail(int id, const std::string& email)
    {
        Party* p = pm_.getPartyById(id);
        if (!p) throw std::runtime_error("Shop: party not found");
        p->setEmail(email);
    }

    void editPartyAddress(int id, const std::string& address)
    {
        Party* p = pm_.getPartyById(id);
        if (!p) throw std::runtime_error("Shop: party not found");
        p->setAddress(address);
    }

    void editCustomerCreditLimit(int id, double limit)
    {
        PermanentCustomer* c = pm_.getPermanentCustomerById(id);
        if (!c) throw std::runtime_error("Shop: permanent customer not found");
        c->setCreditLimit(limit);
    }

    void editDistributorArea(int id, const std::string& area)
    {
        Distributor* d = pm_.getDistributorById(id);
        if (!d) throw std::runtime_error("Shop: distributor not found");
        d->setAreaServed(area);
    }

    void editCompanyDealership(int id, bool hasDealership,
                               const std::string& phone = "",
                               const std::string& email = "",
                               const std::string& address = "",
                               DateTime expiry = DateTime(0),
                               const std::string& gst = "")
    {
        Company* c = pm_.getCompanyById(id);
        if (!c) throw std::runtime_error("Shop: company not found");
        c->setDealership(hasDealership, phone, email, address, expiry, gst);
    }

    void editVariantMinStock(int variantId, int minLevel)
    {
        StockItem* item = sm_.getStockItemByVariantId(variantId);
        if (!item) throw std::runtime_error("Shop: variant not registered in stock");
        item->setMinStockLevel(minLevel);
    }

    Party* findPartyById(int id)   { return pm_.getPartyById(id); }
    void   displayAllParties()     { pm_.displayAll(); }

    // ── Ledger views ──────────────────────────────────────────────
    void displayCustomerLedger(int partyId)
    {
        Party* p = pm_.getPartyById(partyId);
        if (!p) throw std::runtime_error("Shop: party not found");
        pm_.getOrCreateCustomerLedger(p)->Display();
    }

    void displaySupplierLedger(int partyId)
    {
        Party* p = pm_.getPartyById(partyId);
        if (!p) throw std::runtime_error("Shop: party not found");
        pm_.getOrCreateCompanyLedger(p)->Display();
    }

    // ═══════════════════════════════════════════════════════════════
    //  Product Management
    // ═══════════════════════════════════════════════════════════════
    PerishableProduct* addPerishableProduct(const std::string& name,
                                            Category* cat, SubCategory* sub,
                                            Company* company, Source_Type src,
                                            Distributor* dist = nullptr)
    {
        return prm_.addPerishableProduct(name, cat, sub, company, src, dist);
    }

    NonPerishableProduct* addNonPerishableProduct(const std::string& name,
                                                  Category* cat, SubCategory* sub,
                                                  Company* company, Source_Type src,
                                                  Distributor* dist = nullptr,
                                                  int warrantyDays = 0)
    {
        return prm_.addNonPerishableProduct(name, cat, sub, company, src,
                                            dist, warrantyDays);
    }

    ProductVariant* addProductVariant(Product* product,
                                      const std::string& packSize,
                                      const std::string& unit,
                                      double buyPrice, double sellPrice,
                                      const std::string& barcode = "")
    {
        return prm_.addVariant(product, packSize, unit, buyPrice, sellPrice, barcode);
    }

    void editProductName(int id, const std::string& name)
    {
        prm_.setProductName(id, name);
    }

    void editProductActive(int id, bool active)
    {
        prm_.setProductActive(id, active);
    }

    void editVariantBuyPrice(int id, double price)
    {
        prm_.setVariantBuyPrice(id, price);
    }

    void editVariantSellPrice(int id, double price)
    {
        prm_.setVariantSellPrice(id, price);
    }

    void displayAllProducts()  const { prm_.displayAllProducts(); }
    void displayAllVariants()  const { prm_.displayAllVariants(); }

    // ═══════════════════════════════════════════════════════════════
    //  Stock Management
    // ═══════════════════════════════════════════════════════════════
    StockItem* registerVariantForStock(ProductVariant* variant, int minStockLevel = 0)
    {
        return sm_.registerVariant(variant, minStockLevel);
    }

    void displayStock()    const { sm_.displayAll(); }
    void displayLowStock() const { sm_.displayLowStock(); }

    void checkStockForVariant(int variantId) const
    {
        StockItem* item = sm_.getStockItemByVariantId(variantId);
        if (!item) { std::cout << "Variant not registered in stock.\n"; return; }
        item->Display();
    }

    void displayExpiringBefore(DateTime d) const
    {
        auto batches = sm_.getExpiringBefore(d);
        std::cout << "\n===== Expiring Before " << d.ToDateString() << " =====\n";
        for (ExpiryBatch* b : batches)
            std::cout << "  Batch:" << b->batchNo_
                      << "  " << b->variant_->getDisplayName()
                      << "  Qty:" << b->quantity_
                      << "  Expiry:" << b->expiryDate_.ToDateString() << "\n";
        if (batches.empty()) std::cout << "  None.\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  Sale Flow
    // ═══════════════════════════════════════════════════════════════
    SaleOrder* createSaleOrder(Party* customer, const std::string& note = "")
    {
        if (!customer) throw std::invalid_argument("Shop: customer cannot be null");
        return new SaleOrder(customer, DateTime::Now(), note);
    }

    void addLineItemToSale(SaleOrder* order, ProductVariant* variant,
                           int qty, double price,
                           const std::string& batchNo = "")
    {
        StockItem* item = sm_.getStockItemByVariantId(variant->getId());
        if (!item)
            throw std::runtime_error("Shop: variant not registered in stock");
        if (item->getQuantity() < qty)
            throw std::runtime_error("Shop: insufficient stock for "
                                     + variant->getDisplayName());
        if (!batchNo.empty()) {
            ExpiryBatch* batch = item->getBatchByNo(batchNo);
            if (!batch)
                throw std::runtime_error("Shop: batch '" + batchNo + "' not found");
            if (batch->quantity_ < qty)
                throw std::runtime_error("Shop: batch has insufficient quantity");
        }
        order->addLineItem(new LineItem(variant, qty, price, false, batchNo));
    }

    void addPaymentToSale(SaleOrder* order, double amount,
                          Payment_Mode mode, Payment_Type type)
    {
        order->addPayment(amount, mode, type);
    }

    void confirmSaleOrder(SaleOrder* order)
    {
        if (!order) throw std::invalid_argument("Shop: order cannot be null");
        order->confirm();
        pm_.registerSaleOrder(order);
        Party* customer = order->getCustomer();
        pm_.getOrCreateCustomerLedger(customer)->addOrder(order);
        for (LineItem* item : order->getItems())
            if (!item->isReturned())
                sm_.removeStock(item->getProductVaraint()->getId(),
                                item->getQuantity(), item->getBatchNo());
    }

    void returnSaleItem(SaleOrder* order, int lineItemId)
    {
        LineItem* target = nullptr;
        for (LineItem* item : order->getItems())
            if (item->getId() == lineItemId) { target = item; break; }
        if (!target) throw std::runtime_error("Shop: LineItem not found");

        order->returnItem(lineItemId);
        sm_.addStock(target->getProductVaraint()->getId(),
                     target->getQuantity(), order->getId(),
                     target->getBatchNo());
        Party* customer = order->getCustomer();
        pm_.getOrCreateCustomerLedger(customer)->addEntry(
            {DateTime::Now(), Ledger_Entry_Type::Return,
             target->getTotal(), Payment_Mode::Cash, order->getId()});
    }

    // View past sale orders for a customer
    std::vector<SaleOrder*> getSaleOrdersByCustomer(int partyId) const
    {
        std::vector<SaleOrder*> result;
        for (SaleOrder* o : pm_.getAllSaleOrders())
            if (o->getCustomerId() == partyId) result.push_back(o);
        return result;
    }

    void displayAllSaleOrders() const
    {
        std::cout << "\n===== All Sale Orders =====\n";
        for (SaleOrder* o : pm_.getAllSaleOrders()) o->Display();
        if (pm_.getAllSaleOrders().empty()) std::cout << "  None.\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  Purchase Flow
    // ═══════════════════════════════════════════════════════════════
    PurchaseOrder* createPurchaseOrder(Party* supplier, Source_Type src,
                                       Payment_Type pt,
                                       const std::string& note = "")
    {
        if (!supplier) throw std::invalid_argument("Shop: supplier cannot be null");
        return new PurchaseOrder(supplier, src, DateTime::Now(), pt, note);
    }

    void addLineItemToPurchase(PurchaseOrder* order, ProductVariant* variant,
                               int qty, double price)
    {
        // For purchase orders batch is assigned at confirm time
        order->addLineItem(new LineItem(variant, qty, price));
    }

    void makePaymentOnPurchase(PurchaseOrder* order, double amount, Payment_Mode mode)
    {
        order->makePayment(amount, mode);
        // Record payment in ledger
        CompanyLedgerPage* ledger = pm_.getOrCreateCompanyLedger(order->getSupplier());
        ledger->addPayment(amount, mode, order->getId());
    }

    void confirmPurchaseOrder(PurchaseOrder* order,
                              const std::string& batchNo = "",
                              DateTime expiryDate = DateTime())
    {
        if (!order) throw std::invalid_argument("Shop: order cannot be null");
        pm_.registerPurchaseOrder(order);
        pm_.getOrCreateCompanyLedger(order->getSupplier())->addOrder(order);
        for (LineItem* item : order->getItems())
            sm_.addStock(item->getProductVaraint()->getId(),
                         item->getQuantity(), order->getId(),
                         batchNo, expiryDate);
    }

    // Pay installment on existing loan purchase order
    void payLoanInstallment(int purchaseOrderId, double amount, Payment_Mode mode)
    {
        PurchaseOrder* order = nullptr;
        for (PurchaseOrder* o : pm_.getAllPurchaseOrders())
            if (o->getId() == purchaseOrderId) { order = o; break; }
        if (!order)
            throw std::runtime_error("Shop: purchase order not found");
        makePaymentOnPurchase(order, amount, mode);
    }

    // View past purchase orders for a supplier
    std::vector<PurchaseOrder*> getPurchaseOrdersBySupplier(int partyId) const
    {
        std::vector<PurchaseOrder*> result;
        for (PurchaseOrder* o : pm_.getAllPurchaseOrders())
            if (o->getSupplierId() == partyId) result.push_back(o);
        return result;
    }

    void displayAllPurchaseOrders() const
    {
        std::cout << "\n===== All Purchase Orders =====\n";
        for (PurchaseOrder* o : pm_.getAllPurchaseOrders()) o->Display();
        if (pm_.getAllPurchaseOrders().empty()) std::cout << "  None.\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  Customer / Supplier Deposits
    // ═══════════════════════════════════════════════════════════════
    void customerDeposit(Party* customer, double amount, Payment_Mode mode)
    {
        pm_.getOrCreateCustomerLedger(customer)->addDeposit(amount, mode);
    }

    void supplierAdvanceDeposit(int partyId, double amount, Payment_Mode mode)
    {
        Party* p = pm_.getPartyById(partyId);
        if (!p) throw std::runtime_error("Shop: party not found");
        pm_.getOrCreateCompanyLedger(p)->addAdvance(amount, mode);
    }

    // ═══════════════════════════════════════════════════════════════
    //  Manager accessors (for CLI direct queries)
    // ═══════════════════════════════════════════════════════════════
    CategoryManager& categories() { return cm_; }
    PartyManager&    parties()    { return pm_; }
    ProductManager&  products()   { return prm_; }
    StockManager&    stock()      { return sm_; }
    ReportEngine&    reports()    { return re_; }
};