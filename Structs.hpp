#pragma once

#include "Enums.hpp"
#include "DateTime.hpp"
#include "ProductVariant.hpp"
struct LedgerEntry{
    DateTime date;
    Ledger_Entry_Type entryType;
    double amount;
    Payment_Mode mode;
    int orderId;
};
struct ExpiryBatch{
    std::string batchNo_;
    ProductVariant* variant_;
    int quantity_;
    DateTime expiryDate_;
    int purchaseOrderId_;
    
};