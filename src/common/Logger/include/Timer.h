//===========================================================================
// Trustwave ltd. @{SRCH}
//								                  Timers.h
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef COMMON_TIMERS_H
#define COMMON_TIMERS_H
//===========================================================================
//                Include Files.
//===========================================================================
#include <functional>
#include <memory>

namespace trustwave {

    //===========================================================================
    // @{CSEH}
    //                                Timer
    //
    //---------------------------------------------------------------------------
    // Description    : An interface to a Timer
    //===========================================================================
    class Timer {
    public:
        virtual ~Timer() = default;

        virtual bool start(unsigned long interval) noexcept = 0;
        virtual bool stop() noexcept = 0;
    };
    //===========================================================================
    // @{CSEH}
    //                                ManageTimer
    //
    //---------------------------------------------------------------------------
    // Description    : Manage a timer on multiplatform enviroment.
    //===========================================================================
    class ManageTimer {
    public:
        ManageTimer(bool loop, std::function<void(void)> fn);
        virtual ~ManageTimer() = default;

        ManageTimer(ManageTimer const&) = delete;
        ManageTimer& operator=(ManageTimer const&) = delete;
        ManageTimer(ManageTimer&& obj) noexcept = delete;
        ManageTimer& operator=(ManageTimer&& obj) noexcept = delete;

        bool start(unsigned long interval) noexcept;
        bool stop() noexcept;

        bool set_interval(unsigned long interval) noexcept;

    private:
        std::unique_ptr<Timer> concreate_timer_;
    };

} // namespace trustwave
#endif // COMMON_TIMERS_H
