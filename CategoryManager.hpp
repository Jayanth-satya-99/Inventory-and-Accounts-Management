#pragma once

#include "Category.hpp"
#include "SubCategory.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

//  One place to create, store, and look up every category/subcategory.
// all categories,subcategories are stored in the form of vector here
//  Usage:
//      CategoryManager& cm = CategoryManager::getInstance();
//      Category* seeds = cm.addCategory("Seeds", "Crop seeds");
//      SubCategory* good = cm.addSubCategory("Good", seeds->getId());
// ---------------------------------------------------------------------------
class CategoryManager
{
private:
    // Private constructor — only getInstance() can create this
    CategoryManager()
    {
        seedBuiltins();
    }

    // ── Seed the 4 built-in categories with Normal/Good/Best subcategories ───
    void seedBuiltins()
    {
        // Built-in categories
        Category *seeds = addCategory("Seeds", "All types of crop seeds");
        Category *pesticides = addCategory("Pesticides", "Pest control products");
        Category *herbicides = addCategory("Herbicides", "Weed control products");
        Category *fertilizers = addCategory("Fertilizers", "Soil nutrients");

        // Each category gets Normal / Good / Best independently
        for (Category *cat : {seeds, pesticides, herbicides, fertilizers})
        {
            addSubCategory("Normal", cat->getId(), "Standard quality");
            addSubCategory("Good", cat->getId(), "Above average quality");
            addSubCategory("Best", cat->getId(), "Premium quality");
        }
    }

    // Helper: find category by name (case-sensitive)
    Category *findCategoryByName(const std::string &name) const
    {
        for (Category *c : categories_)
            if (c->getName() == name)
                return c;
        return nullptr;
    }

    std::vector<Category *> categories_;
    std::vector<SubCategory *> subCategories_;

public:
    // ── Singleton access ─────────────────────────────────────────────────────
    static CategoryManager &getInstance()
    {
        static CategoryManager instance;
        return instance;
    }

    // Prevent copy and assignment (Singleton rules)
    CategoryManager(const CategoryManager &) = delete;
    CategoryManager &operator=(const CategoryManager &) = delete;

    // ── Category operations ──────────────────────────────────────────────────

    // Add a new category; throws if name already exists
    Category *addCategory(const std::string &name,
                          const std::string &description = "")
    {
        if (findCategoryByName(name))
            throw std::runtime_error("Category already exists: " + name);
        categories_.push_back(new Category(name, description));
        return categories_.back();
    }

    // Get category by id; returns nullptr if not found
    Category *getCategoryById(int id) const
    {
        for (Category *c : categories_)
            if (c->getId() == id)
                return c;
        return nullptr;
    }

    // Get category by name; returns nullptr if not found
    Category *getCategoryByName(const std::string &name) const
    {
        return findCategoryByName(name);
    }

    // All categories
    const std::vector<Category *> &getAllCategories() const
    {
        return categories_;
    }

    // Remove a category only if it has no subcategories
    bool removeCategory(int id)
    {
        for (auto it = categories_.begin(); it != categories_.end(); ++it)
        {
            if ((*it)->getId() == id)
            {
                if (!getSubCategoriesOf(id).empty())
                    throw std::runtime_error(
                        "Cannot remove category: it has subcategories");
                delete *it;
                categories_.erase(it);
                return true;
            }
        }
        return false; // not found
    }

    // ── SubCategory operations ───────────────────────────────────────────────

    // Add a new subcategory under a given category id
    SubCategory *addSubCategory(const std::string &name,
                                int categoryId,
                                const std::string &description = "")
    {
        Category *cat = getCategoryById(categoryId);
        if (!cat)
            throw std::runtime_error("Category not found with id: " + std::to_string(categoryId));
        // Prevent duplicate subcategory name within same category
        for (SubCategory *sc : subCategories_)
            if (sc->getCategory()->getId() == categoryId && sc->getName() == name)
                throw std::runtime_error(
                    "SubCategory '" + name + "' already exists under '" + cat->getName() + "'");

        subCategories_.push_back(new SubCategory(name, cat, description));
        return subCategories_.back();
    }

    // Get subcategory by id
    SubCategory *getSubCategoryById(int id) const
    {
        for (SubCategory *sc : subCategories_)
            if (sc->getId() == id)
                return sc;
        return nullptr;
    }

    // Get all subcategories belonging to a category
    std::vector<SubCategory *> getSubCategoriesOf(int categoryId) const
    {
        std::vector<SubCategory *> result;
        for (SubCategory *sc : subCategories_)
            if (sc->getCategory()->getId() == categoryId)
                result.push_back(sc);
        return result;
    }

    // Remove a subcategory by id
    bool removeSubCategory(int id)
    {
        for (auto it = subCategories_.begin(); it != subCategories_.end(); ++it)
        {
            if ((*it)->getId() == id)
            {
                delete *it;
                subCategories_.erase(it);
                return true;
            }
        }
        return false;
    }

    // ── Display ──────────────────────────────────────────────────────────────

    void displayAll() const
    {
        std::cout << "\n========== Categories & SubCategories ==========\n";
        for (Category *c : categories_)
        {
            c->Display();
            auto subs = getSubCategoriesOf(c->getId());
            if (subs.empty())
            {
                std::cout << "    (no subcategories)\n";
            }
            else
            {
                for (SubCategory *sc : subs)
                {
                    std::cout << "    ---> ";
                    sc->Display();
                }
            }
        }
        std::cout << "================================================\n";
    }

    int categoryCount() const { return (int)categories_.size(); }
    int subCategoryCount() const { return (int)subCategories_.size(); }

    // ── Destructor: clean up owned objects ───────────────────────────────────
    ~CategoryManager()
    {
        for (Category *c : categories_)
            delete c;
        for (SubCategory *sc : subCategories_)
            delete sc;
    }
};