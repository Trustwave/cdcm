#!/usr/bin/env ruby
folder = File.expand_path('.',__dir__)
$:.unshift(folder) unless $:.include?(folder)

#require 'mdclient2.rb'
# in general: run like: ruby mdclient2.rb  XML_NAME CLIENT_ID
# for each CLIENT_ID there will be seperate log file at the logs directory (e.g: cdcm_client1.log, cdcm_client2.log)
# log directory can be found in the testsRunner_settings.json file
# if  XML_NAME  not specified, the default xml is actions.xml will be used 
# if CLIENT_ID  not specified, the default 1 will be used .
fork { exec("ruby mdclient_run_loop.rb actions1.xml 1") }
#fork { exec("ruby mdclient_run_loop.rb actions2.xml 2") }
#fork { exec("ruby mdclient_run_loop.rb actions3.xml 3") }
#fork { exec("ruby mdclient_run_loop.rb actions4.xml 4") }
#fork { exec("ruby mdclient_run_loop.rb actions5.xml 5") }
#fork { exec("ruby mdclient_run_loop.rb actions6.xml 6") }
#fork { exec("ruby mdclient_run_loop.rb actions7.xml 7") }
#fork { exec("ruby mdclient_run_loop.rb actions8.xml 8") }
#fork { exec("ruby mdclient_run_loop.rb actions9.xml 9") }

puts "DONE"
