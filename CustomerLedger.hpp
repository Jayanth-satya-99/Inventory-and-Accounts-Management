#pragma once

#include "Party.hpp"
#include "SaleOrder.hpp"
#include "Enums.hpp"
#include "DateTime.hpp"
#include "Structs.hpp"
#include "PermanentCustomer.hpp"
#include "Retailshop.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class CustomerLedgerPage :public BaseEntity{
    int partyId_;
    Party* party_;
    std:: vector<LedgerEntry> entries_;
    std:: vector<SaleOrder*> orders_;
    public:
    CustomerLedgerPage(Party* party):party_(party){
        if(!party)throw std::invalid_argument("CustomerLedgerPage: party cannot be nullptr");
        partyId_=party_->getId();
    }
    CustomerLedgerPage(int id,DateTime& c,DateTime&u,Party* party,
        std:: vector<LedgerEntry> entries,std:: vector<SaleOrder*> orders)
        :BaseEntity(id,c,u),party_(party),entries_(entries),orders_(orders){
            if(!party)throw std::invalid_argument("CustomerLedgerPage: party cannot be nullptr");
            partyId_=party_->getId();
    }
    ~CustomerLedgerPage()=default;
    std:: string GetEntityType() const override{return "CustomerLedgerPage";}
    void addOrder(SaleOrder* order) {
        orders_.push_back(order);
    // add the bill entry
    entries_.push_back({DateTime::Now(), Ledger_Entry_Type::SaleBill, 
                        order->getTotalAmount(), order->getId()});
    // add each payment as separate entry
        for (const auto& p : order->getPayments()) {
        Ledger_Entry_Type type;
        if (p.type == Payment_Type::Advance) type = Ledger_Entry_Type::AdvanceUsed;
        else if (p.type == Payment_Type::Loan) type = Ledger_Entry_Type::LoanRepayment;
        else type = Ledger_Entry_Type::SupplierPayment; // Spot
        entries_.push_back({DateTime::Now(), type, p.amount, order->getId()});
        }
    
    }
    void addDeposit(double amount, Payment_Mode mode){
    double loanDue = totalDue();   // outstanding loan from ledger
    
        if (loanDue > 0) {
        double towardLoan = std::min(amount, loanDue);
        if(party_->getPartyType() ==Party_Type::PermanentCustomer)dynamic_cast<PermanentCustomer*>(party_)->repayLoan(towardLoan);
        entries_.push_back({DateTime(),Ledger_Entry_Type::LoanRepayment,towardLoan,-1});
        
        // add LoanRepayment entry
        // call customer->repayLoan(towardLoan)
        amount -= towardLoan;
        }
    
        if (amount > 0) {
            if(party_->getPartyType() ==Party_Type::PermanentCustomer)dynamic_cast<PermanentCustomer*>(party_)->addAdvance(amount);
            entries_.push_back({DateTime(),Ledger_Entry_Type::AdvanceDeposit,amount,-1});
        
        // add AdvanceDeposit entry
        // call customer->addAdvance(amount)
        }
    }
    void addEntry(LedgerEntry entry){entries_.push_back(entry);}
    SaleOrder* getOrderById(int orderId) const{
        if(orderId<-1)throw std:: invalid_argument("CustomerLedgerPage: orderid cannot be negative");
        for(int i=0;i<orders_.size();i++){
            if(orderId==orders_[i]->getId())return orders_[i];
        }
        throw std::invalid_argument("CustomerLedgerPage: orderid is not in the list");
    }
    double totalBilled()const{
        double amount=0;
        for(int i=0;i<entries_.size();i++){
            if(entries_[i].entryType==Ledger_Entry_Type::SaleBill||entries_[i].entryType==Ledger_Entry_Type::LoanRepayment)amount+=entries_[i].amount;
        }
        return amount;
    }
    double totalReceived()const{
        double amount=0;
        for(int i=0;i<entries_.size();i++){//here supplierPayment means spot
            if(entries_[i].entryType==Ledger_Entry_Type::AdvanceDeposit||entries_[i].entryType==Ledger_Entry_Type::LoanRepayment||entries_[i].entryType==Ledger_Entry_Type::SupplierPayment)amount+=entries_[i].amount;
        }
        return amount;
    }
    double totalDue()const{return totalBilled()-totalReceived();}
    void Display() const override{
        std::cout<<"=======================================================================================\n";
        std:: cout<<"";
    }//from here you take care of display,serialize,validate functions... it is not giving me any expericence 
    

};