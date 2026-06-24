#pragma once

#include "BaseEntity.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

//  Category for top-level product grouping
//  4 default categories Seeds, Pesticides, Herbicides, Fertilizers
//  New ones can be added at runtime.
class Category : public BaseEntity
{
private:
    std::string name_;
    std::string description_;

public:
    // constructor
    explicit Category(const std::string &name,
                      const std::string &description = "")
        : name_(name), description_(description)
    {
        if (name.empty())
            throw std::invalid_argument("Category name is empty like (\"\")");
    }

    // constructor Restore from storage
    Category(int id, const DateTime &c, const DateTime &u,
             const std::string &name, const std::string &desc)
        : BaseEntity(id, c, u), name_(name), description_(desc)
    {
    }

    const std::string &getName() const { return name_; }               // getter for name of the category
    const std::string &getDescription() const { return description_; } // getter for about the category

    // setter to update name of the category
    void setName(const std::string &name)
    {
        if (name.empty())
            throw std::invalid_argument("Category name cannot be empty");
        name_ = name;
        MarkUpdated();
    }
    // setter to update the description of the category
    void setDescription(const std::string &desc)
    {
        description_ = desc;
        MarkUpdated();
    }

    // displays details of the category
    void Display() const override
    {
        std::cout << "[Category] id=" << getId()
                  << "  " << name_;
        if (!description_.empty())
            std::cout << "  (" << description_ << ")";
        std::cout << "\n";
    }
    // dispalys details of the category and can be used for debugging
    std::string Serialize() const override
    {
        return "Category|" + std::to_string(getId()) + "|" + name_ + "|" + description_;
    }
    // polymorphism writing for the type of entity
    std::string GetEntityType() const override { return "Category"; }
    // checking are all entries valid
    void Validate() const override
    {
        BaseEntity::Validate();
        if (name_.empty())
            throw std::runtime_error("Category: name cannot be empty");
    }
};