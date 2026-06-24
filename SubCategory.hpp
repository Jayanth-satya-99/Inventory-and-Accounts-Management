#pragma once
#include "BaseEntity.hpp"
#include "Category.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
//  SubCategory  –  quality tier/point of application within a Category
//  3 deafult tiers: Normal, Good, Best
//  new sub categories can be added at run time
//  Each SubCategory belongs to exactly one Category.
class SubCategory : public BaseEntity
{
private:
    std::string name_;
    Category *category_; // pointer to the catgories
                         // category and subcategory are separate antities but relation is made through pointer
    std::string description_;

public:
    // constructor  tier names are fixed but stored as string so new ones can be added
    explicit SubCategory(const std::string &name,
                         Category *category,
                         const std::string &description = "")
        : name_(name), category_(category), description_(description)
    {
        if (name.empty())
            throw std::invalid_argument("SubCategory name is empty like (\"\")");
        if (!category_)
            throw std::invalid_argument("SubCategory: category cannot be null");
    }

    // constructor Restore from storage
    SubCategory(int id, const DateTime &c, const DateTime &u,
                const std::string &name, Category *category,
                const std::string &desc)
        : BaseEntity(id, c, u), name_(name), category_(category), description_(desc)
    {
    }

    // getters to set name and description
    const std::string &getName() const { return name_; }
    const std::string &getDescription() const { return description_; }
    Category *getCategory() const { return category_; }

    // returns category and subcategory like this "Seeds > Good"
    std::string getFullName() const
    {
        return category_->getName() + " > " + name_;
    }

    // setters to update name and description
    void setName(const std::string &name)
    {
        if (name.empty())
            throw std::invalid_argument("SubCategory name cannot be empty");
        name_ = name;
        MarkUpdated();
    }
    void setDescription(const std::string &desc)
    {
        description_ = desc;
        MarkUpdated();
    }
    void setCategory(Category *cat)
    {
        if (!cat)
            throw std::invalid_argument("SubCategory: category cannot be null");
        category_ = cat;
        MarkUpdated();
    }

    // ── BaseEntity interface ─────────────────────────────────────────────────
    void Display() const override
    {
        std::cout << "[SubCategory] id=" << getId()
                  << "  " << getFullName();
        if (!description_.empty())
            std::cout << "  (" << description_ << ")";
        std::cout << "\n";
    }

    std::string Serialize() const override
    {
        return "SubCategory|" + std::to_string(getId()) + "|" + name_ + "|" + std::to_string(category_->getId()) + "|" + description_;
    }

    std::string GetEntityType() const override { return "SubCategory"; }

    void Validate() const override
    {
        BaseEntity::Validate();
        if (name_.empty())
            throw std::runtime_error("SubCategory: name cannot be empty");
        if (!category_)
            throw std::runtime_error("SubCategory: category cannot be null");
    }
};