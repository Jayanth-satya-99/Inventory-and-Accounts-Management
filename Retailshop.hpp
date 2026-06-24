#pragma once

#include "Party.hpp"
#include "Enums.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

// RetailShop — a shop that buys from us in bulk (B2B customer)
// Always requires phone + address (like a distributor)
// Can have GST number (optional — some small shops may not)
// Payment can be spot or advance; loan handled separately via ledger
class RetailShop : public Party
{
private:
    std::string gstNumber_;   // optional — empty if not registered
    //std::string shopType_;    // e.g. "General Store", "Agro Center"

public:
    RetailShop(const std::string& name,
               const std::string& phone,
               const std::string& email,
               const std::string& address,
               //const std::string& shopType   = "",
               const std::string& gstNumber  = "")
        : Party(name, phone, email, address),
          gstNumber_(gstNumber)
          //shopType_(shopType)
    {
        validateMembershipFields(phone, address, "RetailShop");
        if (!gstNumber_.empty() && gstNumber_.size() != 15)
            throw std::invalid_argument("RetailShop: GST number must be 15 characters");
    }

    // Restore from storage
    RetailShop(int id, DateTime& c, DateTime& u,
               const std::string& name,
               const std::string& phone,
               const std::string& email,
               const std::string& address,
               //const std::string& shopType,
               const std::string& gstNumber)
        : Party(id, c, u, name, phone, email, address),
          gstNumber_(gstNumber)
          //shopType_(shopType)
    {
        if (!gstNumber_.empty() && gstNumber_.size() != 15)
            throw std::invalid_argument("RetailShop: GST number must be 15 characters");
    }

    // ── Getters ──────────────────────────────────────────────────────────────
    const std::string& getGstNumber() const { return gstNumber_; }
   // const std::string& getShopType()  const { return shopType_; }
    bool hasGst() const { return !gstNumber_.empty(); }

    // ── Setters ───────────────────────────────────────────────────────────────
    void setGstNumber(const std::string& gst)
    {
        if (!gst.empty() && gst.size() != 15)
            throw std::invalid_argument("RetailShop: GST number must be 15 characters");
        gstNumber_ = gst;
        MarkUpdated();
    }
    // void setShopType(const std::string& type)
    // {
    //     shopType_ = type;
    //     MarkUpdated();
    // }

    // ── Core virtual overrides ────────────────────────────────────────────────
    Party_Type  getPartyType()  const override { return Party_Type::RetailShop; }
    std::string GetEntityType() const override { return "RetailShop"; }

    // ── Display / Serialize / Validate ────────────────────────────────────────
    void Display() const override
    {
        Party::Display();
        std::cout << "  [RetailShop]";
       // if (!shopType_.empty())  std::cout << "  Type=" << shopType_;
        if (hasGst())            std::cout << "  GST=" << gstNumber_;
        else                     std::cout << "  (no GST)";
        std::cout << "\n";
    }

    std::string Serialize() const override
    {
        return Party::Serialize()
             + "|RetailShop"
           //  + "|" + shopType_
             + "|" + gstNumber_;
    }

    void Validate() const override
    {
        Party::Validate();
        if (phone_.empty())
            throw std::runtime_error("RetailShop: phone cannot be empty");
        if (address_.empty())
            throw std::runtime_error("RetailShop: address cannot be empty");
        if (!gstNumber_.empty() && gstNumber_.size() != 15)
            throw std::runtime_error("RetailShop: GST number must be 15 characters");
    }
};