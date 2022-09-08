#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ql/time/date.hpp>
#include <ql/settings.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/types.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/shared_ptr.hpp>
#include <ql/indexes/ibor/eonia.hpp>
#include <ql/instruments/makeois.hpp>
#include <ql/cashflows/fixedratecoupon.hpp>
#include <ql/cashflows/floatingratecoupon.hpp>
//#include <ql/utilities/null.hpp>

#include <sstream>

namespace py = pybind11;
namespace ql = QuantLib;

PYBIND11_MODULE(quantlib, m)
{
  // QuantLib::Date

  py::class_<ql::Date>(m, "Date")
    .def(py::init<ql::Day, ql::Month, ql::Year>())
    .def(py::init([](ql::Day d, int m, ql::Year y) {
                    return std::unique_ptr<ql::Date>(
                        new ql::Date(d, static_cast<ql::Month>(m), y)); }))
    .def("dayOfMonth", &ql::Date::dayOfMonth)
    .def("month", &ql::Date::month)
    .def("year", &ql::Date::year)
    .def("__lt__", [](const ql::Date &a, const ql::Date &b) { return a < b; })  // operator <
    .def("__repr__",  // string representation
        [](const ql::Date &d) {
          std::stringstream ss;
          ss << d;
          return ss.str();
        });

  // QuantLib::Month

  py::enum_<ql::Month>(m, "Month")
    .value("January", ql::Month::January)
    .value("February", ql::Month::February)
    .value("March", ql::Month::March)
    .value("April", ql::Month::April)
    .value("May", ql::Month::May)
    .value("June", ql::Month::June)
    .value("July", ql::Month::July)
    .value("August", ql::Month::August)
    .value("September", ql::Month::September)
    .value("October", ql::Month::October)
    .value("November", ql::Month::November)
    .value("December", ql::Month::December)
    .value("Jan", ql::Month::Jan)
    .value("Feb", ql::Month::Feb)
    .value("Mar", ql::Month::Mar)
    .value("Apr", ql::Month::Apr)
    .value("Jun", ql::Month::Jun)
    .value("Jul", ql::Month::Jul)
    .value("Aug", ql::Month::Aug)
    .value("Sep", ql::Month::Sep)
    .value("Oct", ql::Month::Oct)
    .value("Nov", ql::Month::Nov)
    .value("Dec", ql::Month::Dec)
    .export_values();

  // QuantLib::Settings

  py::class_<ql::Settings, std::unique_ptr<ql::Settings, py::nodelete>>(m, "Settings")
    .def(py::init([](){ return std::unique_ptr<ql::Settings, py::nodelete>(&ql::Settings::instance()); }))
    .def_property("evaluationDate",
                  [](ql::Settings& self) { return ql::Date(self.evaluationDate()); },
                  [](ql::Settings& self, ql::Date& date) { self.evaluationDate() = date; });

  // QuantLib::DayCounter

  py::class_<ql::DayCounter>(m, "DayCounter")
    .def(py::init<>());

  // QuantLib::Actual365Fixed

  py::class_<ql::Actual365Fixed, ql::DayCounter> actual365fixed(m, "Actual365Fixed");

  py::enum_<ql::Actual365Fixed::Convention>(actual365fixed, "Convention")
    .value("Standard", ql::Actual365Fixed::Standard)
    .value("Canadian", ql::Actual365Fixed::Canadian)
    .value("NoLeap", ql::Actual365Fixed::NoLeap)
    .export_values();

  actual365fixed
    .def(py::init<ql::Actual365Fixed::Convention>(), py::arg("c") = ql::Actual365Fixed::Standard);

  // QuantLib::YieldTermStructure

  // TODO: Can we avoid shared_ptr as holder class (here, and also in other classes)
  py::class_<ql::YieldTermStructure,
             std::shared_ptr<ql::YieldTermStructure>>(m, "YieldTermStructure");

  py::class_<ql::Handle<ql::YieldTermStructure>>(m, "YieldTermStructureHandle")
    .def(py::init<const ql::ext::shared_ptr<ql::YieldTermStructure>&>());

  // QuantLib::InterpolatedZeroCurve<ql::Linear>

  py::class_<ql::InterpolatedZeroCurve<ql::Linear>, ql::YieldTermStructure,
             std::shared_ptr<ql::InterpolatedZeroCurve<ql::Linear>>>
             (m, "ZeroCurve", py::multiple_inheritance())
    .def(py::init([](const std::vector<ql::Date>& d, const std::vector<py::object>& r,
                     const ql::DayCounter& dc) {
                       std::vector<ql::Rate> rates;
                       rates.reserve(r.size());
                       for (auto obj : r) {
#ifdef AADC_QL                        
                         if (py::type::of(obj) == py::type::of<idouble>())
                           rates.push_back(obj.cast<idouble>());
                         else
#endif
                           rates.push_back(obj.cast<double>());
                       }
                       return std::make_unique<ql::InterpolatedZeroCurve<ql::Linear>>(d, rates, dc);
                  }));

  // QuantLib::Period

  py::class_<ql::Period>(m, "Period")
    .def(py::init<ql::Integer, ql::TimeUnit>());

  // QuantLib::TimeUnit

  py::enum_<ql::TimeUnit>(m, "TimeUnit")
    .value("Days", ql::TimeUnit::Days)
    .value("Weeks", ql::TimeUnit::Weeks)
    .value("Months", ql::TimeUnit::Months)
    .value("Years", ql::TimeUnit::Years)
    .value("Hours", ql::TimeUnit::Hours)
    .value("Minutes", ql::TimeUnit::Minutes)
    .value("Seconds", ql::TimeUnit::Seconds)
    .value("Milliseconds", ql::TimeUnit::Milliseconds)
    .value("Microseconds", ql::TimeUnit::Microseconds)
    .export_values();

  // QuantLib::OvernightIndex

  py::class_<ql::OvernightIndex, std::shared_ptr<ql::OvernightIndex>>(m, "OvernightIndex");

  // QuantLib::Eonia

  py::class_<ql::Eonia, ql::OvernightIndex, std::shared_ptr<ql::Eonia>>(m, "Eonia")
    .def(py::init<const ql::Handle<ql::YieldTermStructure>&>());

  // QuantLib::OvernightIndexedSwap

  py::class_<ql::OvernightIndexedSwap>(m, "OvernightIndexedSwap")
    .def("fixedLeg", &ql::OvernightIndexedSwap::fixedLeg)
    .def("overnightLeg", &ql::OvernightIndexedSwap::overnightLeg);

  // QuantLib::MakeOIS

  m.def("MakeOIS",
        [](const ql::Period& swapTenor,
           const ql::ext::shared_ptr<ql::OvernightIndex>& overnightIndex,
           double fixedRate, const ql::Period& fwdStart) {
             return std::make_unique<ql::OvernightIndexedSwap>(ql::OvernightIndexedSwap(
                 ql::MakeOIS(swapTenor, overnightIndex, fixedRate, fwdStart))); },
        py::arg("swapTenor"), py::arg("overnightIndex"),
        py::arg("fixedRate") /*= ql::Null<ql::Rate>*/,
        py::arg("fwdStart") = 0*ql::Days);
  //py::class_<ql::MakeOIS>(m, "MakeOIS")
  //  .def(py::init<const ql::Period&, const ql::ext::shared_ptr<ql::OvernightIndex>&,
  //                ql::Rate, const ql::Period&>(),
  //       py::arg("swapTenor"), py::arg("overnightIndex"),
  //       py::arg("fixedRate") /*= ql::Null<ql::Rate>*/,
  //       py::arg("fwdStart") = 0*ql::Days)
  //  .def("AsOvernightIndexedSwap",
  //       [](const ql::MakeOIS& self) {
  //         return std::make_unique<ql::OvernightIndexedSwap>(ql::OvernightIndexedSwap(self)); });

  // QuantLib::CashFlow

  py::class_<ql::CashFlow, std::shared_ptr<ql::CashFlow>>(m, "CashFlow")
    .def("date", &ql::CashFlow::date)
    .def("amount", &ql::CashFlow::amount);

  // QuantLib::FixedRateCoupon

  py::class_<ql::FixedRateCoupon, ql::CashFlow,
             std::shared_ptr<ql::FixedRateCoupon>>(m, "FixedRateCoupon")
    .def("rate", &ql::FixedRateCoupon::rate)
    .def("accrualStartDate", &ql::FixedRateCoupon::accrualStartDate)
    .def("accrualEndDate", &ql::FixedRateCoupon::accrualEndDate)
    .def("accrualPeriod", &ql::FixedRateCoupon::accrualPeriod);

  m.def("as_fixed_rate_coupon",
        [](ql::ext::shared_ptr<ql::CashFlow>& cf) {
          return ql::ext::dynamic_pointer_cast<ql::FixedRateCoupon>(cf);
        });

  // QuantLib::FloatingRateCoupon

  py::class_<ql::FloatingRateCoupon, ql::CashFlow, std::shared_ptr<ql::FloatingRateCoupon>>
             (m, "FloatingRateCoupon", py::multiple_inheritance())
    .def("rate", &ql::FloatingRateCoupon::rate)
    .def("accrualStartDate", &ql::FloatingRateCoupon::accrualStartDate)
    .def("accrualEndDate", &ql::FloatingRateCoupon::accrualEndDate)
    .def("accrualPeriod", &ql::FloatingRateCoupon::accrualPeriod);

  m.def("as_floating_rate_coupon",
        [](ql::ext::shared_ptr<ql::CashFlow>& cf) {
          return ql::ext::dynamic_pointer_cast<ql::FloatingRateCoupon>(cf);
        });
}
