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
#include <codecvt>
int main(int, char **) {
    //  test_map();
    for(auto & p :  boost::filesystem::directory_iterator( "/opt/pes/" ))
    {
        std::cout  << p << std::endl;
        trustwave::file_reader fr(p.path().string());
        trustwave::pe_context pc(fr);
        pc.parse();
        std::map<std::u16string,std::u16string> ret;
        pc.extract_info(ret);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        for(const auto& a:ret) {
            std::cerr << convert.to_bytes(std::u16string(a.first)) << " : "
                      << convert.to_bytes(std::u16string(a.second)) << std::endl;
        }
    }

        return 0;

}