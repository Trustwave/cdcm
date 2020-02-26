//===========================================================================
// Trustwave ltd. @{SRCH}
//								typedefs.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 5 Aug 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_TYPEDEFS_HPP_
#define TRUSTWAVE_COMMON_TYPEDEFS_HPP_

namespace trustwave {

    struct process_type {
        struct broker;
        struct worker;
        struct supervisor;
    };

    static constexpr std::string_view conf_root("/etc/cdcm/");

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_TYPEDEFS_HPP_ */
