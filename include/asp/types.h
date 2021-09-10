/**
 * @file   types.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_TYPES_H
#define INCLUDE_ASP_TYPES_H

#include <boost/asio.hpp>
#include <boost/outcome.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/string_view.hpp>

namespace asp {

namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;

template <class R,
          class S = std::error_code,
          class NoValuePolicy = outcome::policy::default_policy<R, S, void>>
using result_t = outcome::result<R, S, NoValuePolicy>;

using string_view_t = boost::string_view;

using po_desc_t = boost::program_options::options_description;
using po_vars_t = boost::program_options::variables_map;

using context_t = boost::asio::io_context;

} // namespace asp

#endif /* INCLUDE_ASP_TYPES_H */
