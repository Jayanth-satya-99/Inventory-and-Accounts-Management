#include "DateTime.hpp"
DateTime DateTime::Now()
{
    return DateTime(std::time(nullptr));
}
DateTime DateTime::FromDateString(const std::string &s)
{
    std::tm tm{};
    std::istringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail())
        throw std::invalid_argument("DateTime: bad date string: " + s);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    return DateTime(std::mktime(&tm));
}
DateTime DateTime::FromString(const std::string &s)
{
    std::tm tm{};
    std::istringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail())
        throw std::invalid_argument("DateTime: bad datetime string: " + s);
    tm.tm_isdst = -1;
    return DateTime(std::mktime(&tm));
}
std::time_t DateTime::epoch() const
{
    return epoch_;
}
std::string DateTime::ToDateString() const
{
    if (epoch_ == 0)
        return "not set";
    std::tm *tm = std::localtime(&epoch_);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}
std::string DateTime::ToString() const
{
    if (epoch_ == 0)
        return "not set";
    std::tm *tm = std::localtime(&epoch_);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
bool DateTime::IsSet() const
{
    return epoch_ != 0;
}
int DateTime::DaysFromNow() const
{
    double d = std::difftime(epoch_, std::time(nullptr));
    return static_cast<int>(d / 86400.0);
}

bool DateTime::operator==(const DateTime &o) const { return epoch_ == o.epoch_; }
bool DateTime::operator!=(const DateTime &o) const { return epoch_ != o.epoch_; }
bool DateTime::operator<(const DateTime &o) const { return epoch_ < o.epoch_; }
bool DateTime::operator<=(const DateTime &o) const { return epoch_ <= o.epoch_; }
bool DateTime::operator>(const DateTime &o) const { return epoch_ > o.epoch_; }
bool DateTime::operator>=(const DateTime &o) const { return epoch_ >= o.epoch_; }
