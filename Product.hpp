#pragma once

#include "BaseEntity.hpp"
#include "Enums.hpp"
#include "Category.hpp"
#include "SubCategory.hpp"
#include "Company.hpp"
#include "Distributor.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
// it has details only about the product but not about the product varient
// name,category,subcategory,company,sourcetype,distributor,has expiry,is active
// two subclasses perishable,imperishable
class Product : public BaseEntity
{
protected:
    std::string name_;
    Category *category_;
    SubCategory *subcategory_;
    Company *company_;
    Source_Type sourceType_;
    bool hasExpiry_;
    Distributor *distributor_;
    bool isActive_;
    

public:
    // consturctor
    Product(const std::string name,
            Category *category,
            SubCategory *subCategory,
            Company *company,
            Source_Type sourceType,
            bool hasExpiry,
            Distributor *distributor = nullptr)
        : name_(name),
          category_(category),
          subcategory_(subCategory),
          company_(company),
          sourceType_(sourceType),
          hasExpiry_(hasExpiry),
          distributor_(distributor),
          isActive_(true)
    {
        if (!category_)
            throw std::invalid_argument("category cannot be null");
        if (!subcategory_)
            throw std::invalid_argument("sub category cannot be null");
        if (!company_)
            throw std::invalid_argument("company cannot be null");
        if (sourceType_ == Source_Type::Distributor && !distributor_)
            throw std::invalid_argument("source is distributor so distributor cannot be null");
    }
    // constructor
    Product(int id, const DateTime &c, const DateTime &u,
            const std::string name,
            Category *category,
            SubCategory *subCategory,
            Company *company,
            Source_Type sourceType,
            bool hasExpiry,
            Distributor *distributor,
            
            bool isActive) : BaseEntity(id, c, u),
                             name_(name),
                             category_(category),
                             subcategory_(subCategory),
                             company_(company),
                             sourceType_(sourceType),
                             hasExpiry_(hasExpiry),
                             distributor_(distributor),
                             isActive_(isActive) {}
    virtual ~Product() = default;                   // disturctor
                                                    // getters
    virtual std::string getProductType() const = 0; // subclasses will define it. it tells about the perishability
    const std::string &getName() const { return name_; }
    Category *getCategory() const { return category_; }
    SubCategory *getSubCategory() const { return subcategory_; }
    Company *getCompany() const { return company_; }
    Source_Type getSourceType() const { return sourceType_; }
    bool hasExpiry() const { return hasExpiry_; }
    bool isActive() const { return isActive_; }
    Distributor *getDistributor() const { return distributor_; }
    std::string getFullName() const
    {
        return category_->getName() + " > " + subcategory_->getName() + " > " + name_;
    }
    // setters
    void setName(std::string name)
    {
        if (name.empty())
            throw std::invalid_argument("product:name cannot be empty");
        name_ = name;
        MarkUpdated();
    }
    void setCategory(Category *category)
    {
        if (!category)
            throw std::invalid_argument("product:catgory cannot be null");
        category_ = category;
        MarkUpdated();
    }
    void setSubCategory(SubCategory *subcategory)
    {
        if (!subcategory)
            throw std::invalid_argument("product:sub catgory cannot be null");
        subcategory_ = subcategory;
        MarkUpdated();
    }
    void setCompany(Company *company)
    {
        if (!company)
            throw std::invalid_argument("product: company cannot be null");
        company_ = company;
        MarkUpdated();
    }
    void setActive(bool isactive)
    {
        isActive_ = isactive;
        MarkUpdated();
    }
    void setSourceType(Source_Type st, Distributor *distributor = nullptr)
    {
        if (st == Source_Type::Distributor && !distributor)
            throw std::invalid_argument("product: changing sourcetype but distributor is null ");
        sourceType_ = st;
        distributor_ = distributor;
        MarkUpdated();
    }
    void setDistributor(Distributor *distributor)
    {
        if (sourceType_ == Source_Type::Dealership)
            throw std::invalid_argument("product: cannot assign ditributor , source type is dealership");
        if (!distributor)
            throw std::invalid_argument("product: distributor cannot be null");
        distributor_ = distributor;
        MarkUpdated();
    }
    void Display() const override
    {
        std::cout << "[" << getProductType() << "] " << std::to_string(getId())
                  << getFullName() << " "
                  << "source " << (sourceType_ == Source_Type::Dealership ? "dealership" : "distributor")
                  << "supplier " << (sourceType_ == Source_Type::Dealership ? company_->getName() : distributor_->getName()) // here i need to use getters
                  << "company " << company_->getName()
                  << (isActive_ ? "[Active]" : "[inactive]")
                  << std::endl;
    }
    std::string Serialize() const override
    {
        return getProductType() + "|" + name_ + "|" + std::to_string(category_->getId()) + "|" + std::to_string(subcategory_->getId()) + "|" + std::to_string(static_cast<int>(sourceType_))
                +"|"+std::to_string(company_->getId())
                +"|"+((sourceType_==Source_Type::Dealership)?"N/A":std::to_string(distributor_->getId()))
               + "|" + (hasExpiry_ ? "1" : "0") + "|" + (isActive_ ? "1" : "0");
    }
    std::string GetEntityType() const override { return "Product"; }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (name_.empty())
            throw std::runtime_error("Product: name cannot be empty");
        if (!category_)
            throw std::runtime_error("Product: category is null");
        if (!subcategory_)
            throw std::runtime_error("Product: subCategory is null");
        if (!company_)
            throw std::runtime_error("Product: subcompany is null");
        if (!subcategory_)
            throw std::runtime_error("Product: subCategory is null");
        if (sourceType_ == Source_Type::Distributor && !distributor_)
            throw std::runtime_error("Product: distributor is null");
    }
};

//  NonPerishableProduct  –  product with expiry
class PerishableProduct : public Product
{
public:
    PerishableProduct(
        const std::string name,
        Category *category,
        SubCategory *subCategory,
        Company *company,
        Source_Type sourceType,
        Distributor *distributor = nullptr) : Product(name, category, subCategory, company, sourceType, true, distributor) {}

    std::string getProductType() const override { return "PerishableProduct"; }
};

//  NonPerishableProduct  –  product with no expiry
class NonPerishableProduct : public Product
{
private:
    int warrantyDays_;

public:
    NonPerishableProduct(
        const std::string name,
        Category *category,
        SubCategory *subCategory,
        Company *company,
        Source_Type sourceType,
        Distributor *distributor = nullptr,
        int warrantyDays=0) : Product(name, category, subCategory, company, sourceType, false, distributor),
                            warrantyDays_(warrantyDays)
    {
    }

    int getWarrantyDays() const { return warrantyDays_; }
    void setWarrantyDays(int d)
    {
        warrantyDays_ = d;
        MarkUpdated();
    }

    std::string getProductType() const override { return "NonPerishableProduct"; }

    void Display() const override
    {
        Product::Display();
        if (warrantyDays_ > 0)
            std::cout << "         warranty=" << warrantyDays_ << " days\n";
    }
};