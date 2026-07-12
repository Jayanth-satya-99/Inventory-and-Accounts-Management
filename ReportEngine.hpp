#pragma once

#include "PartyManager.hpp"
#include "StockManager.hpp"
#include "CustomerLedger.hpp"
#include "CompanyLedger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>

// ReportEngine — Singleton
// Pulls data from PartyManager and StockManager
// Provides display reports + CSV exports
class ReportEngine
{
private:
    ReportEngine() = default;

    PartyManager& pm_ = PartyManager::getInstance();
    StockManager& sm_ = StockManager::getInstance();

    // ── CSV helpers ───────────────────────────────────────────────────────────
    static std::string csvEscape(const std::string& s)
    {
        if (s.find(',') == std::string::npos &&
            s.find('"') == std::string::npos &&
            s.find('\n') == std::string::npos)
            return s;
        std::string out = "\"";
        for (char c : s) { if (c == '"') out += '"'; out += c; }
        out += '"';
        return out;
    }

    static bool inRange(const DateTime& date, const DateTime& from, const DateTime& to)
    {
        return date >= from && date <= to;
    }

    static std::string modeStr(Payment_Mode m)
    {
        switch(m) {
            case Payment_Mode::Cash:          return "Cash";
            case Payment_Mode::UPI:           return "UPI";
            case Payment_Mode::Card:          return "Card";
            case Payment_Mode::Bank_Transfer: return "Bank Transfer";
            case Payment_Mode::Cheque:        return "Cheque";
            default:                          return "Other";
        }
    }

    static std::string entryStr(Ledger_Entry_Type t)
    {
        switch(t) {
            case Ledger_Entry_Type::SaleBill:        return "Sale Bill";
            case Ledger_Entry_Type::PurchaseBill:    return "Purchase Bill";
            case Ledger_Entry_Type::LoanRepayment:   return "Loan Repayment";
            case Ledger_Entry_Type::AdvanceDeposit:  return "Advance Deposit";
            case Ledger_Entry_Type::AdvanceUsed:     return "Advance Used";
            case Ledger_Entry_Type::SupplierPayment: return "Supplier Payment";
            case Ledger_Entry_Type::Return:          return "Return";
            default:                                 return "Other";
        }
    }

    static void writeToFile(const std::string& path, const std::string& content)
    {
        std::ofstream f(path);
        if (!f.is_open())
            throw std::runtime_error("ReportEngine: cannot open file: " + path);
        f << content;
        f.close();
    }

public:
    static ReportEngine& getInstance()
    {
        static ReportEngine instance;
        return instance;
    }
    ReportEngine(const ReportEngine&)            = delete;
    ReportEngine& operator=(const ReportEngine&) = delete;

    // ═══════════════════════════════════════════════════════════════
    //  1. Sales Summary
    // ═══════════════════════════════════════════════════════════════
    void displaySalesSummary(DateTime from, DateTime to) const
    {
        double total = 0.0; int count = 0;
        for (SaleOrder* o : pm_.getAllSaleOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            total += o->getTotalAmount(); count++;
        }
        std::cout << "\n===== Sales Summary =====\n"
                  << "Period: " << from.ToDateString() << " to " << to.ToDateString() << "\n"
                  << "Orders:  " << count << "\n"
                  << "Revenue: Rs" << std::fixed << std::setprecision(2) << total << "\n";
    }

    void exportSalesSummaryCSV(const std::string& path,
                               DateTime from, DateTime to) const
    {
        std::ostringstream ss;
        ss << "OrderId,Customer,Date,Total,Paid,Due,Status\n";
        for (SaleOrder* o : pm_.getAllSaleOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            std::string status;
            switch(o->getStatus()) {
                case Order_Status::Pending: status = "Pending"; break;
                case Order_Status::Partial: status = "Partial"; break;
                case Order_Status::Paid:    status = "Paid";    break;
            }
            ss << o->getId() << ","
               << csvEscape(o->getCustomer()->getName()) << ","
               << o->getOrderDate().ToDateString() << ","
               << o->getTotalAmount() << ","
               << o->getTotalPaid() << ","
               << o->getAmountDue() << ","
               << status << "\n";
        }
        writeToFile(path, ss.str());
        std::cout << "Exported sales summary to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  2. Purchase Summary
    // ═══════════════════════════════════════════════════════════════
    void displayPurchaseSummary(DateTime from, DateTime to) const
    {
        double total = 0.0; int count = 0;
        for (PurchaseOrder* o : pm_.getAllPurchaseOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            total += o->getTotalAmount(); count++;
        }
        std::cout << "\n===== Purchase Summary =====\n"
                  << "Period: " << from.ToDateString() << " to " << to.ToDateString() << "\n"
                  << "Orders:  " << count << "\n"
                  << "Spent:   Rs" << std::fixed << std::setprecision(2) << total << "\n";
    }

    void exportPurchaseSummaryCSV(const std::string& path,
                                  DateTime from, DateTime to) const
    {
        std::ostringstream ss;
        ss << "OrderId,Supplier,Date,Total,Paid,Due,PaymentType,Status\n";
        for (PurchaseOrder* o : pm_.getAllPurchaseOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            std::string status, ptype;
            switch(o->getStatus()) {
                case Order_Status::Pending: status = "Pending"; break;
                case Order_Status::Partial: status = "Partial"; break;
                case Order_Status::Paid:    status = "Paid";    break;
            }
            ptype = (o->getPaymentType() == Payment_Type::Loan) ? "Loan" : "Spot";
            ss << o->getId() << ","
               << csvEscape(o->getSupplier()->getName()) << ","
               << o->getOrderDate().ToDateString() << ","
               << o->getTotalAmount() << ","
               << o->getAmountPaid() << ","
               << o->getAmountDue() << ","
               << ptype << ","
               << status << "\n";
        }
        writeToFile(path, ss.str());
        std::cout << "Exported purchase summary to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  3. Outstanding Dues
    // ═══════════════════════════════════════════════════════════════
    void displayOutstandingDues() const
    {
        std::cout << "\n===== Outstanding Customer Dues =====\n";
        for (auto& [id, ledger] : pm_.getAllCustomerLedgers()) {
            double due = ledger->totalDue();
            if (due > 0.001)
                std::cout << "  " << ledger->getParty()->getName()
                          << "  owes Rs" << std::fixed << std::setprecision(2) << due << "\n";
        }
        std::cout << "\n===== Outstanding Supplier Dues =====\n";
        for (auto& [id, ledger] : pm_.getAllCompanyLedgers()) {
            double due = ledger->totalDue();
            if (due > 0.001)
                std::cout << "  We owe " << ledger->getParty()->getName()
                          << "  Rs" << std::fixed << std::setprecision(2) << due << "\n";
        }
    }

    void exportOutstandingDuesCSV(const std::string& path) const
    {
        std::ostringstream ss;
        ss << "Type,Party,TotalBilled,TotalReceived,Outstanding\n";
        for (auto& [id, ledger] : pm_.getAllCustomerLedgers())
            ss << "Customer,"
               << csvEscape(ledger->getParty()->getName()) << ","
               << ledger->totalBilled() << ","
               << ledger->totalReceived() << ","
               << ledger->totalDue() << "\n";
        for (auto& [id, ledger] : pm_.getAllCompanyLedgers())
            ss << "Supplier,"
               << csvEscape(ledger->getParty()->getName()) << ","
               << ledger->totalPurchased() << ","
               << ledger->totalPaid() << ","
               << ledger->totalDue() << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported outstanding dues to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  4. Stock Report
    // ═══════════════════════════════════════════════════════════════
    void displayStockReport() const
    {
        std::cout << "\n===== Stock Report =====\n";
        for (StockItem* item : sm_.getAllStockItems())
            item->Display();
        auto low = sm_.getLowStockItems();
        if (!low.empty()) {
            std::cout << "\n--- Low Stock Alerts ---\n";
            for (StockItem* item : low)
                std::cout << "  [LOW] " << item->getProductVariant()->getDisplayName()
                          << "  Qty=" << item->getQuantity()
                          << "  Min=" << item->getMinStockLevel() << "\n";
        }
    }

    void exportStockReportCSV(const std::string& path) const
    {
        std::ostringstream ss;
        ss << "VariantId,Product,PackSize,Quantity,MinLevel,LowStock\n";
        for (StockItem* item : sm_.getAllStockItems())
            ss << item->getProductVariant()->getId() << ","
               << csvEscape(item->getProductVariant()->getProduct()->getName()) << ","
               << csvEscape(item->getProductVariant()->getPackSize()) << ","
               << item->getQuantity() << ","
               << item->getMinStockLevel() << ","
               << (item->isLowStock() ? "Yes" : "No") << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported stock report to " << path << "\n";
    }

    void exportExpiryReportCSV(const std::string& path, DateTime before) const
    {
        std::ostringstream ss;
        ss << "BatchNo,Product,PackSize,Quantity,ExpiryDate\n";
        for (ExpiryBatch* b : sm_.getExpiringBefore(before))
            ss << csvEscape(b->batchNo_) << ","
               << csvEscape(b->variant_->getProduct()->getName()) << ","
               << csvEscape(b->variant_->getPackSize()) << ","
               << b->quantity_ << ","
               << b->expiryDate_.ToDateString() << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported expiry report to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  5. Product-wise Sales Report
    // ═══════════════════════════════════════════════════════════════
    void exportProductWiseSalesCSV(const std::string& path,
                                   DateTime from, DateTime to) const
    {
        // map: productId -> {name, totalQty, totalRevenue}
        struct PStat { std::string name; int qty = 0; double revenue = 0.0; };
        std::map<int, PStat> stats;

        for (SaleOrder* o : pm_.getAllSaleOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            for (LineItem* item : o->getItems()) {
                if (item->isReturned()) continue;
                int pid = item->getProductVaraint()->getProduct()->getId();
                stats[pid].name    = item->getProductVaraint()->getProduct()->getName();
                stats[pid].qty    += item->getQuantity();
                stats[pid].revenue+= item->getTotal();
            }
        }

        std::ostringstream ss;
        ss << "ProductId,ProductName,TotalQtySold,TotalRevenue\n";
        for (auto& [id, s] : stats)
            ss << id << "," << csvEscape(s.name) << ","
               << s.qty << "," << s.revenue << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported product-wise sales to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  6. Variant-wise Sales Report
    // ═══════════════════════════════════════════════════════════════
    void exportVariantWiseSalesCSV(const std::string& path,
                                   DateTime from, DateTime to) const
    {
        struct VStat { std::string product, packSize; int qty = 0; double revenue = 0.0; };
        std::map<int, VStat> stats;

        for (SaleOrder* o : pm_.getAllSaleOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            for (LineItem* item : o->getItems()) {
                if (item->isReturned()) continue;
                int vid = item->getProductVaraint()->getId();
                stats[vid].product  = item->getProductVaraint()->getProduct()->getName();
                stats[vid].packSize = item->getProductVaraint()->getPackSize();
                stats[vid].qty     += item->getQuantity();
                stats[vid].revenue += item->getTotal();
            }
        }

        std::ostringstream ss;
        ss << "VariantId,Product,PackSize,TotalQtySold,TotalRevenue\n";
        for (auto& [id, s] : stats)
            ss << id << "," << csvEscape(s.product) << ","
               << csvEscape(s.packSize) << ","
               << s.qty << "," << s.revenue << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported variant-wise sales to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  7. Day-wise Sales Report (per variant)
    // ═══════════════════════════════════════════════════════════════
    void exportDayWiseSalesCSV(const std::string& path,
                               DateTime from, DateTime to) const
    {
        // map: date string -> {qty, revenue}
        struct DStat { int qty = 0; double revenue = 0.0; };
        // outer map: variantId -> date -> DStat
        std::map<int, std::map<std::string, DStat>> stats;
        std::map<int, std::string> variantNames;

        for (SaleOrder* o : pm_.getAllSaleOrders()) {
            if (!inRange(o->getOrderDate(), from, to)) continue;
            std::string dateStr = o->getOrderDate().ToDateString();
            for (LineItem* item : o->getItems()) {
                if (item->isReturned()) continue;
                int vid = item->getProductVaraint()->getId();
                variantNames[vid] = item->getProductVaraint()->getProduct()->getName()
                                  + " - " + item->getProductVaraint()->getPackSize();
                stats[vid][dateStr].qty     += item->getQuantity();
                stats[vid][dateStr].revenue += item->getTotal();
            }
        }

        std::ostringstream ss;
        ss << "VariantId,VariantName,Date,QtySold,Revenue\n";
        for (auto& [vid, dateMap] : stats)
            for (auto& [date, s] : dateMap)
                ss << vid << "," << csvEscape(variantNames[vid]) << ","
                   << date << "," << s.qty << "," << s.revenue << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported day-wise sales to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  8. Customer Statement
    // ═══════════════════════════════════════════════════════════════
    void exportCustomerStatementCSV(const std::string& path, int partyId) const
    {
        auto it = pm_.getAllCustomerLedgers().find(partyId);
        if (it == pm_.getAllCustomerLedgers().end())
            throw std::runtime_error("ReportEngine: no ledger for partyId " + std::to_string(partyId));

        CustomerLedgerPage* ledger = it->second;
        std::ostringstream ss;
        ss << "Customer: " << ledger->getParty()->getName() << "\n";
        ss << "Date,EntryType,Amount,Mode,OrderId\n";
        for (const LedgerEntry& e : ledger->getEntries())
            ss << e.date.ToDateString() << ","
               << csvEscape(entryStr(e.entryType)) << ","
               << e.amount << ","
               << csvEscape(modeStr(e.mode)) << ","
               << (e.orderId == -1 ? "-" : std::to_string(e.orderId)) << "\n";
        ss << "\nTotal Billed," << ledger->totalBilled() << "\n";
        ss << "Total Received," << ledger->totalReceived() << "\n";
        ss << "Outstanding," << ledger->totalDue() << "\n";
        ss << "Advance Balance," << ledger->getAdvanceBalance() << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported customer statement to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  9. Company/Distributor Statement
    // ═══════════════════════════════════════════════════════════════
    void exportCompanyStatementCSV(const std::string& path, int partyId) const
    {
        auto it = pm_.getAllCompanyLedgers().find(partyId);
        if (it == pm_.getAllCompanyLedgers().end())
            throw std::runtime_error("ReportEngine: no ledger for partyId " + std::to_string(partyId));

        CompanyLedgerPage* ledger = it->second;
        std::ostringstream ss;
        ss << "Supplier: " << ledger->getParty()->getName() << "\n";
        ss << "Date,EntryType,Amount,Mode,OrderId\n";
        for (const LedgerEntry& e : ledger->getEntries())
            ss << e.date.ToDateString() << ","
               << csvEscape(entryStr(e.entryType)) << ","
               << e.amount << ","
               << csvEscape(modeStr(e.mode)) << ","
               << (e.orderId == -1 ? "-" : std::to_string(e.orderId)) << "\n";
        ss << "\nTotal Purchased," << ledger->totalPurchased() << "\n";
        ss << "Total Paid," << ledger->totalPaid() << "\n";
        ss << "Outstanding," << ledger->totalDue() << "\n";
        ss << "Advance Balance," << ledger->getAdvanceBalance() << "\n";
        writeToFile(path, ss.str());
        std::cout << "Exported supplier statement to " << path << "\n";
    }

    // ═══════════════════════════════════════════════════════════════
    //  10. Export Everything
    // ═══════════════════════════════════════════════════════════════
    void exportAll(const std::string& folder,
                   DateTime from, DateTime to) const
    {
        auto p = [&](const std::string& f){ return folder + "/" + f; };

        exportSalesSummaryCSV   (p("sales_summary.csv"),    from, to);
        exportPurchaseSummaryCSV(p("purchase_summary.csv"), from, to);
        exportOutstandingDuesCSV(p("outstanding_dues.csv"));
        exportStockReportCSV    (p("stock_report.csv"));
        exportExpiryReportCSV   (p("expiry_report.csv"), DateTime::FromDateString("2099-12-31"));
        exportProductWiseSalesCSV(p("product_sales.csv"), from, to);
        exportVariantWiseSalesCSV(p("variant_sales.csv"), from, to);
        exportDayWiseSalesCSV    (p("day_wise_sales.csv"), from, to);

        // Individual customer statements
        for (auto& [id, ledger] : pm_.getAllCustomerLedgers())
            exportCustomerStatementCSV(
                p("customer_" + std::to_string(id) + ".csv"), id);

        // Individual supplier statements
        for (auto& [id, ledger] : pm_.getAllCompanyLedgers())
            exportCompanyStatementCSV(
                p("supplier_" + std::to_string(id) + ".csv"), id);

        std::cout << "All reports exported to " << folder << "\n";
    }
};