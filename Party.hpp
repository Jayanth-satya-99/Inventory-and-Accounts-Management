#pragma once
#include "Enums.hpp"
#include "BaseEntity.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
class Party : public BaseEntity
{protected:
    std::string name_;
    std::string phone_;
    std::string email_;
    std::string address_;
     void validateMembershipFields(const std::string &phone, const std::string &address,const std::string type)
    {
        if (phone.empty())
            throw std::invalid_argument(type+": phone number cannot be empty");
        if (phone.size() != 10)
            throw std::invalid_argument(type+":phone number should have 10 digits");
        for (int i = 0; i < 10; i++)
        {
            if (phone[i] > '9' || phone[i] < '0')
                throw std::invalid_argument(type+":phone number digits should be 0-9");
        }
        if (address.empty())
            throw std::invalid_argument(type+":address cannot be empty");
    }

public:
    explicit Party(const std::string name,
                   const std::string phone,
                   const std::string email,
                   const std::string address)
        : name_(name), phone_(phone), email_(email), address_(address)
    {
        if (name.empty())
            throw std::invalid_argument("Party: name cannot be empty");
        // if (phone.empty())
        //     throw std::invalid_argument("Part: phone number cannot be empty");
        // if (phone.size() != 10)
        //     throw std::invalid_argument("Party:phone number should have 10 digits");
        // for (int i = 0; i < 10; i++)
        // {
        //     if (phone[i] > '9' || phone[i] < '0')
        //         throw std::invalid_argument("Party:phone number digits should be 0-9");
        // }
        // if (address.empty())
        //     throw std::invalid_argument("party:address cannot be empty");
    }
    Party(int id, const DateTime &c, const DateTime &u,
          const std::string name,
          const std::string phone, // i neceasrily need phone number of a customer,distributor,retailshop. if i cont have the dealership of the company i necsarily do not need it
                                   // how do i chack the validity in party
                                   // should i remove the validity of phone number in this section and add it in the derived classes
          const std::string email,
          const std::string address)
        : BaseEntity(id, c, u), name_(name), phone_(phone), email_(email), address_(address)
    {
        if (name.empty())
            throw std::invalid_argument("Party: name cannot be empty");
        // if (phone.empty())
        //     throw std::invalid_argument("Part: phone number cannot be empty");
        // if (phone.size() != 10)
        //     throw std::invalid_argument("Party:phone number should have 10 digits");
        // for (int i = 0; i < 10; i++)
        // {
        //     if (phone[i] > '9' || phone[i] < '0')
        //         throw std::invalid_argument("Party:phone number digits should be 0-9");
        // }
        // if (address.empty())
        //     throw std::invalid_argument("party:address cannot be empty");
    }
    virtual ~Party() = default;
    // getters
    virtual Party_Type getPartyType() const = 0;
    const std::string &getName() const { return name_; }
    const std::string &getPhone() const { return phone_; }
    const std::string &getEmail() const { return email_; }
    const std::string &getAddress() const { return address_; }
    std::string GetEntityType() const override { return "Party"; }
    // setters
    void setName(std::string name)
    {
        if (name.empty())
            throw std::invalid_argument("Party: name cannot be empty");
        name_ = name;
        MarkUpdated();
    }
    void setPhone(std::string phone)
    {
        if (phone.empty())
            throw std::invalid_argument("Part: phone number cannot be empty");
        if (phone.size() != 10)
            throw std::invalid_argument("Party:phone number should have 10 digits");
        for (int i = 0; i < 10; i++)
        {
            if (phone[i] > '9' || phone[i] < '0')
                throw std::invalid_argument("Party:phone number digits should be 0-9");
        }
        phone_ = phone;
        MarkUpdated();
    }
    void setEmail(std::string email)
    {
        email_ = email;
        MarkUpdated();
    }
    void setAddress(std::string address)
    {
        if (address.empty())
            throw std::invalid_argument("Party:adrress cannot be empty");
        address_ = address;
        MarkUpdated();
    }
    // Distributor,
    // PermanentCustomer,
    // TemporaryCustomer,
    // RetailShop
    void Display() const override
    {
        std::string type;
        switch (getPartyType())
        {
        case Party_Type::Company:
            type = "Company";
            break;
        case Party_Type::Distributor:
            type = "Distributor";
            break;
        case Party_Type::PermanentCustomer:
            type = "PermanentCustomer";
            break;
        case Party_Type::TemporaryCustomer:
            type = "TemporaryCustomer";
            break;
        case Party_Type::RetailShop:
            type = "RetailShop";
            break;
        }
        std::cout << "[" << type << "]" << getId()
                  << " name " << name_
                  << " phone " << phone_
                  << ((!email_.empty()) ? " email " + email_ : "")
                  << " address " << address_
                  << std::endl;
    }
    std::string Serialize() const override
    {
        std::string type;
        switch (getPartyType())
        {
        case Party_Type::Company:
            type = "Company";
            break;
        case Party_Type::Distributor:
            type = "Distributor";
            break;
        case Party_Type::PermanentCustomer:
            type = "PermanentCustomer";
            break;
        case Party_Type::TemporaryCustomer:
            type = "TemporaryCustomer";
            break;
        case Party_Type::RetailShop:
            type = "RetailShop";
            break;
        }
        return type + "|" + std::to_string(getId()) + "|" + name_ + "|" + email_ + "|" + address_;
    }
    void Validate() const override
    {
        BaseEntity::Validate();
        if (name_.empty())
            throw std::runtime_error("Party: name cannot be empty");
        // if (phone_.empty())
        //     throw std::runtime_error("Part: phone number cannot be empty");
        // if (phone_.size() != 10)
        //     throw std::runtime_error("Party:phone number should have 10 digits");
        // for (int i = 0; i < 10; i++)
        // {
        //     if (phone_[i] > '9' || phone_[i] < '0')
        //         throw std::runtime_error("Party:phone number digits should be 0-9");
        // }
        // if (address_.empty())
        //     throw std::runtime_error("party:address cannot be empty");
    }
};