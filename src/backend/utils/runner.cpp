//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														runner.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/21/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "pe_context.hpp"
#include <boost/filesystem.hpp>
#include "file_reader.hpp"
#include <iostream>
int main(int, char **) {
    //  test_map();
    for(auto & p :  boost::filesystem::directory_iterator( "/opt/pes/" ))
    {
        std::cout  << p << std::endl;
        trustwave::file_reader fr(p.path().string());
        trustwave::pe_context pc(fr);
        pc.parse();
        pc.showVersion();
    }

        return 0;

}