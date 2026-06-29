#pragma once

#include "BaseEntity.hpp"
#include "ProductVariant.hpp"

#include<iostream>
#include<stdexcept>
#include<string>

class LineItem : public BaseEntity{
    ProductVariant* variant_;
    int quantity_;
    double pricePerUnit_;
   bool returned_;

    public:
    LineItem(ProductVariant* variant,
        int quantity,
        double pricePerUnit,
        bool returned=false
    ):variant_(variant),quantity_(quantity),pricePerUnit_(pricePerUnit),returned_(returned){
        if(variant==nullptr)throw std:: invalid_argument("Lineitem: product varant cannot br nullptr");
        if(quantity<=0)throw std::invalid_argument("Line item: quantity should be positive");
        if(pricePerUnit<0) throw std::invalid_argument("LineItem: price per unit shold not be negative");
        
        
    }
    LineItem(int id,DateTime& c,DateTime&u,
        ProductVariant* variant,
        int quantity,
        double pricePerUnit,
        bool returned
    ):BaseEntity(id,c,u),variant_(variant),quantity_(quantity),pricePerUnit_(pricePerUnit),returned_(returned){
        if(variant==nullptr)throw std:: invalid_argument("Lineitem: product varant cannot br nullptr");
        
        if(quantity<=0)throw std::invalid_argument("Line item: quantity should be positive");
         if(pricePerUnit<0) throw std::invalid_argument("LineItem: price per unit shold not be negative");
    }

    std ::string GetEntityType()const override{return "LineItem";}
    bool isReturned()const{return returned_;}
    void markReturned(){returned_=true;MarkUpdated();}
    double getTotal()const{return pricePerUnit_*quantity_;}
    int getQuantity()const {return quantity_;}
    double getPricePerUnit()const{return pricePerUnit_;}
    int getProductVariantId()const{return variant_->getId();}
    ProductVariant* getProductVaraint()const {return variant_;}
    void Display()const override{
        std::cout<<" Name:"<<variant_->getProduct()->getName()
            <<" Company:"<<variant_->getProduct()->getCompany()->getName()
            <<" Varaiant:"<<variant_->getPackSize()<<" "<<variant_->getUnit()
            <<" Quantity:"<<quantity_
            <<" PricePerUnit:"<<pricePerUnit_
            <<" Total Price:"<<getTotal()<<"\n";

    }
    std::string Serialize()const override{
        std::string ret=(returned_)?"returned":"Not returned";
        return variant_->getProduct()->getName()
            +" |"+variant_->getProduct()->getCompany()->getName()
            +" |"+variant_->getPackSize()+" "+ variant_->getUnit()
            +" |"+std::to_string(quantity_)
            +" |"+std::to_string(pricePerUnit_)
            +" |"+ std::to_string(getTotal())
            +" |"+ ret;
    }
    void Validate() const override{
        if(variant_==nullptr)throw std:: runtime_error("Line Item: product varaiant cannot be nullptr");
         if(quantity_<=0)throw std::runtime_error("Line item: quantity should be positive");
          if(pricePerUnit_<0) throw std::runtime_error("LineItem: price per unit shold not be negative");


    }

};