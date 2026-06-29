#pragma once


#include "BaseEntity.hpp"
#include "DateTime.hpp"
#include "ProductVariant.hpp"
#include "StockItem.hpp"


#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class StockManager: public BaseEntity{
    std::vector<StockItem*>stock_;
    StockManager()=default;
    public:
    static StockManager& getInstance(){
        static StockManager instance;
        return instance;
    }
    StockManager(const StockManager&)=delete;
    StockManager&operator=(const StockManager&)=delete;
    void registerVariant(ProductVariant*variant,int minStockLevel){
        for(int i=0;i<stock_.size();i++){
            if(stock_[i]->getProductVariant()==variant)throw std::invalid_argument("StockManger:this variant already exists");
        }
        stock_.push_back(new StockItem(variant,minStockLevel));
    }
    StockItem* getStockItemByVariantId(int id ){
        for(int i=0;i<stock_.size();i++){
            if(stock_[i]->getProductVariant()->getId()==id)return stock_[i];
        }
        return nullptr;
    }
    StockItem* getStockItemById(int id){
        for(int i=0;i<stock_.size();i++){
            if(stock_[i]->getId()==id)return stock_[i];
        }
        return nullptr;
    }
    std::vector<StockItem*> getStockItemsByProductId(Product* product){
        std::vector<StockItem*>list;
        for(int i=0;i<stock_.size();i++){
            if(stock_[i]->getProductVariant()->getProduct()==product)list.push_back(stock_[i]);
        }
        return list;
    }
    void addStock(int variantId,int qty, DateTime expiryDate,int purchaseOrderId,std::string batchNo ){
        StockItem* item=getStockItemByVariantId(variantId);
        if(item==nullptr)throw std::invalid_argument("StockManager:No such varaintId exists");
        item->addStock(batchNo,qty,purchaseOrderId,expiryDate);
    }
    void removeStock(int variantId,int qty,std:: string batchNo){
        StockItem* item=getStockItemByVariantId(variantId);
        if(item==nullptr)throw std::invalid_argument("StockManager:No such varaintId exists");
        item->removeStock(qty,batchNo);
    }
    std::vector<StockItem*>getLowStockItems(){
        std::vector<StockItem*>list;
        for(int i=0;i<stock_.size();i++){
            if(stock_[i]->isLowStock())list.push_back(stock_[i]);
        }
        return list;
    }
    std::vector<ExpiryBatch*>getExpiringBefore(DateTime date){
        std::vector<ExpiryBatch*>list;
        for(int i=0;i<stock_.size();i++){
            std::vector<ExpiryBatch*>l=stock_[i]->getExpiringBefore(date);
            list.insert(list.end(),l.begin(),l.end());
        }
        return list;
    }
    std::vector<std::vector<ExpiryBatch*>>getExpiringBefore(DateTime date,int c=0){
        std::vector<std::vector<ExpiryBatch*>>list;
        for(int i=0;i<stock_.size();i++){
            list.push_back(stock_[i]->getExpiringBefore(date));
        }
        return list;
    }

    void DisplayAll(){

    }




    void Validate()const override{}
    void Display()const override{}
    std::string Serialize()const override{return "";}
    std::string GetEntityType()const override{return " ";}

};