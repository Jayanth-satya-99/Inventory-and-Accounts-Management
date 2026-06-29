#pragma once


#include "BaseEntity.hpp"
#include "DateTime.hpp"
#include "ProductVariant.hpp"
#include "Structs.hpp"


#include<iostream>
#include<stdexcept>
#include<string>
#include<vector>
#include<math.h>


class StockItem: public BaseEntity{
    ProductVariant* variant_;
    std::vector<ExpiryBatch>batches_;
    int quantity_;
    int minStockLevel_;
    public:
    StockItem(ProductVariant* variant,int minStockLevel=0):variant_(variant),minStockLevel_(minStockLevel){
        if(!variant)throw std::invalid_argument("StockItem:productvariant cannot be nullptr");
        quantity_=0;
    }
    StockItem(int id,DateTime&c,DateTime&u,
         ProductVariant* variant,std::vector<ExpiryBatch>batches,int minStockLevel)
    :BaseEntity(id,c,u),variant_(variant),batches_(batches),minStockLevel_(minStockLevel){
        if(!variant)throw std::invalid_argument("StockItem:productvariant cannot be nullptr");
        quantity_=0;
        for(ExpiryBatch&b:batches){
            quantity_+=b.quantity_;
        }
    }
    ~StockItem()=default;
    void addStock(std::string batchNo,int qty ,int purchaseOrderId,DateTime expiryDate=DateTime()){
        ExpiryBatch batch;
        batch.batchNo_=batchNo;
        batch.expiryDate_=expiryDate;
        batch.purchaseOrderId_=purchaseOrderId;
        batch.quantity_=qty;
        batch.varaiant_=variant_;
    }
    void removeStock(int qty,std::string batchNo=""){
        if(qty>quantity_)throw std::invalid_argument("StockItem:the quantity to be removed is more than the actual quantity");
        quantity_-=qty;
        if(batchNo==""){
            for(ExpiryBatch& b:batches_){
                if(b.quantity_>0){
                   if(b.quantity_>=qty){
                    b.quantity_-=qty;
                    return;
                   }else{
                    b.quantity_=0;
                    qty-=b.quantity_;
                   }
                    
                }
            }
        }else{
            for(ExpiryBatch&b:batches_){
                if(b.batchNo_==batchNo){
                    if(qty>quantity_){quantity_+=qty; throw std::invalid_argument("StockItem:the quantity to be removed is more than the actual quantity");}
                    b.quantity_-=qty;
                    return;
                }
            }
        }
    }
    ExpiryBatch* getBatchByNo(std::string batchNo){
        for(int i=0;i<batches_.size();i++){
            if(batches_[i].batchNo_==batchNo)return &batches_[i];
        }
        return nullptr;
    }
   std:: vector<ExpiryBatch*>getExpiringBefore(DateTime date){
        std::vector<ExpiryBatch*>list;
        for(int i=0;i<batches_.size();i++){
            if(batches_[i].expiryDate_<date)list.push_back(&batches_[i]);
        }
        return list;
   }
   bool isLowStock()const{ return quantity_<minStockLevel_;}
   ProductVariant* getProductVariant() const{ return variant_;}
   void setMinStockLevel(int qty){minStockLevel_=qty;}

   void Validate()const override{}
    void Display()const override{}
    std::string Serialize()const override{return "";}
    std::string GetEntityType()const override{return " ";}

};