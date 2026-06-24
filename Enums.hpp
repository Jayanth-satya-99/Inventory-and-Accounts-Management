#pragma once
enum class Payment_Mode
{
    Cash,
    UPI,
    Card,
    Bank_Transfer,
    Cheque
};
enum class Payment_Type
{
    Spot,
    Advance,
    Loan
};
enum class Source_Type
{
    Dealership,
    Distributor
};
enum class Order_Status
{
    Pending,
    Partial,
    Paid
};
enum class Party_Type
{
    Company,
    Distributor,
    PermanentCustomer,
    TemporaryCustomer,
    RetailShop
};
enum class Stock_Movement_Type
{
    Purchase,
    Sale,
    ReturnCustomer,
    ReturnCompany,
    Adjustment,
    Expired
};
enum class Ledger_Entry_Type {
    SaleBill,          // new sale order
    PurchaseBill,      // new purchase order
    LoanRepayment,     // customer paying back loan
    AdvanceDeposit,    // customer/supplier depositing advance
    AdvanceUsed,       // advance deducted against a bill
    SupplierPayment,   // we paying supplier against purchase
    Return             // item returned
};