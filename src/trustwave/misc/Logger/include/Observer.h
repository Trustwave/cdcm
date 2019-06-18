//===========================================================================
// Trustwave ltd. @{SRCH}
//								Observer.h
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
// Subscriber Pattern implimentation
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef COMMON_OBSERVER_H
#define	COMMON_OBSERVER_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <list>
#include <algorithm>

namespace trustwave {
namespace Patterns {
//===========================================================================
//								Forword declaration.
//===========================================================================
class Publisher;
//===========================================================================
// @{CSEH}
//								Subscriber
// 
//---------------------------------------------------------------------------
// Description    : 
//
//===========================================================================
class Subscriber {
public:
	virtual ~Subscriber() { }
	
	virtual void update_subscriber(Publisher* changed_publisher, long hint=0) = 0;

protected:
	Subscriber() { }
};
//===========================================================================
// @{CSEH}
//								Publisher
// 
//---------------------------------------------------------------------------
// Description    : 
//
//===========================================================================
class Publisher {
	using subscriber_t = std::list<Subscriber*>;

protected:
	Publisher() { }

public:
	virtual ~Publisher() { }

    Publisher(Publisher const&) = delete;
    Publisher& operator =(Publisher const&) = delete;
	Publisher(Publisher &&obj) noexcept = delete;
	Publisher& operator=(Publisher &&obj) noexcept = delete;

	bool add_subscriber(Subscriber *o) {
		subscribers_.push_back(o);		
		return true;
	}

	bool remove_subscriber(Subscriber* o) {
		auto it=std::find(subscribers_.begin(),subscribers_.end(),o);
		
		bool found_subscriber = (it != subscribers_.end());
		if(found_subscriber) {
			subscribers_.erase(it,it);
		}
		return found_subscriber;
	}

	void notify_change(long hint=0)
	{
		for( auto s : subscribers_ ) {
			s->update_subscriber(this, hint);
		}
	}

private:
	subscriber_t subscribers_;
};

} // Patterns
} // MSC

#endif // COMMON_OBSERVER_H