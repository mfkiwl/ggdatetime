///
/// @file  datetime_write.hpp
///
/// @brief Function to format dso::datetime objects as strings
///
/// @see dso::datetime
///
/// @author xanthos
///
/// @bug No known bugs.
///

#ifndef __NGPT_DT_WRITERS__
#define __NGPT_DT_WRITERS__

#include "dtcalendar.hpp"
#include "dtfund.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace dso {

/// Format a double to an std::string using std::fixed and a precision on
/// n digits
inline std::string _d2s_(double d, int n = 2) noexcept {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(n) << d;
  return stream.str();
}

/// Format a double to an std::string with a width of w chars
inline std::string _i2s_(int i, int w = 2) noexcept {
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(w) << i;
  return stream.str();
}

/// @brief Format as YYYY-MM-DD
///
/// Read and return a date from a c-string of type: YYYY-MM-DD, where the
/// delimeters can be whatever (but something, i.e. two numbers must be
/// seperated by some char -- 20150930 is wrong --).
/// Hours, minutes and seconds are set to 0.
/// If the argument stop is passed, it will be set to the last character (of
/// str) interpreted.
///
/// @throw std::invalid_argument if the input string cannot be resolved.
template <typename T, Timescale TS>
std::string strftime_ymd_hmfs(const datetime<T,TS> &t, char del = '-') {
  auto ymd = t.as_ymd();
  auto hmsf = t.as_hmsf();

  double secs = std::get<2>(hmsf).as_underlying_type() +
                std::get<3>(hmsf) / T::template sec_factor<double>();

  return _i2s_((ymd.__year).as_underlying_type(), 4) + del +
         _i2s_((ymd.__month).as_underlying_type(), 2) + del +
         _i2s_((ymd.__dom).as_underlying_type(), 2) + ' ' +
         _i2s_(std::get<0>(hmsf).as_underlying_type(), 2) + ':' +
         _i2s_(std::get<1>(hmsf).as_underlying_type(), 2) + ':' +
         _d2s_(secs, 5);
}
template <typename T,TimeScale TS>
std::string strftime_ymd_hms(const datetime<T,TS> &t, char del = '-') {
  auto ymd = t.as_ymd();
  auto hmsf = t.as_hmsf();

  return _i2s_((ymd.__year).as_underlying_type(), 4) + del +
         _i2s_((ymd.__month).as_underlying_type(), 2) + del +
         _i2s_((ymd.__dom).as_underlying_type(), 2) + ' ' +
         _i2s_(std::get<0>(hmsf).as_underlying_type(), 2) + ':' +
         _i2s_(std::get<1>(hmsf).as_underlying_type(), 2) + ':' +
         _i2s_(std::get<2>(hmsf).as_underlying_type(), 2);
}

} // namespace dso

#endif
