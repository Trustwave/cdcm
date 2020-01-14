//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														my_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 10/6/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <iostream>
#include <vector>
#include "OpenWsmanClient.h"
int main()
{
    trustwave::OpenWsmanClient cli("%{host}",%{port},"/wsman","http","Basic","%{username}","%{password}");
    cli.Identify();
    const trustwave::NameValuePairs nvp={{"IpEnabled","TRUE"}};
/*
    const std::string r{"http://schemas.microsoft.com/wbem/wsman/1/wmi/root/cimv2/Win32_Service"};
    std::cerr << cli.Get(r,std::addressof(nvp));
    const std::string r2{"http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2/CIM_ComputerSystem"};

    std::cerr << "1,\n";
  */

    const std::string r2{"http://schemas.microsoft.com/wbem/wsman/1/wmi/root/cimv2/*"};
    std::vector<std::string> res2;
    trustwave::WsmanFilter filt("http://schemas.microsoft.com/wbem/wsman/1/WQL","SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IpEnabled=TRUE");
    cli.Enumerate(r2,filt,res2);
    for(const auto e : res2)
    {
        std::cerr << e <<", ";
    }
    std::cerr << std::endl;
}

