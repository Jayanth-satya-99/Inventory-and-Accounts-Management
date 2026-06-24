#include "BaseEntity.hpp"
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