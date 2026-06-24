#pragma once

#include "Company.hpp"
#include "Distributor.hpp"
#include "PermanentCustomer.hpp"
#include "TemporaryCustomer.hpp"
#include "RetailShop.hpp"
#include "Category.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>

// PartyManager — Singleton
// Owns all party objects (Company, Distributor, Customer subtypes, RetailShop)
// Provides add / get / remove and cross-object queries
//
// Usage:
//   PartyManager& pm = PartyManager::getInstance();
//   Company* c = pm.addCompany("Syngenta", "", "", "", false, DateTime(0), "");
//   Distributor* d = pm.addDistributor("Ram Traders", "9876543210", "", "Odisha", "Odisha");
class PartyManager
{
private:
    // ── Storage ───────────────────────────────────────────────────────────────
    std::vector<Company*>           companies_;
    std::vector<Distributor*>       distributors_;
    std::vector<PermanentCustomer*> permCustomers_;
    std::vector<TemporaryCustomer*> tempCustomers_;
    std::vector<RetailShop*>        retailShops_;

    // ── Singleton ─────────────────────────────────────────────────────────────
    PartyManager() = default;

public:
    static PartyManager& getInstance()
    {
        static PartyManager instance;
        return instance;
    }
    PartyManager(const PartyManager&)            = delete;
    PartyManager& operator=(const PartyManager&) = delete;

    // ═══════════════════════════════════════════════════════════════
    //  Company
    // ═══════════════════════════════════════════════════════════════
    Company* addCompany(const std::string& name,
                        const std::string& phone,
                        const std::string& email,
                        const std::string& address,
                        bool hasDealership,
                        DateTime dealershipExpiry,
                        const std::string& gstNumber)
    {
        companies_.push_back(
            new Company(name, phone, email, address,
                        hasDealership, dealershipExpiry, gstNumber));
        return companies_.back();
    }

    Company* getCompanyById(int id) const
    {
        for (Company* c : companies_)
            if (c->getId() == id) return c;
        return nullptr;
    }

    Company* getCompanyByName(const std::string& name) const
    {
        for (Company* c : companies_)
            if (c->getName() == name) return c;
        return nullptr;
    }

    const std::vector<Company*>& getAllCompanies() const { return companies_; }

    bool removeCompany(int id)
    {
        for (auto it = companies_.begin(); it != companies_.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                companies_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ═══════════════════════════════════════════════════════════════
    //  Distributor
    // ═══════════════════════════════════════════════════════════════
    Distributor* addDistributor(const std::string& name,
                                const std::string& phone,
                                const std::string& email,
                                const std::string& address,
                                const std::string& areaServed,
                                std::vector<Category*> categories = {})
    {
        distributors_.push_back(
            new Distributor(name, phone, email, address, areaServed, categories));
        return distributors_.back();
    }

    Distributor* getDistributorById(int id) const
    {
        for (Distributor* d : distributors_)
            if (d->getId() == id) return d;
        return nullptr;
    }
     std::vector<Distributor*> getDistributorsByName(const std::string& name)const{
        std::vector<Distributor*>result;
        for(Distributor* d:distributors_){
            if(d->getName()==name)result.push_back(d);
        }
        return result;
    }

    const std::vector<Distributor*>& getAllDistributors() const { return distributors_; }

    // Cross-object query: all distributors that supply a given category
    std::vector<Distributor*> getDistributorsForCategory(int categoryId) const
    {
        std::vector<Distributor*> result;
        for (Distributor* d : distributors_) {
            for (Category* cat : d->getSupportedCategory()) {
                if (cat->getId() == categoryId) {
                    result.push_back(d);
                    break;
                }
            }
        }
        return result;
    }

    bool removeDistributor(int id)
    {
        for (auto it = distributors_.begin(); it != distributors_.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                distributors_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ═══════════════════════════════════════════════════════════════
    //  PermanentCustomer
    // ═══════════════════════════════════════════════════════════════
    PermanentCustomer* addPermanentCustomer(const std::string& name,
                                            const std::string& phone,
                                            const std::string& email,
                                            const std::string& address,
                                            double creditLimit = 0.0)
    {
        permCustomers_.push_back(
            new PermanentCustomer(name, phone, email, address, creditLimit));
        return permCustomers_.back();
    }

    PermanentCustomer* getPermanentCustomerById(int id) const
    {
        for (PermanentCustomer* c : permCustomers_)
            if (c->getId() == id) return c;
        return nullptr;
    }
     std::vector<PermanentCustomer*> getPermanentCustomersByName(const std::string& name)const{
        std::vector<PermanentCustomer*>result;
        for(PermanentCustomer* c:permCustomers_){
            if(c->getName()==name)result.push_back(c);
        }
        return result;
    }

    const std::vector<PermanentCustomer*>& getAllPermanentCustomers() const
    { return permCustomers_; }

    bool removePermanentCustomer(int id)
    {
        for (auto it = permCustomers_.begin(); it != permCustomers_.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                permCustomers_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ═══════════════════════════════════════════════════════════════
    //  TemporaryCustomer
    // ═══════════════════════════════════════════════════════════════
    TemporaryCustomer* addTemporaryCustomer(const std::string& name,
                                            const std::string& phone,
                                            const std::string& email  = "",
                                            const std::string& address = "")
    {
        tempCustomers_.push_back(
            new TemporaryCustomer(name, phone, email, address));
        return tempCustomers_.back();
    }

    TemporaryCustomer* getTemporaryCustomerById(int id) const
    {
        for (TemporaryCustomer* c : tempCustomers_)
            if (c->getId() == id) return c;
        return nullptr;
    }
     std::vector<TemporaryCustomer*> getTemporaryCustomersByName(const std::string& name)const{
        std::vector<TemporaryCustomer*>result;
        for(TemporaryCustomer* c:tempCustomers_){
            if(c->getName()==name)result.push_back(c);
        }
        return result;
    }


    const std::vector<TemporaryCustomer*>& getAllTemporaryCustomers() const
    { return tempCustomers_; }

    bool removeTemporaryCustomer(int id)
    {
        for (auto it = tempCustomers_.begin(); it != tempCustomers_.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                tempCustomers_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ═══════════════════════════════════════════════════════════════
    //  RetailShop
    // ═══════════════════════════════════════════════════════════════
    RetailShop* addRetailShop(const std::string& name,
                              const std::string& phone,
                              const std::string& email,
                              const std::string& address,
                              //const std::string& shopType  = "",
                              const std::string& gstNumber = "")
    {
        retailShops_.push_back(
            new RetailShop(name, phone, email, address,  gstNumber));//shopType,
        return retailShops_.back();
    }

    RetailShop* getRetailShopById(int id) const
    {
        for (RetailShop* r : retailShops_)
            if (r->getId() == id) return r;
        return nullptr;
    }
     std::vector<RetailShop*> getRetailShopsByName(const std::string& name)const{
        std::vector<RetailShop*>result;
        for(RetailShop* r : retailShops_){
            if(r->getName()==name)result.push_back(r);
        }
        return result;
    }
    

    const std::vector<RetailShop*>& getAllRetailShops() const { return retailShops_; }

    bool removeRetailShop(int id)
    {
        for (auto it = retailShops_.begin(); it != retailShops_.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                retailShops_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ═══════════════════════════════════════════════════════════════
    //  Unified lookup (search any party by id across all types)
    // ═══════════════════════════════════════════════════════════════
    Party* getPartyById(int id) const
    {
        if (auto* p = getCompanyById(id))           return p;
        if (auto* p = getDistributorById(id))       return p;
        if (auto* p = getPermanentCustomerById(id)) return p;
        if (auto* p = getTemporaryCustomerById(id)) return p;
        if (auto* p = getRetailShopById(id))        return p;
        return nullptr;
    }

    // ═══════════════════════════════════════════════════════════════
    //  Display helpers
    // ═══════════════════════════════════════════════════════════════
    void displayAll() const
    {
        auto section = [](const std::string& title) {
            std::cout << "\n===== " << title << " =====\n";
        };

        section("Companies (" + std::to_string(companies_.size()) + ")");
        for (auto* c : companies_) c->Display();

        section("Distributors (" + std::to_string(distributors_.size()) + ")");
        for (auto* d : distributors_) d->Display();

        section("Permanent Customers (" + std::to_string(permCustomers_.size()) + ")");
        for (auto* c : permCustomers_) c->Display();

        section("Temporary Customers (" + std::to_string(tempCustomers_.size()) + ")");
        for (auto* c : tempCustomers_) c->Display();

        section("Retail Shops (" + std::to_string(retailShops_.size()) + ")");
        for (auto* r : retailShops_) r->Display();

        std::cout << "============================================\n";
    }

    int totalPartyCount() const
    {
        return (int)(companies_.size() + distributors_.size() +
                     permCustomers_.size() + tempCustomers_.size() +
                     retailShops_.size());
    }

    // ── Destructor ────────────────────────────────────────────────────────────
    ~PartyManager()
    {
        for (auto* p : companies_)    delete p;
        for (auto* p : distributors_) delete p;
        for (auto* p : permCustomers_)delete p;
        for (auto* p : tempCustomers_)delete p;
        for (auto* p : retailShops_)  delete p;
    }
};