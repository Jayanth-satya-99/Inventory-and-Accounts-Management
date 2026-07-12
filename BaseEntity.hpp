#pragma once
#include "DateTime.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>

class BaseEntity
{
private:
    int id_;
    DateTime CreatedAt_;
    DateTime UpdatedAt_;
    static std::atomic<int> NextId_;

protected:
    void SetId(int id) { id_ = id; }

public:
    BaseEntity();                                        // contructor BaseEntity b; cannot be used because abstract class as there is some pure virtual function. compile says object is incomplete
    BaseEntity(int, const DateTime &, const DateTime &); // constructor same here;
    virtual ~BaseEntity() = default;                     // desturctor making virtual so first the derived class deleted and then base

    BaseEntity(const BaseEntity &) = delete;            // by putting "delete" we are forbiding copy
    BaseEntity &operator=(const BaseEntity &) = delete; // same for bidding copy
    BaseEntity(BaseEntity &&) = default;                // just saying moving is allowed by using default and the compiler now how to do the default mwthod
    BaseEntity &operator=(BaseEntity &&) = default;     // same for just saying moving is allowed
                                                    // pure virtual functions.. derived classes will tell how the these functions or methods work
    virtual void Display() const = 0;              // dispaly the content
    virtual std::string Serialize() const = 0;     // for debugging
    virtual std::string GetEntityType() const = 0; // for knowing classname

    virtual void Validate() const;
    virtual void MarkUpdated();
    // getters
    int getId() const { return id_; }
    DateTime GetCreatedAt() const { return CreatedAt_; }
    DateTime GetUpdatedAt() const { return UpdatedAt_; }

    bool operator==(const BaseEntity &O) const { return id_ == O.id_; }
    bool operator!=(const BaseEntity &O) const { return id_ != O.id_; }
};
//#include "BaseEntity.hpp"
BaseEntity::BaseEntity() : id_(NextId_.fetch_add(1)), CreatedAt_(DateTime::Now()), UpdatedAt_(DateTime::Now()) {}
BaseEntity::BaseEntity(int id, const DateTime &CreatedAt, const DateTime &UpdatedAt)
    : id_(id), CreatedAt_(CreatedAt), UpdatedAt_(UpdatedAt)
{
    int current = NextId_.load();
    while (id >= current)
    {
        NextId_.compare_exchange_weak(current, id + 1);
        current = NextId_.load();
    }
}
void BaseEntity::Validate() const
{
    if (id_ <= 0)
        throw std::runtime_error("BaseEntity: id must be > 0, got " + std::to_string(id_));
}
void BaseEntity::MarkUpdated()
{
    UpdatedAt_ = DateTime::Now();
}

std::atomic<int> BaseEntity::NextId_(1);