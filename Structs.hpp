#pragma once

#include "Enums.hpp"
#include "DateTime.hpp"
struct LedgerEntry{
    DateTime date;
    Ledger_Entry_Type entryType;
    double amount;
    int orderId;
};
