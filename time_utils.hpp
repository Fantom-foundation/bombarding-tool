#pragma once

#include <stdint.h>
#include <exception>

namespace time_utils
{

class InvalidDateTime : public std::exception
{
public:
    InvalidDateTime() {}

    const char *what() const noexcept override
    {
        return "invalid datetime string";
    }
};

static inline
bool is_leap_year(unsigned year)
{
    if (year % 4 != 0)
        return false;
    if (year % 100 != 0)
        return true;
    if (year % 400 != 0)
        return false;
    return true;
}

static inline
uint64_t cumulative_days_in_years(unsigned from, unsigned to)
{
    uint64_t r = 0;
    for (unsigned y = from; y < to; ++y)
        r += is_leap_year(y) ? 366 : 365;
    return r;
}

static inline
uint64_t cumulative_days_in_months_before(unsigned year, unsigned month)
{
    const int days_in_month[] = {
        0,
        31,
        is_leap_year(year) ? 29 : 28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31,
    };
    int r = 0;
    for (unsigned m = 1; m < month; ++m)
        r += days_in_month[m];
    return r;
}

static inline
uint64_t parse_datetime(const char *s)
{
    const auto slurp_digits = [&](char until) {
        unsigned r = 0;
        while (true) {
            const char c = *s;
            ++s;
            if (c == until)
                break;
            const int digit = static_cast<int>(c) - '0';
            if (digit < 0 || digit > 9) {
                fprintf(stderr, "Symbol: %c (until '%c')\n", c, until);
                throw InvalidDateTime{};
            }
            r = r * 10 + digit;
        }
        return r;
    };

    const auto year = slurp_digits('-');
    const auto month = slurp_digits('-');
    const auto day = slurp_digits('T');
    const auto hour = slurp_digits(':');
    const auto minute = slurp_digits(':');
    const auto second = slurp_digits('.');
    const auto nanosecond = slurp_digits('Z');

    const uint64_t total_days =
        cumulative_days_in_years(2000, year) +
        cumulative_days_in_months_before(year, month) +
        (day - 1);

    uint64_t r = total_days;

    r *= 24;
    r += hour;

    r *= 60;
    r += minute;

    r *= 60;
    r += second;

    r *= 1'000'000'000;
    r += nanosecond;

    return r;
}

}
