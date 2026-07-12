#include "Shop.hpp"
#include <iostream>
#include <string>
#include <limits>

// ── Input helpers ─────────────────────────────────────────────────────────────
int getInt(const std::string& prompt)
{
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) { std::cin.ignore(); return val; }
        std::cout << "  Invalid. Enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double getDouble(const std::string& prompt)
{
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) { std::cin.ignore(); return val; }
        std::cout << "  Invalid. Enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string getString(const std::string& prompt)
{
    std::string val;
    std::cout << prompt;
    std::getline(std::cin, val);
    return val;
}

Payment_Mode getPaymentMode()
{
    std::cout << "  Mode: 1=Cash 2=UPI 3=Card 4=BankTransfer 5=Cheque\n";
    switch(getInt("  Choice: ")) {
        case 2: return Payment_Mode::UPI;
        case 3: return Payment_Mode::Card;
        case 4: return Payment_Mode::Bank_Transfer;
        case 5: return Payment_Mode::Cheque;
        default: return Payment_Mode::Cash;
    }
}

DateTime getDate(const std::string& prompt)
{
    while (true) {
        try {
            return DateTime::FromDateString(getString(prompt + " (YYYY-MM-DD): "));
        } catch (...) {
            std::cout << "  Invalid date format. Try again.\n";
        }
    }
}

// ── Party Menu ────────────────────────────────────────────────────────────────
void menuParties(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Parties -----\n"
                  << "1.  Add Company\n"
                  << "2.  Add Distributor\n"
                  << "3.  Add Permanent Customer\n"
                  << "4.  Add Temporary Customer\n"
                  << "5.  Add Retail Shop\n"
                  << "6.  View All Parties\n"
                  << "7.  Find Party by ID\n"
                  << "8.  Edit Party\n"
                  << "9.  View Party Ledger\n"
                  << "0.  Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            if (c == 1) {
                std::string name = getString("Company name: ");
                bool dealer = getInt("Has dealership? 1=Yes 0=No: ");
                std::string phone, address, gst;
                DateTime expiry;
                if (dealer) {
                    phone   = getString("Phone: ");
                    address = getString("Address: ");
                    gst     = getString("GST (15 chars): ");
                    expiry  = getDate("Dealership expiry");
                }
                auto* co = shop.addCompany(name, phone, "", address, dealer, expiry, gst);
                std::cout << "Added company id=" << co->getId() << "\n";
            }
            else if (c == 2) {
                std::string name    = getString("Distributor name: ");
                std::string phone   = getString("Phone: ");
                std::string address = getString("Address: ");
                std::string area    = getString("Area served: ");
                auto* d = shop.addDistributor(name, phone, "", address, area);
                std::cout << "Added distributor id=" << d->getId() << "\n";
            }
            else if (c == 3) {
                std::string name    = getString("Customer name: ");
                std::string phone   = getString("Phone: ");
                std::string address = getString("Address: ");
                double limit        = getDouble("Credit limit (Rs): ");
                auto* cu = shop.addPermanentCustomer(name, phone, "", address, limit);
                std::cout << "Added permanent customer id=" << cu->getId() << "\n";
            }
            else if (c == 4) {
                std::string name  = getString("Name: ");
                std::string phone = getString("Phone: ");
                auto* cu = shop.addTemporaryCustomer(name, phone);
                std::cout << "Added temporary customer id=" << cu->getId() << "\n";
            }
            else if (c == 5) {
                std::string name    = getString("Shop name: ");
                std::string phone   = getString("Phone: ");
                std::string address = getString("Address: ");
                std::string gst     = getString("GST (optional, Enter to skip): ");
                auto* r = shop.addRetailShop(name, phone, "", address, gst);
                std::cout << "Added retail shop id=" << r->getId() << "\n";
            }
            else if (c == 6) shop.displayAllParties();
            else if (c == 7) {
                int id = getInt("Party ID: ");
                Party* p = shop.findPartyById(id);
                if (p) p->Display();
                else   std::cout << "Not found.\n";
            }
            else if (c == 8) {
                int id = getInt("Party ID to edit: ");
                Party* p = shop.findPartyById(id);
                if (!p) { std::cout << "Not found.\n"; continue; }
                p->Display();
                std::cout << "Edit: 1=Name 2=Phone 3=Email 4=Address "
                          << "5=CreditLimit(Perm) 6=DistributorArea 7=CompanyDealership\n";
                int ec = getInt("Choice: ");
                if      (ec == 1) shop.editPartyName(id, getString("New name: "));
                else if (ec == 2) shop.editPartyPhone(id, getString("New phone: "));
                else if (ec == 3) shop.editPartyEmail(id, getString("New email: "));
                else if (ec == 4) shop.editPartyAddress(id, getString("New address: "));
                else if (ec == 5) shop.editCustomerCreditLimit(id, getDouble("New credit limit: "));
                else if (ec == 6) shop.editDistributorArea(id, getString("New area: "));
                else if (ec == 7) {
                    bool has = getInt("Has dealership? 1=Yes 0=No: ");
                    if (has) {
                        std::string ph  = getString("Phone: ");
                        std::string em  = getString("Email: ");
                        std::string ad  = getString("Address: ");
                        std::string gst = getString("GST: ");
                        DateTime exp    = getDate("Expiry");
                        shop.editCompanyDealership(id, true, ph, em, ad, exp, gst);
                    } else {
                        shop.editCompanyDealership(id, false);
                    }
                }
                std::cout << "Updated.\n";
            }
            else if (c == 9) {
                int id = getInt("Party ID: ");
                Party* p = shop.findPartyById(id);
                if (!p) { std::cout << "Not found.\n"; continue; }
                auto pt = p->getPartyType();
                if (pt == Party_Type::Company || pt == Party_Type::Distributor)
                    shop.displaySupplierLedger(id);
                else
                    shop.displayCustomerLedger(id);
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Product Menu ──────────────────────────────────────────────────────────────
void menuProducts(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Products -----\n"
                  << "1. Add Product\n"
                  << "2. Add Product Variant\n"
                  << "3. View All Products\n"
                  << "4. View All Variants\n"
                  << "5. Edit Product\n"
                  << "6. Edit Variant Prices\n"
                  << "7. Register Variant for Stock\n"
                  << "8. Edit Min Stock Level\n"
                  << "0. Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            if (c == 1) {
                shop.displayCategories();
                int catId = getInt("Category ID: ");
                Category* cat = shop.categories().getCategoryById(catId);
                if (!cat) { std::cout << "Category not found.\n"; continue; }

                auto subs = shop.categories().getSubCategoriesOf(catId);
                for (auto* s : subs) s->Display();
                int subId = getInt("SubCategory ID: ");
                SubCategory* sub = shop.categories().getSubCategoryById(subId);
                if (!sub) { std::cout << "SubCategory not found.\n"; continue; }

                shop.displayAllParties();
                int compId = getInt("Company ID (manufacturer): ");
                Party* compParty = shop.findPartyById(compId);
                if (!compParty || compParty->getPartyType() != Party_Type::Company)
                { std::cout << "Company not found.\n"; continue; }
                Company* company = dynamic_cast<Company*>(compParty);

                std::cout << "Source: 1=Dealership 2=Distributor\n";
                Source_Type src = (getInt("Choice: ") == 1)
                                  ? Source_Type::Dealership : Source_Type::Distributor;

                Distributor* dist = nullptr;
                if (src == Source_Type::Distributor) {
                    int dId = getInt("Distributor ID: ");
                    Party* dp = shop.findPartyById(dId);
                    if (!dp || dp->getPartyType() != Party_Type::Distributor)
                    { std::cout << "Distributor not found.\n"; continue; }
                    dist = dynamic_cast<Distributor*>(dp);
                }

                std::string name = getString("Product name: ");
                std::cout << "Type: 1=Perishable 2=NonPerishable\n";
                int type = getInt("Choice: ");
                Product* prod = nullptr;
                if (type == 1) {
                    prod = shop.addPerishableProduct(name, cat, sub, company, src, dist);
                } else {
                    int wd = getInt("Warranty days (0 if none): ");
                    prod = shop.addNonPerishableProduct(name, cat, sub, company,
                                                        src, dist, wd);
                }
                std::cout << "Added product id=" << prod->getId() << "\n";
            }
            else if (c == 2) {
                shop.displayAllProducts();
                int pid = getInt("Product ID: ");
                Product* prod = shop.products().getProductById(pid);
                if (!prod) { std::cout << "Product not found.\n"; continue; }

                std::string pack  = getString("Pack size (e.g. 1 kg): ");
                std::string unit  = getString("Unit (kg/ml/piece etc): ");
                double buy        = getDouble("Buy price (Rs): ");
                double sell       = getDouble("Sell price (Rs): ");
                std::string bar   = getString("Barcode (Enter to skip): ");

                ProductVariant* v = shop.addProductVariant(prod, pack, unit,
                                                           buy, sell, bar);
                std::cout << "Added variant id=" << v->getId() << "\n";
            }
            else if (c == 3) shop.displayAllProducts();
            else if (c == 4) shop.displayAllVariants();
            else if (c == 5) {
                shop.displayAllProducts();
                int pid = getInt("Product ID: ");
                std::cout << "Edit: 1=Name 2=Active/Inactive\n";
                int ec = getInt("Choice: ");
                if      (ec == 1) shop.editProductName(pid, getString("New name: "));
                else if (ec == 2) shop.editProductActive(pid, getInt("1=Active 0=Inactive: "));
                std::cout << "Updated.\n";
            }
            else if (c == 6) {
                shop.displayAllVariants();
                int vid = getInt("Variant ID: ");
                std::cout << "Edit: 1=Buy Price 2=Sell Price\n";
                int ec = getInt("Choice: ");
                if      (ec == 1) shop.editVariantBuyPrice(vid, getDouble("New buy price: "));
                else if (ec == 2) shop.editVariantSellPrice(vid, getDouble("New sell price: "));
                std::cout << "Updated.\n";
            }
            else if (c == 7) {
                shop.displayAllVariants();
                int vid = getInt("Variant ID: ");
                ProductVariant* v = shop.products().getVariantById(vid);
                if (!v) { std::cout << "Variant not found.\n"; continue; }
                int minLevel = getInt("Min stock level: ");
                shop.registerVariantForStock(v, minLevel);
                std::cout << "Registered for stock tracking.\n";
            }
            else if (c == 8) {
                int vid      = getInt("Variant ID: ");
                int minLevel = getInt("New min stock level: ");
                shop.editVariantMinStock(vid, minLevel);
                std::cout << "Updated.\n";
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Sale Menu ─────────────────────────────────────────────────────────────────
void menuSale(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Sales -----\n"
                  << "1. New Sale\n"
                  << "2. Return Item from Past Sale\n"
                  << "3. Customer Deposit\n"
                  << "4. View Customer Orders\n"
                  << "5. View All Sale Orders\n"
                  << "0. Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            if (c == 1) {
                int custId = getInt("Customer ID: ");
                Party* cust = shop.findPartyById(custId);
                if (!cust) { std::cout << "Not found.\n"; continue; }

                SaleOrder* order = shop.createSaleOrder(cust);

                while (true) {
                    std::cout << "\n  1=Add item  2=Done\n";
                    if (getInt("  Choice: ") == 2) break;
                    shop.displayStock();
                    int varId = getInt("  Variant ID: ");
                    StockItem* si = shop.stock().getStockItemByVariantId(varId);
                    if (!si) { std::cout << "  Not in stock.\n"; continue; }

                    int qty      = getInt("  Quantity: ");
                    double price = getDouble("  Price per unit (Rs): ");
                    std::string batchNo = "";
                    if (si->getProductVariant()->getProduct()->hasExpiry()) {
                        si->Display();
                        batchNo = getString("  Batch No: ");
                    }
                    shop.addLineItemToSale(order, si->getProductVariant(),
                                           qty, price, batchNo);
                    std::cout << "  Added. Running total: Rs"
                              << order->getTotalAmount() << "\n";
                }

                std::cout << "\nTotal: Rs" << order->getTotalAmount() << "\n";
                while (order->getAmountDue() > 0.001) {
                    std::cout << "Due: Rs" << order->getAmountDue() << "\n";
                    double amt       = getDouble("Payment amount (Rs): ");
                    Payment_Mode mode = getPaymentMode();
                    Payment_Type type = Payment_Type::Spot;

                    if (cust->getPartyType() == Party_Type::PermanentCustomer) {
                        std::cout << "  Type: 1=Spot 2=Advance 3=Loan\n";
                        int tc = getInt("  Choice: ");
                        if      (tc == 2) type = Payment_Type::Advance;
                        else if (tc == 3) type = Payment_Type::Loan;
                    }
                    shop.addPaymentToSale(order, amt, mode, type);
                }

                shop.confirmSaleOrder(order);
                std::cout << "Sale confirmed!\n";
                order->Display();

                int ret = getInt("\nReturn an item? (LineItem ID or 0 to skip): ");
                if (ret != 0) {
                    shop.returnSaleItem(order, ret);
                    std::cout << "Item returned.\n";
                }
            }
            else if (c == 2) {
                int custId = getInt("Customer ID: ");
                auto orders = shop.getSaleOrdersByCustomer(custId);
                if (orders.empty()) { std::cout << "No orders found.\n"; continue; }
                for (SaleOrder* o : orders) o->Display();
                int oid  = getInt("Order ID: ");
                SaleOrder* order = nullptr;
                for (SaleOrder* o : orders)
                    if (o->getId() == oid) { order = o; break; }
                if (!order) { std::cout << "Order not found.\n"; continue; }
                int lid = getInt("LineItem ID to return: ");
                shop.returnSaleItem(order, lid);
                std::cout << "Returned.\n";
            }
            else if (c == 3) {
                int custId  = getInt("Customer ID: ");
                Party* cust = shop.findPartyById(custId);
                if (!cust) { std::cout << "Not found.\n"; continue; }
                double amt       = getDouble("Deposit amount (Rs): ");
                Payment_Mode mode = getPaymentMode();
                shop.customerDeposit(cust, amt, mode);
                std::cout << "Deposit recorded.\n";
            }
            else if (c == 4) {
                int custId = getInt("Customer ID: ");
                auto orders = shop.getSaleOrdersByCustomer(custId);
                if (orders.empty()) std::cout << "No orders.\n";
                else for (SaleOrder* o : orders) o->Display();
            }
            else if (c == 5) shop.displayAllSaleOrders();
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Purchase Menu ─────────────────────────────────────────────────────────────
void menuPurchase(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Purchases -----\n"
                  << "1. New Purchase Order\n"
                  << "2. Pay Loan Installment\n"
                  << "3. Supplier Advance Deposit\n"
                  << "4. View Supplier Orders\n"
                  << "5. View All Purchase Orders\n"
                  << "6. View Supplier Ledger\n"
                  << "0. Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            if (c == 1) {
                int suppId = getInt("Supplier ID: ");
                Party* supp = shop.findPartyById(suppId);
                if (!supp) { std::cout << "Not found.\n"; continue; }

                std::cout << "Source: 1=Dealership 2=Distributor\n";
                Source_Type src = (getInt("Choice: ") == 1)
                                  ? Source_Type::Dealership : Source_Type::Distributor;
                std::cout << "Payment: 1=Spot 2=Loan\n";
                Payment_Type pt = (getInt("Choice: ") == 1)
                                  ? Payment_Type::Spot : Payment_Type::Loan;

                PurchaseOrder* order = shop.createPurchaseOrder(supp, src, pt);

                while (true) {
                    std::cout << "\n  1=Add item  2=Done\n";
                    if (getInt("  Choice: ") == 2) break;
                    shop.displayAllVariants();
                    int varId    = getInt("  Variant ID: ");
                    ProductVariant* v = shop.products().getVariantById(varId);
                    if (!v) { std::cout << "  Variant not found.\n"; continue; }
                    int qty      = getInt("  Quantity: ");
                    double price = getDouble("  Buy price per unit (Rs): ");
                    shop.addLineItemToPurchase(order, v, qty, price);
                    std::cout << "  Added. Total: Rs" << order->getTotalAmount() << "\n";
                }

                if (pt == Payment_Type::Spot) {
                    std::cout << "Total: Rs" << order->getTotalAmount() << "\n";
                    Payment_Mode mode = getPaymentMode();
                    shop.makePaymentOnPurchase(order, order->getTotalAmount(), mode);
                }

                std::string batchNo = getString("Batch No (Enter to skip for non-perishable): ");
                DateTime expiry;
                if (!batchNo.empty())
                    expiry = getDate("Expiry date");

                shop.confirmPurchaseOrder(order, batchNo, expiry);
                std::cout << "Purchase confirmed!\n";
                order->Display();
            }
            else if (c == 2) {
                shop.displayAllPurchaseOrders();
                int oid   = getInt("Purchase Order ID: ");
                double amt = getDouble("Payment amount (Rs): ");
                Payment_Mode mode = getPaymentMode();
                shop.payLoanInstallment(oid, amt, mode);
                std::cout << "Payment recorded.\n";
            }
            else if (c == 3) {
                int suppId = getInt("Supplier ID: ");
                double amt = getDouble("Advance amount (Rs): ");
                Payment_Mode mode = getPaymentMode();
                shop.supplierAdvanceDeposit(suppId, amt, mode);
                std::cout << "Advance recorded.\n";
            }
            else if (c == 4) {
                int suppId = getInt("Supplier ID: ");
                auto orders = shop.getPurchaseOrdersBySupplier(suppId);
                if (orders.empty()) std::cout << "No orders.\n";
                else for (PurchaseOrder* o : orders) o->Display();
            }
            else if (c == 5) shop.displayAllPurchaseOrders();
            else if (c == 6) {
                int id = getInt("Supplier ID: ");
                shop.displaySupplierLedger(id);
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Stock Menu ────────────────────────────────────────────────────────────────
void menuStock(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Stock -----\n"
                  << "1. View All Stock\n"
                  << "2. Low Stock Alerts\n"
                  << "3. Expiring Soon\n"
                  << "4. Check Stock for Variant\n"
                  << "0. Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            if      (c == 1) shop.displayStock();
            else if (c == 2) shop.displayLowStock();
            else if (c == 3) shop.displayExpiringBefore(getDate("Show expiring before"));
            else if (c == 4) {
                int vid = getInt("Variant ID: ");
                shop.checkStockForVariant(vid);
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Report Menu ───────────────────────────────────────────────────────────────
void menuReports(Shop& shop)
{
    while (true) {
        std::cout << "\n----- Reports -----\n"
                  << "1.  Sales Summary\n"
                  << "2.  Purchase Summary\n"
                  << "3.  Outstanding Dues\n"
                  << "4.  Stock Report (CSV)\n"
                  << "5.  Expiry Report (CSV)\n"
                  << "6.  Product-wise Sales (CSV)\n"
                  << "7.  Variant-wise Sales (CSV)\n"
                  << "8.  Day-wise Sales (CSV)\n"
                  << "9.  Customer Statement (CSV)\n"
                  << "10. Supplier Statement (CSV)\n"
                  << "11. Export All\n"
                  << "0.  Back\n";
        int c = getInt("Choice: ");
        if (c == 0) break;
        try {
            // Reports that need date range
            if (c == 1 || c == 2 || c == 6 || c == 7 || c == 8 || c == 11) {
                DateTime from = getDate("From date");
                DateTime to   = getDate("To date");
                if      (c == 1)  shop.reports().displaySalesSummary(from, to);
                else if (c == 2)  shop.reports().displayPurchaseSummary(from, to);
                else if (c == 6)  shop.reports().exportProductWiseSalesCSV(
                                      getString("Output file: "), from, to);
                else if (c == 7)  shop.reports().exportVariantWiseSalesCSV(
                                      getString("Output file: "), from, to);
                else if (c == 8)  shop.reports().exportDayWiseSalesCSV(
                                      getString("Output file: "), from, to);
                else if (c == 11) shop.reports().exportAll(
                                      getString("Output folder: "), from, to);
            }
            // Reports that don't need date range
            else if (c == 3)  shop.reports().displayOutstandingDues();
            else if (c == 4)  shop.reports().exportStockReportCSV(getString("Output file: "));
            else if (c == 5)  shop.reports().exportExpiryReportCSV(
                                  getString("Output file: "), getDate("Expiring before"));
            else if (c == 9) {
                int id = getInt("Customer ID: ");
                shop.reports().exportCustomerStatementCSV(getString("Output file: "), id);
            }
            else if (c == 10) {
                int id = getInt("Supplier ID: ");
                shop.reports().exportCompanyStatementCSV(getString("Output file: "), id);
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main()
{
    Shop& shop = Shop::getInstance();
    std::cout << "===== Fertilizer Shop Management System =====\n";

    while (true) {
        std::cout << "\n========== Main Menu ==========\n"
                  << "1. Parties\n"
                  << "2. Products\n"
                  << "3. Sales\n"
                  << "4. Purchases\n"
                  << "5. Stock\n"
                  << "6. Reports\n"
                  << "0. Exit\n";
        int c = getInt("Choice: ");
        if      (c == 0) { std::cout << "Goodbye.\n"; break; }
        else if (c == 1) menuParties(shop);
        else if (c == 2) menuProducts(shop);
        else if (c == 3) menuSale(shop);
        else if (c == 4) menuPurchase(shop);
        else if (c == 5) menuStock(shop);
        else if (c == 6) menuReports(shop);
        else std::cout << "Invalid choice.\n";
    }
    return 0;
}