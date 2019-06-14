//===========================================================================
// Trustwave ltd. @{SRCH}
//								Threads.h
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
#ifndef COMMON_THREADS_H
#define COMMON_THREADS_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <atomic>
#include <thread>

namespace trustwave {
//===========================================================================
// @{CSEH}
//								Thread
// 
//---------------------------------------------------------------------------
// Description    : A class that represents a particular thread activity.
//  The solution is to tightly couple a thread to a class object; Make the 
//  lifetime of the thread the lifetime of the object. Make the interface 
//  to the class the interface to the thread.
//
//  The thread is started when the object is completely constructed.
//  The thread is stopped before the object is destroyed.
//  The thread is encapsulated inside the class.
//===========================================================================
class Thread {
public:
    Thread();
    virtual ~Thread();

    Thread(Thread const&) = delete;
    Thread& operator =(Thread const&) = delete;
    Thread(Thread &&obj) noexcept;
    Thread& operator =(Thread &&obj) noexcept;

    void stop();
    void start();

protected:
    void do_task();

    virtual bool on_start_task() { return true;}
    virtual bool on_end_task() { return true;}
    virtual void task() = 0;

// data members    
protected:
    std::atomic<bool> stop_; // usally used in the inhereted 
                             // class in the busy loop

private:
    std::thread thread_;
};
//===========================================================================
//===========================    Implementation    ==========================
//===========================================================================
// @{FUNH}
//                              Thread()
//
//---------------------------------------------------------------------------
// Description: 
// Default constructor
//===========================================================================
Thread::Thread() : stop_(false), thread_() {
}
//===========================================================================
// @{FUNH}
//                              ~Thread()
//
//---------------------------------------------------------------------------
// Description: 
// Default destructor. If the thread is running stop it.
//===========================================================================
Thread::~Thread() { 
	try { 
		stop(); 
	} catch(...) { 
		/* Logging */ 
	} 
}
//===========================================================================
// @{FUNH}
//                              Thread()
//
//---------------------------------------------------------------------------
// Description: 
// Move constructor. Since you're moving obj, no one else will access it. 
// So reading from its stop_ is safe, wether it's atomic or not.
//===========================================================================
Thread::Thread(Thread &&obj) noexcept : 
	stop_(obj.stop_.load()),
	thread_(std::move(obj.thread_)) {
}
//===========================================================================
// @{FUNH}
//                              Thread()
//
//---------------------------------------------------------------------------
// Description: 
// Move operator =. Since you're moving obj, no one else will access it. 
// So reading from its stop_ is safe, wether it's atomic or not.
//===========================================================================
Thread& Thread::operator=(Thread &&obj) noexcept {
  stop_ = obj.stop_.load();
  thread_ = std::move(obj.thread_);
  return *this;
}
//===========================================================================
// @{FUNH}
//                              stop()
//
//---------------------------------------------------------------------------
// Description: 
// waiting for the thread to finish. and sets the stop_ flag to true, so if 
// the thread is in a wait loop it can take the stop flag and close nicely
//===========================================================================
void Thread::stop() { 
	stop_ = true;
	if (false == thread_.joinable()) return;
	thread_.join(); 
}
//===========================================================================
// @{FUNH}
//                              start()
//
//---------------------------------------------------------------------------
// Description: 
// If the thread is joinable (not started) then start a thread. by the way 
// verify that the stop_ flag is true if some client play with stop start
//===========================================================================
void Thread::start() { 
	if (true == thread_.joinable()) return;
	stop_ = false; 
	thread_ = std::thread(&Thread::do_task, this); 
}
//===========================================================================
// @{FUNH}
//                              do_task()
//
//---------------------------------------------------------------------------
// Description: 
// The entry point of the new thread. make sure that the thread does not 
// throw an exception. at the end call the pure virtual call task. One can 
// take advantage of the on_start_task for thread initialization and 
// on_end_task for thread deinitialization.
//===========================================================================
void Thread::do_task() {
	if(on_start_task()) {
		try {
			task();
		}
		catch (std::exception& ) { 
			// Logging 
		} 
		catch (...) {
			// Logging 
		}
	}
	on_end_task();
}

}// namespace MSC
#endif //COMMON_THREADS_H
