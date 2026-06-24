#pragma once

#include "BaseEntity.hpp"
#include "Party.hpp"
#include "Enums.hpp"
#include "Category.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
class Distributor:public Party{
    std:: string areaServed_;
std::vector <Category*>supportsCategory_;


public:
    Distributor(const std:: string name,
                const std:: string phone,
                const std:: string email,
                const std:: string address,
                const std:: string areaServed,
                std::vector<Category*>supportsCategory=std::vector<Category*>()    
    ):Party(name,phone,email,address),areaServed_(areaServed),supportsCategory_(supportsCategory){
       validateMembershipFields(phone,address,"Distributor");
       if(areaServed_.empty())throw std:: invalid_argument("Distributor: aresd served should not be empty");
    }
    Distributor(int id,DateTime&c,DateTime&u,
                const std:: string name,
                const std:: string phone,
                const std:: string email,
                const std:: string address,
                const std:: string areaServed,
                std::vector<Category*>supportsCategory=std::vector<Category*>()    
    ):Party(id,c,u,name,phone,email,address),areaServed_(areaServed),supportsCategory_(supportsCategory){
       validateMembershipFields(phone,address,"Distributor");
       if(areaServed_.empty())throw std:: invalid_argument("Distributor: aresd served should not be empty");
    }
    std:: string GetEntityType()const override{return "Distributor";}
    const std:: string& getAreaServed() const{return areaServed_;}
    const std:: vector<Category*>getSupportedCategory()const{return supportsCategory_;}
    Party_Type getPartyType() const override{return Party_Type::Distributor;}
    bool isCategorySupported(Category* cat){
        for(int i=0;i<supportsCategory_.size();i++){
            if(supportsCategory_[i]==cat) return true;
        }
        return false;
    }
    void setAreaServed(std:: string area){
        areaServed_=area;
        MarkUpdated();
    }
    void addCategory(Category* category){
        supportsCategory_.push_back(category);
        MarkUpdated();
    }
    void removeCategory(Category* category){
        int n=supportsCategory_.size();
        for(int i=0;i<n;i++){
            if(supportsCategory_[i]==category){
                supportsCategory_[i]=supportsCategory_[n-1];
                supportsCategory_[n-1]=nullptr;
                MarkUpdated();
                return;
            }
            
        }
        //throw std::invalid_argument("the given category is not in the support list");
    }
    void Display() const override{
        Party::Display();
        std::cout<<" areaServed:"<<areaServed_
        <<"no of categories supported is:"<<supportsCategory_.size()<<std::endl;
    }
    std::string Serialize() const override{
        return Party::Serialize()+"|"+areaServed_+"|"+std::to_string(supportsCategory_.size());
    }
    void Validate() const override{
        Party::Validate();
         if (phone_.empty())
                throw std::runtime_error("Distributor: phone number cannot be empty");
            if (phone_.size() != 10)
                throw std::runtime_error("Distributor:phone number should have 10 digits");
            for (int i = 0; i < 10; i++)
            {
                if (phone_[i] > '9' || phone_[i] < '0')
                    throw std::runtime_error("Distributor:phone number digits should be 0-9");
            }
            if (address_.empty())
                throw std::runtime_error("Distributor:address cannot be empty");
            if(areaServed_.empty()) throw std::runtime_error("Distributor: area served is empty");
    }
};