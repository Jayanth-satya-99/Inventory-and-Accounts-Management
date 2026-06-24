#pragma once


#include "BaseEntity.hpp"
#include"Party.hpp"
#include "Enums.hpp"


#include <iostream>
#include <string>
#include <stdexcept>

class Customer: public Party{
    protected:
    double totalPurchase_;
    int totalbills_;
    public:
    virtual Party_Type getCustomerType()const=0;
    virtual bool canBuyOnLoan()const=0;
    explicit Customer(const std::string name,
                   const std::string phone,
                   const std::string email,
                   const std::string address,int totalPurchase=0,int totalbills=0)
        : Party(name,phone,email,address),totalPurchase_(totalPurchase),totalbills_(totalbills){
            if(totalPurchase<0)throw std:: invalid_argument("Customer: total purchases canot be negative");
        }
    Customer(int id,DateTime&c,DateTime&u,
                const std::string name,
                   const std::string phone,
                   const std::string email,
                   const std::string address,int totalPurchase,int totalbills)
        : Party(id,c,u,name,phone,email,address),totalPurchase_(totalPurchase),totalbills_(totalbills){
            if(totalPurchase<0)throw std:: invalid_argument("Customer: total purchases canot be negative");
        }
        double getTotalPurchases()const{return totalPurchase_;}
        int getTotalBills()const {return totalbills_;}
        Party_Type getPartyType()const override{return getCustomerType();}
        std:: string GetEntityType()const override{return "Customer"; }
        void setTotalPurchase(double purchase){
            totalPurchase_=purchase;
            MarkUpdated();
        }
        void setTotalBills(){
            totalbills_+=1;
            MarkUpdated();
        }

};