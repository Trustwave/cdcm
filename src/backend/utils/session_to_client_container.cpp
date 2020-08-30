//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														session_to_client_container.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/27/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "session_to_client_container.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"

using trustwave::sessions_to_clients;

void sessions_to_clients::get_clients_for_session(const std::string& session_id)
{
        auto it = session_to_clients_.find(session_id);
        if(it == session_to_clients_.end()) {
            it = session_to_clients_.emplace(session_id, std::make_unique<clients_map>(ios_,*this)).first;
        }
}
//void sessions_to_clients::set_clients_map(clients_map* the_clients_map)
//{
//    clients_=the_clients_map;
//}
void sessions_to_clients::set_client(std::shared_ptr<cdcm_client> c,const std::string& session_id,size_t idle_client_timeout)
{
    auto it = session_to_clients_.find(session_id);
    if(it != session_to_clients_.end()) { it->second->set_client(c, idle_client_timeout); }
}

trustwave::Dispatcher<trustwave::cdcm_client> & sessions_to_clients::get_clients_dispatcher()
{
    return clients_map::dispatcher();
}
trustwave::Dispatcher<trustwave::process_specific_object>::Registrator
    sessions_to_clients::m_registrator(new sessions_to_clients(authenticated_scan_server::instance().io_context()), authenticated_scan_server::instance().process_specific_repository());
