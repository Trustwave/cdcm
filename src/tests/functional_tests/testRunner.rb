#!/usr/bin/env ruby

require 'myLogger.rb'
require 'mdcliapi2'
require 'results_verification.rb'

class Statistics
    attr_accessor :total_run, :total_passed, :total_failed
    def initialize 
        @total_run =0
        @total_passed = 0
        @total_failed = 0
    end
end #end of class statistics

###########################################################################
# Class: Test_Runner
#
#
###########################################################################
class Test_Runner

    def initialize
        @client = MajorDomoClient.new('tcp://localhost:7613')
        @sessions = Array.new
        @assets_list = Hash.new { | asset_name , asset_details |}
        @actions_list = Hash.new { | action_name , action_params |}
        @verification_methods_list  = Hash.new { | vm_name , vm_params |}
        @verifier = Verifier.new
        @response_timeout = nil
        @statistics = Statistics.new
    end

    ########################################
    #
    ########################################
    def sumerize_results
        total_num_of_session_items = 0
        num_passed = 0
        num_failed = 0
        num_other = 0
        @client.sessions.each {
            |session|
            session.session_items.each {
                |session_item|
                total_num_of_session_items += 1
                case session_item.verification_ctx.vm_result
                when "passed"
                    num_passed += 1
                when "failed"
                    num_failed += 1
                else
                    num_other += 1
                end
            }
        }
        @statistics.total_run += total_num_of_session_items
        @statistics.total_passed += num_passed
        @statistics.total_failed += num_failed
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"\n===================================\nrun " + total_num_of_session_items.to_s + " session items. \npassed [" + num_passed.to_s + "] \nfailed [" + num_failed.to_s + "] \nother\/error [" + num_other.to_s + "]\n===================================\n" + "statistics (since testRunner started): total run: " + @statistics.total_run.to_s + " total passed: " + @statistics.total_passed.to_s + " total failed: " + @statistics.total_failed.to_s + "\n==================================="}
    
        puts "===================================\nrun " + total_num_of_session_items.to_s + " session items. \npassed [" + num_passed.to_s + "] \nfailed [" + num_failed.to_s + "] \nother\/error [" + num_other.to_s + "]\n==================================="
        puts "statistics (since testRunner started): total run: " + @statistics.total_run.to_s + " total passed: " + @statistics.total_passed.to_s + " total failed: " + @statistics.total_failed.to_s + "\n===================================" 
    end
    ########################################
    #
    ########################################
    def run_test_from_file(path)
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"\n\n\nstart of run_test_from_file"}
        @sessions.clear
        load_sessions_from_xml_file(path)
        @client.load_sessions(@sessions)
        #run all sessions
        @client.execute_sessions_sequentially
        sumerize_results
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"=========================Done running all sessions. results dump: ========================="}
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {@client.dump}
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"end of run_test_from_file"}
    end

    ########################################
    # map between action name and action params (array of strings)
    ########################################
    def read_actions root_element
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reading actions from xml"}
        root_element.elements.each("actions/action") {
            |action|
            name = action.attributes["name"]
            params_str = action.attributes["params"]
            params_vec = params_str.delete(" ").split(",")
            @actions_list[name] = params_vec
        }
    end

    ########################################
    # map between vm_name and vm_params (array of strings)
    ########################################
    def read_verification_methods root_element
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reading verification methods from xml"}
        root_element.elements.each("verification_methods/verification_method") {
            |verification_method|
            name = verification_method.attributes["vm_name"]
            params_str = verification_method.attributes["vm_params"]
            params_vec = params_str.delete(" ").split(",")
            @verification_methods_list[name] = params_vec
        }
    end

    ########################################
    # map between asset name and asset_details
    ########################################
    def read_assets root_element
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reading assets from xml"}
        root_element.elements.each("assets/asset") {
            |asset|
            name = asset.attributes["name"]
            addr = asset.attributes["address"]
            user = asset.attributes["user"]
            pass = asset.attributes["pass"]
            domain = asset.attributes["domain"]
            workstation = asset.attributes["workstation"]
            @assets_list[name] = Asset_Details.new(addr, user, pass, domain,   workstation)
        }
    end

    ########################################
    #
    ########################################
    def read_cdcm_client_settings root_element
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reading cdcm_citent_settings from xml"}
        root_element.elements.each("cdcm_client_settings") {
            |cdcm_client_settings|
            timeout = cdcm_client_settings.attributes["response_timeout"]
            #verify timeout value is a number
            if timeout !~ (/\D/)
                @response_timeout = timeout
            end
        }
    end

    ########################################
    #
    #
    ########################################
    def load_sessions_from_xml_file(path)
        xmlfile = File.new(path)
        xmldoc = Document.new(xmlfile)
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"xml opened: " + path}

        root_element = xmldoc.root()
        read_cdcm_client_settings(root_element)
        if (! @response_timeout.nil?)
            log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"setting timeout of: " + @response_timeout.to_s + " seconds"}
            @client.set_resp_timeout(@response_timeout)
        else
            log.error ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"error reading response timeout. will not set client timeout"}
        end

        # read actions snippet
        read_actions(root_element)
        read_verification_methods(root_element)
        read_assets(root_element)

        #for each 'session'
        root_element.elements.each("sessions/session") {
            |xml_session|
            #create Session object
            tmp_session =  Session.new
            # extract the asset details
            asset_name = xml_session.attributes["asset_name"]
            tmp_session.asset_details = @assets_list[asset_name]

            session_name = xml_session.attributes["session_name"]
            tmp_session.session_name = session_name

            session_description = xml_session.attributes["description"]
            tmp_session.session_description = session_description

            test_set_name =  xml_session.attributes["test_set_name"]

            #populate the session items based on the test_set_name
            root_element.elements.each("test_sets/test_set") {
                |test_set|
                current_test_set_name = test_set.attributes["test_set_name"]
                if current_test_set_name == test_set_name
                    #extract the session_items and add to session
                    test_set.elements.each("session_item") {
                        |xml_session_item|
                        action_name = xml_session_item.attributes["action"]
                        # get the list of params required for the specific action
                        action_params_names_vec = @actions_list[action_name]
                        # build a vector that hold pair of [param , param_value]
                        action_params = Array.new
                        action_params_names_vec.each {
                            |param|
                            param_val = xml_session_item.attributes[param]
                            param_pair = [param , param_val]
                            action_params.push(param_pair)
                        }

                        ver_method = xml_session_item.attributes["verification_method"]
                        # get the list of params required for the specific action
                        vm_params_names_vec = @verification_methods_list[ver_method]
                        # build a vector that hold pair of [param , param_value]
                        verification_params = Array.new
                        vm_params_names_vec.each {
                            |param|
                            param_val = xml_session_item.attributes[param]
                            param_pair = [param , param_val]
                            verification_params.push(param_pair)
                        }
                        tmp_verification_ctx = Verification_Ctx.new(ver_method, verification_params)
                        tmp_session.add_session_item(action_name, action_params, tmp_verification_ctx)
                    }
                    @sessions.push(tmp_session)
                    break
                end
            }

        }
    end

    ########################################
    # push session to the end of sessions list
    ########################################
    def add_session(session)
        @sessions.push(session)
    end
end #end of class Test_Runner
