#pragma once
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdexcept>

class DateTime
{
private:
    std::time_t epoch_;

public:
    DateTime() : epoch_(0) {} // Dafault consturctor       DateTime d;
                              // DateTime(){epoch_=0;}   this is also write but this first creates and then assigns 0
    explicit DateTime(std::time_t epoch) : epoch_(epoch) {} // constructor  DateTime d(x); or DateTime d=DateTime(x);
    // explicit is used to tell the compiler that do not accidentally convert other data type like int to time_t

    static DateTime Now();                               // DateTime d=DateTime::Now();
    static DateTime FromDateString(const std::string &); // DateTime d=DateTime::FromDateString("YYYY-MM-DD");
    static DateTime FromString(const std::string &);     // DateTime d=DateTime::FromString("YYYY-MM-DD HH-MM-SS");

    std::time_t epoch() const;        // getter //time expected in epoch
    std::string ToString() const;     // getter //time expected in YYYY-MM-DD HH-MM-SS
    std::string ToDateString() const; // getter //time expected in YYYY-MM-DD
    bool IsSet() const;               // check weatehr time is set tell weatehr a value of time is assigned
    int DaysFromNow() const;          // given a DateTime d finds the number of days between

    bool operator==(const DateTime &o) const;
    bool operator!=(const DateTime &o) const;
    bool operator<(const DateTime &o) const;
    bool operator<=(const DateTime &o) const;
    bool operator>(const DateTime &o) const;
    bool operator>=(const DateTime &o) const;
};
