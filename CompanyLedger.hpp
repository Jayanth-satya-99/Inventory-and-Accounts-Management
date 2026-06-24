#pragma once

#include "Party.hpp"
#include "PurchaseOrder.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"
#include "Structs.hpp"
#include "Company.hpp"
#include "Distributor.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class CompanyLedgerPage :public BaseEntity{
    int partyId_;
    Party* party_;
    std:: vector<LedgerEntry> entries_;
    std:: vector<PurchaseOrder*> orders_;
    double advanceBalance_;
    public:
    CompanyLedgerPage(Party* party,double advanceBalance=0):party_(party),advanceBalance_(advanceBalance){
        if(!party)throw std::invalid_argument("CompanyLedgerPage: party cannot be nullptr");
        partyId_=party_->getId();
    }
    CompanyLedgerPage(int id,DateTime& c,DateTime&u,Party* party,
        std:: vector<LedgerEntry> entries,std:: vector<PurchaseOrder*> orders,double advanceBalance)
        :BaseEntity(id,c,u),party_(party),entries_(entries),orders_(orders),advanceBalance_(advanceBalance){
            if(!party)throw std::invalid_argument("CompanyLedgerPage: party cannot be nullptr");
            partyId_=party_->getId();
    }
    ~CompanyLedgerPage()=default;
    std:: string GetEntityType() const override{return "CompanyLedgerPage";}
    void addOrder(PurchaseOrder* order) {
        orders_.push_back(order);
    // add the bill entry
    DateTime date;
    entries_.push_back({date, Ledger_Entry_Type::SaleBill, 
                        order->getTotalAmount(), order->getId()});
    // here we have only  how much paid.but we lost track of payment mode i think it is better to keep track of the mode of payment for company..  
    // entries_.push_back({date,Ledger_Entry_Type::SupplierPayment,order->getAmountPaid(),order->getId()});
    //     for (const auto& p : order->getPayments()) {
    //     Ledger_Entry_Type type;
    //     if (p.type == Payment_Type::Advance) type = Ledger_Entry_Type::AdvanceUsed;
    //     else if (p.type == Payment_Type::Loan) type = Ledger_Entry_Type::LoanRepayment;
    //     else type = Ledger_Entry_Type::SupplierPayment; // Spot
    //     entries_.push_back({DateTime::Now(), type, p.amount, order->getId()});
    //     }
    
    }
    
    
    
    //from here you take care of display,serialize,validate functions... it is not giving me any expericence 
    

};