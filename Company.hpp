#pragma once

#include "BaseEntity.hpp"
#include "Party.hpp"
#include "Enums.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
class Company : public Party
{
    bool hasDealership_;
    DateTime dealershipExpiry_;
    std::string gstNumber_;
    

public:
    Company(const std::string name,
            const std::string phone,
            const std::string email,
            const std::string address,
            bool hasDealership, DateTime dealershipExpiry, std::string gstNumber)
        : Party(name, phone, email, address), hasDealership_(hasDealership), dealershipExpiry_(dealershipExpiry), gstNumber_(gstNumber)
    {
        if (hasDealership_ && gstNumber_.size() != 15)
            throw std::invalid_argument("Company: gst number size not matching");
        if (hasDealership_)
            validateMembershipFields(phone, address,"Company");
    }
    Company(int id, DateTime &c, DateTime &u,
            const std::string name,
            const std::string phone,
            const std::string email,
            const std::string address,
            bool hasDealership, DateTime dealershipExpiry, std::string gstNumber)
        : Party(id, c, u, name, phone, email, address), hasDealership_(hasDealership), dealershipExpiry_(dealershipExpiry), gstNumber_(gstNumber)
    {
        if (hasDealership_ && gstNumber_.size() != 15)
            throw std::invalid_argument("Company: gst number size not matching");
        if (hasDealership_)
            validateMembershipFields(phone, address,"Company");
    }
    // virtual ~Company()=default;
    // getters
    std::string GetEntityType() const override { return "Company"; }
    bool isActiveDealership() const { return hasDealership_ && dealershipExpiry_ > DateTime::Now(); }
    bool hasDealership() const { return hasDealership_; }
    Party_Type getPartyType() const override { return Party_Type::Company; }
    const std::string &getGstNumber() const { return gstNumber_; }
    const DateTime getDealershipExpiry() const { return hasDealership_ ? dealershipExpiry_ : DateTime(0); } // if we dont have dealership have expiry date is unnecsaary. i guess using get dealership is not used at first,intially check for dealer ship then expiry
    // setter
    void setDealership(bool c, std::string phone = "", std::string email = "", std::string address = "", DateTime dealershipexpiry = DateTime(0), std::string gst = "")
    {
        if (c)
        {
            validateMembershipFields(phone, address,"Company");
            if (dealershipexpiry == DateTime(0))
                throw std::invalid_argument("company:invlaid expiry date");
            if (gst.empty() || gst.size() != 15)
                throw std::invalid_argument("company: invalid gst number");
        }
        hasDealership_ = c;
        phone_ = phone;
        email_ = email;
        address_ = address;
        dealershipExpiry_ = dealershipexpiry;
        gstNumber_ = gst;
        MarkUpdated();
    }
    // void setName(const std::string name)
    // {
    //     name_ = name;
    //     MarkUpdated();
    // }
    void Display() const override
    {
        Party::Display();
        std::cout << "  Dealership: " << (hasDealership_ ? "Yes" : "No");
        if (hasDealership_)
            std::cout << "  Expiry: " << dealershipExpiry_.ToDateString()
                      << "  GST: " << gstNumber_;
        std::cout << "\n";
    }
    std::string Serialize() const override
    {
        return Party::Serialize() + "|" + (hasDealership_ ? "has dealership" : "No dealership") + "|" + (dealershipExpiry_.ToDateString()) + '|' + (hasDealership_ ? gstNumber_ : "");
    }
    void Validate() const override
    {
        Party::Validate();
        if (hasDealership_ && gstNumber_.empty())
            throw std::runtime_error("Company:invalid gst number");
        if (hasDealership_ && dealershipExpiry_ == DateTime(0))
            throw std::runtime_error("Company: invalid expiry date");
        if (hasDealership_)
        {
            if (phone_.empty())
                throw std::runtime_error("company: phone number cannot be empty");
            if (phone_.size() != 10)
                throw std::runtime_error("company:phone number should have 10 digits");
            for (int i = 0; i < 10; i++)
            {
                if (phone_[i] > '9' || phone_[i] < '0')
                    throw std::runtime_error("company:phone number digits should be 0-9");
            }
            if (address_.empty())
                throw std::runtime_error("company:address cannot be empty");
        }
    }
};
