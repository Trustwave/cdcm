# cdcm_client_ruby

the cdcm client can be used to test the cdcm functionality.
the client implements the cdcm protocol and the requests are sent using ZMQ sockets.
this client implements the MajorDomo design pattern.

the testRunner reads xml that holds the asset details, the set of actions to run on the asset and verification details for each action.
from the xml the testRunner prepare a structure that holds all of these details, the 'sessions', load it into the  MajorDomoClient 
run options:
* single client 
command:
'ruby mdclient2 XML_NAME CLIENT_ID'
if XML_NAME is not listed, actions.xml will be used by default
if CLIENT_ID is not listed, default client id will be 1 (means that the log file will be cdcm_client1.log)
'ruby mdclient2.rb' 
'ruby mdclient2.rb actions1.xml' 
'ruby mdclient2.rb actions1.xml 1' 

* single client - run in loop
command:
'ruby mdclient_run_loop.rb actions1.xml 1'

* multiple clients - run in loop
command:
'ruby run_multiple_clients.rb'
edit the file run_multiple_clients.rb. 
each client will be executed at seperate process, in loop

logs directory 
log path can be set by editing the testsRunner_settings.json file

how to use the actions.xml:

