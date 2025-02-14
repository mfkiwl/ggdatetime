#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include "datetime_read.hpp"
#include "dtcalendar.hpp"
#include "dtfund.hpp"

using namespace dso;
template<typename S>
using Datetime = datetime<S,TimeScale::TAI>;

constexpr long MilliSec = 1000L;
constexpr long MicroSec = 1000000L;
constexpr long NanoSec = 1000000000L;
constexpr long maxL = std::numeric_limits<long int>::max();

// This is a bit tricky ....
// Suppose you have a template function, based on any sec-type parameter (e.g.
// seconds, milliseconds, etc) in which you want to call the (static) function
// S::sec_factor<T>(). To do this, you need to implement the function as follows
template <typename S> long foo() { return S::template sec_factor<long>(); }

int main() {
  //
  // Implementation on this arch.
  // -----------------------------------------------------------------------
  //
  std::cout << "\nTesting implementation for this architecture.\n";
  std::cout
      << "-------------------------------------------------------------\n";
  std::cout << "Size of datetime class is " << sizeof(Datetime<seconds>)
            << " bytes.\n";
  std::cout << "Max representable (long) int is " << maxL << "\n";
  static_assert(86400L * MilliSec < maxL, "-- MilliSeconds Overflow --");
  static_assert(86400L * MicroSec < maxL, "-- MicroSeconds Overflow --");
  static_assert(86400L * NanoSec < maxL, "-- NanoSeconds Overflow --");
  static_assert(86400L * NanoSec * 7 < maxL, "-- GPS NanoSeconds Overflow --");
  std::cout << "A whole day fits in long for:\n";
  std::cout << "\tMilliseconds: 86400 * 1000       = " << 86400 * MilliSec
            << ", can fit about " << maxL / (86400 * MilliSec) << " days\n";
  std::cout << "\tMicroseconds: 86400 * 1000000    = " << 86400 * MicroSec
            << ", can fit about " << maxL / (86400 * MicroSec) << " days\n";
  std::cout << "\tNanoseconds:  86400 * 1000000000 = " << 86400 * NanoSec
            << ", can fit about " << maxL / (86400 * NanoSec) << " days\n";
  std::cout << "\tA whole (GPS) week in Nanoseconds:  7 * 86400 * 1000000000 = "
            << 86400 * NanoSec * 7 << ", can fit about "
            << maxL / (86400 * NanoSec * 7) << " weeks\n";

  std::cout << "Part A -- OK\n\n";

  //
  // Let's try the user defined literals (these need namespace dso)
  // -----------------------------------------------------------------------
  //
  std::cout << "Testing constructors & user-defined literals\n";
  std::cout
      << "-------------------------------------------------------------\n";
  auto yr1 = 2016_Y; // year  = 2016
  year yr2{2016};
  assert(yr1 == yr2);
  auto mt1 = 12_M; // month = 12
  month mt2{12};
  assert(mt1 == mt2);
  auto dm1 = 30_D; // day of month = 30
  day_of_month dm2{30};
  assert(dm1 == dm2);
  auto hr1 = 12_h; //
  hours hr2{12};
  assert(hr1 == hr2);
  auto mn1 = 59_m; //
  minutes mn2{59};
  assert(mn1 == mn2);
  constexpr auto ss1 = 30_sec;            //
  constexpr auto sm1 = 30000_millisec;    //
  constexpr auto sm2 = 30000000_microsec; //
  static_assert(sm1 == (milliseconds)sm2, "-- Terribly Wrong --");
  static_assert(ss1 == (seconds)sm1 && ss1 == (seconds)sm2,
                "-- Terribly Wrong --");
  std::cout << "Part B -- OK\n\n";

  //
  // Let's try reading dates
  // -----------------------------------------------------------------------
  //
  std::cout << "Testing datetime reading functions\n";
  std::cout
      << "-------------------------------------------------------------\n";
  /*char* __end = nullptr;*/
  const char *date1_str = "2015-12-30 12:09:30";
  const char *date2_str = "2015/12/30 12:09:30";
  const char *date3_str = "2015-12-30 12:09:30.000011";
  const char *date4_str = "2015 12 30 12 9 30";
  Datetime<seconds> dfs1 =
      dso::strptime_ymd_hms<seconds,TimeScale::TAI>(date1_str /*, &__end*/);
  Datetime<seconds> dfs2 =
      dso::strptime_ymd_hms<seconds,TimeScale::TAI>(date2_str /*, &__end*/);
  Datetime<seconds> dfs3 =
      dso::strptime_ymd_hms<seconds,TimeScale::TAI>(date3_str /*, &__end*/);
  Datetime<seconds> dfs4 =
      dso::strptime_ymd_hms<seconds,TimeScale::TAI>(date4_str /*, &__end*/);
  Datetime<seconds> dfs1_{year(2015), month(12),  day_of_month(30),
                          hours(12),  minutes(9), seconds(30)};
  assert(dfs1 == dfs1_ && dfs1 == dfs2 && dfs2 == dfs3 && dfs3 == dfs4);

  auto dfs5 = dso::strptime_ymd_hms<microseconds,TimeScale::TAI>(date3_str);
  Datetime<microseconds> dfs5_{year(2015), month(12),  day_of_month(30),
                               hours(12),  minutes(9), microseconds(30000011)};
  assert(dfs5 == dfs5_);

  const char *date6_str = "2015-12-30";
  Datetime<seconds> dfs6 = dso::strptime_ymd<seconds,TimeScale::TAI>(date6_str);
  const char *date7_str = "2015-12-30 0 0 0";
  Datetime<seconds> dfs7 = dso::strptime_ymd<seconds,TimeScale::TAI>(date7_str);
  Datetime<seconds> dfs7_{year(2015), month(12),  day_of_month(30),
                          hours(0),   minutes(0), seconds(0)};
  assert(dfs6 == dfs7 && dfs7 == dfs7_);

  const char *date8_str = "2015 Dec 30 12 9 30";
  const char *date9_str = "2015/DEC/30 12 9 30";
  const char *date10_str = "2015-DEC-30 12 9 30";
  const char *date11_str = "2015-DEC-30 12 09 30";
  Datetime<seconds> dfs8 = dso::strptime_yod_hms<seconds,TimeScale::TAI>(date8_str);
  Datetime<seconds> dfs9 = dso::strptime_yod_hms<seconds,TimeScale::TAI>(date9_str);
  Datetime<seconds> dfs10 = dso::strptime_yod_hms<seconds,TimeScale::TAI>(date10_str);
  Datetime<seconds> dfs11 = dso::strptime_yod_hms<seconds,TimeScale::TAI>(date11_str);
  assert(dfs8 == dfs9 && dfs9 == dfs10);
  assert(dfs10 == dfs11);
  assert(dfs1 == dfs1_ && dfs1 == dfs8 && dfs8 == dfs9 && dfs9 == dfs10);

  std::cout << "Part C -- OK\n\n";

  std::cout << "Testing Micro- Mill- and Seconds & casting\n";
  std::cout
      << "-------------------------------------------------------------\n";
  //
  // Seconds, MicroSec and Millisec
  // -----------------------------------------------------------------------
  //
  seconds sec1{10};
  assert(sec1 == seconds(10));
  milliseconds mlsec1{10};
  assert(mlsec1 == milliseconds(10));
  microseconds mcsec1{10};
  assert(mcsec1 == microseconds(10));
  seconds sec2{mcsec1}; // casting microsec to sec is allowed
  assert(sec2 == (dso::cast_to<microseconds, seconds>(mcsec1)));
  seconds sec3{mlsec1}; // casting millisec to sec is allowed
  assert(sec3 == (dso::cast_to<milliseconds, seconds>(mlsec1)));
  milliseconds mlsec2{mcsec1}; // casting microsec to millisec is allowed
  assert(mlsec2 == (dso::cast_to<microseconds, milliseconds>(mcsec1)));
  // However, it is not allowed to cast from lower to higher precsision, e.g.
  // milliseconds ml1 {sec1};   // ERROR!
  // microseconds mc1 {mlsec1}; // ERROR!
  assert(foo<milliseconds>() == 1000);
  seconds _s1 = cast_to<microseconds, seconds>(mcsec1);
  assert(_s1 == seconds{0});
  _s1 = cast_to<milliseconds, seconds>(mlsec1);
  assert(_s1 == seconds{0});
  milliseconds _m1 = cast_to<microseconds, milliseconds>(mcsec1);
  assert(_m1 == milliseconds{0});
  _s1 = cast_to<milliseconds, seconds>(milliseconds{2345L});
  assert(_s1 == seconds{2});
  _s1 = cast_to<microseconds, seconds>(microseconds{23456789L});
  assert(_s1 == seconds{23});
  _m1 = cast_to<microseconds, milliseconds>(microseconds{12345678L});
  assert(_m1 == milliseconds{12345});
  _s1 = 12345;
  _m1 = cast_to<seconds, milliseconds>(_s1);
  assert(_m1 == milliseconds{12345000});
  auto _cs = cast_to<seconds, microseconds>(_s1);
  assert(_cs == microseconds{12345000000});
  _m1 = 12345;
  _cs = cast_to<milliseconds, microseconds>(_m1);
  assert(_cs == microseconds{12345000});
  std::cout << "Part D -- OK\n\n";

  std::cout << "Testing Datetime Constructors (and arg deduction)\n";
  std::cout
      << "-------------------------------------------------------------\n";
  //
  // Construction of datetime objects
  // -----------------------------------------------------------------------
  //
  Datetime<seconds> d2(year(2015), month(12), day_of_month(30));
  std::cout << "d2  = " << d2.stringify() << " (" << d2.sec_as_i() << ")\n";

  // the following won't compile; the template parameter can be
  // seconds, milliseconds or microseconds.
  // datetime<year> d3(year(2015), month(12), day_of_month(30)); // ERROR

  // this is fine; microseconds to seconds is allowed (BUT fractional sec
  // are ignored!)
  Datetime<seconds> d21(year(2015), month(12), day_of_month(30),
                        milliseconds(MilliSec));
  std::cout << "d21 = " << d21.stringify() << " (" << d21.sec_as_i() << ")\n";
  // the opposite however id not allowed!
  // datetime<microseconds> d5(year(2015), month(12), day_of_month(30),
  //     seconds(100)); ERROR!
  // we can also use time (i.e. hours, minutes, etc..)
  Datetime<seconds> d22(year(2015), month(12), day_of_month(30), hours(12),
                        minutes(50), seconds(30));
  std::cout << "d22 = " << d22.stringify() << " (" << d22.sec_as_i() << ")\n";
  // or
  Datetime<seconds> d23(year(2015), month(12), day_of_month(30), hours(12),
                        minutes(50), microseconds(30000001));
  std::cout << "d23 = " << d23.stringify() << " (" << d23.sec_as_i() << ")\n";
  // or
  Datetime<microseconds> d24(year(2015), month(12), day_of_month(30), hours(12),
                             minutes(50), microseconds(30000001));
  std::cout << "d24 = " << d24.stringify() << " (" << d24.sec_as_i() << ")\n";
  // but not (seconds cannot be cast to milliseconds) ERROR!
  // datetime<milliseconds> d25 (year(2015), month(12), day_of_month(30),
  // hours(12),
  //     minutes(50), seconds(30000001));
  // std::cout << "d25 = " << d25.stringify() << " (" << d25.secs() << ")\n";

  // this is fine; use fractional seconds (which are skipped!)
  Datetime<seconds> d3(year(2015), month(12), day_of_month(30), hours(12),
                       minutes(50), 30.001234);
  std::cout << "d3  = " << d3.stringify() << " (" << d3.sec_as_i() << ")\n";
  // or, for bigger accuracy ..
  Datetime<microseconds> d31(year(2015), month(12), day_of_month(30), hours(12),
                             minutes(5), 30.0000010);
  std::cout << "d31 = " << d31.stringify() << " (" << d31.sec_as_i() << ")\n";
  std::cout << "Part E -- OK\n\n";

  std::cout << "Testing Datetime Resolution\n";
  std::cout
      << "-------------------------------------------------------------\n";
  //
  // Manipulation of datetime objects
  // -----------------------------------------------------------------------
  //
  d2.add_seconds(seconds(10));
  // remember datetime<seconds> d2(year(2015), month(12), day_of_month(30));
  auto d2_copy = d2;

  std::cout << "\n\nSequentialy adding seconds to a date.\n";
  std::chrono::steady_clock::time_point begin, end;
  double mjd1 = d2.as_mjd(), mjd2;

  std::cout << "d2: " << d2.stringify() << ", MJD = " << d2.as_mjd() << "\n";
  begin = std::chrono::steady_clock::now();
  for (int i = 0; i < 86400 * 2.5; ++i) { /* sequentialy add 2+1/2 days */
    d2.add_seconds(seconds(1L));
  }
  end = std::chrono::steady_clock::now();
  mjd2 = d2.as_mjd();
  std::cout << "Adding 2+1/2 days to d2 takes about "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     begin)
                   .count()
            << " microsec.\n";
  std::cout << "New mjd is " << mjd2 << "\n";
  std::cout << "Difference = " << mjd2 - mjd1 << " days, or "
            << (mjd2 - mjd1) * 86400e0 << " seconds";
  assert(std::abs(mjd2 - mjd1 - 2.5e0) < 1e-15);
  std::cout << " or " << (mjd2 - mjd1) * 86400000e0 << " milliseconds.\n";
  assert(std::abs((mjd2 - mjd1) * 86400e0 - (2.5e0 * 86400e0)) < 1e-15);
  std::cout << "The folowing number should be zero: "
            << ((mjd2 - mjd1) * 86400000.0 - 2.5 * 86400000.0) << ", is it? "
            << std::boolalpha
            << (((mjd2 - mjd1) * 86400000.0 - 2.5 * 86400000.0) == 0.0e0)
            << "\n";
  std::cout << "d2: " << d2.stringify() << ", MJD = " << d2.as_mjd() << "\n";

  // go back to initial epoch d2 by removing seconds
  std::cout << "\nd2: " << d2.stringify() << ", MJD = " << d2.as_mjd();
  std::cout << "\nd:  " << d2_copy.stringify()
            << ", MJD = " << d2_copy.as_mjd();
  // delta_sec(datetime<seconds>-datetime<seconds>) -> seconds, aka
  // sdif is seconds
  auto sdif = delta_sec(d2, d2_copy);
  std::cout << "\nDifference in seconds between d2 and original date is: "
            << sdif.as_underlying_type();
  std::cout << "\nLet's go back by subtracting seconds";
  d2.remove_seconds(sdif);
  std::cout << "d2: " << d2.stringify() << ", MJD = " << d2.as_mjd() << "\n";
  assert(d2 == d2_copy);

  std::cout << "\nAdd and then subtract random seconds from dates";
  long max_sec = 7 * 86400; // one week in seconds
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<long> uni(-max_sec, max_sec);
  for (int i = 0; i < 1000; i++) {
    seconds rand_sec{uni(rng)};
    d2.add_seconds(rand_sec);
    d2.remove_seconds(rand_sec);
    assert(d2 == d2_copy);
  }
  std::cout
      << "\nAdded/Subtracted 1000 times randoms seconds; everything looks ok!";

  std::cout << "\nPart E -- OK\n\n";

  std::cout << "\n";
  return 0;
}
