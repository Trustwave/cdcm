//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wql.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : rfrenkel
// Date    : 11/5/2019
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "wql.hpp"

#include <tuple>
#include <string>
#include <chrono> //rotem added to delete
#include <boost/tokenizer.hpp>

#include "../wmi_wql_client.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
namespace {
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

// trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

// trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }
}

using trustwave::WMI_WQL_Action;
using action_status = trustwave::Action_Base::action_status;
tao::json::value wql_resp_to_json_value(std::string work_str) {

    boost::char_separator<char> end_of_line_delim("\n");
    boost::char_separator<char> keys_delim("|");

    //tokenized_response will hold the parsed response as a table.
    // at the first row there will be only the keys names.
    // the rest of the rows will hold only the values of the key, matching by the index:
    // | x | y |
    // | 1 | 2 |
    // means x=1, y=2
    std::vector<std::vector<std::string>> tokenized_response;

    // split the response into rows by the '\n' delimiter.
    // each row, parse by the '|' delimiter
    boost::tokenizer<boost::char_separator<char>> rows_tokens(work_str, end_of_line_delim);
    int row_number = 0;
    for (auto row : rows_tokens)
    {
        tokenized_response.push_back( std::vector<std::string>() );
        boost::tokenizer<boost::char_separator<char>> keys_tokens(row, keys_delim);
        for (auto key : keys_tokens)
        {
            tokenized_response[row_number].push_back(key);
        }
        ++row_number;
    }


    std::cout << "vec.rows: " << tokenized_response.size() << std::endl;
    for (auto row : tokenized_response)
    {
        std::cout << "num of elements: " << row.size() << std::endl;
    }

    for (auto row: tokenized_response)
    {
        for (auto column : row)
        {
            std::cout << column << " || ";
        }
        std::cout << std::endl;
    }



    tao::json::events::to_value consumer;
    consumer.begin_array();
    for (std::vector<std::vector<std::basic_string<char>>>::size_type i = 1; i< tokenized_response.size(); ++i)
    {
        consumer.begin_object();
        for (std::vector<std::basic_string<char> >::size_type j=0; j< tokenized_response[i].size(); ++j )
        {
            trim(tokenized_response[0][j]);
            if(!tokenized_response[0][j].empty()) {
                std::cout << j << " inserting pair: " << tokenized_response[0][j] << " : " << tokenized_response[i][j]
                          << std::endl;
                consumer.key(tokenized_response[0][j]);
                trim(tokenized_response[i][j]);
                consumer.string(tokenized_response[i][j]);
                consumer.member();
            }
        }
        consumer.end_object();
        consumer.element();
    }

    consumer.end_array();
    const tao::json::value json_value = std::move( consumer.value );

    return (std::move(json_value));
}



/********************************************************
 *
 *  return value: -1 for error, 0 for success
 *********************************************************/
action_status WMI_WQL_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    if (!sess || (sess && sess->id().is_nil())){
        AU_LOG_ERROR("Session not found");
        res->res("Error: Session not found");
        return action_status::FAILED;
    }

    auto wmi_wql_action = std::dynamic_pointer_cast<wmi_action_wql_query_msg>(action);
    if (!wmi_wql_action){
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: Internal error");
        return action_status::FAILED;
    }

    trustwave::wmi_wql_client client;

    //rotem: TODO: think how to distingush between our error and legit error
    auto connect_result = client.connect(*sess, wmi_wql_action->wmi_namespace);
    if (false == std::get<0>(connect_result) )
    {
        AU_LOG_ERROR("failed to connect to the asset");
        res->res(std::string("Error: Failed to connect to the asset"));
        return action_status::FAILED;
    }

    auto query_result = client.query_remote_asset(wmi_wql_action->wql);
    if (false == std::get<0>(query_result) )
    {
        AU_LOG_ERROR("failed to get wql response");
        res->res(std::string(std::get<1>(query_result)));
        return action_status::FAILED;
    }

    std::string wql_raw_response = std::get<1>(query_result);
    auto wql_resp_json_value = wql_resp_to_json_value(wql_raw_response);
    res->res(wql_resp_json_value);
    return action_status::SUCCEEDED;
}


// instance of the our plugin
static std::shared_ptr<WMI_WQL_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action() {
    return instance ? instance : (instance = std::make_shared<WMI_WQL_Action>());
}