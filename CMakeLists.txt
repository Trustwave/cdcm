cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include_directories(src)
configure_file(cdcm_settings.json /var/cdcm/conf/cdcm_settings.json COPYONLY)
configure_file(Logger/module.xml /var/cdcm/conf/Logger/module.xml COPYONLY)

install(FILES scripts/cdcm.sh
        DESTINATION ${release_bin_dest})