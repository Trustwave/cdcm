#!/usr/bin/env ruby

# Majordomo Protocol Client API, Python version.
#
# Implements the MDP/Worker spec at http:#rfc.zeromq.org/spec:7.
#
# Author: Tom van Leeuwen <tom@vleeuwen.eu>
# Based on Python example by Min RK

require 'ffi-rzmq'
require 'mdp.rb'
require 'securerandom'
#require 'nokogiri'
require 'rexml/document'
include REXML
require 'singleton'
require "logger"
require 'timeout'
require 'myLogger.rb'
require 'results_verification.rb'
def log
    MyLogger.instance.log
end

###########################################################################
# Class: MajorDomoClient
# response_timeout can be set by set_resp_timeout. otherwise, default of 5 sec will be used
#
###########################################################################
class MajorDomoClient
  include MDP

  attr_accessor :timeout
  attr_reader  :sessions

  ########################################
  #
  ########################################
  def initialize broker
    @broker = broker
    @context = ZMQ::Context.new(1)
    @client = nil
    @poller = ZMQ::Poller.new
    @timeout = 2500
    @resp_timeout = 5
    @sessions = Array.new
    @verifier = Verifier.new
    reconnect_to_broker
  end

  ########################################
  #
  ########################################
  def send service, request
    request = [request] unless request.is_a?(Array)

    # Prefix request with protocol frames
    # Frame 0: empty (REQ emulation)
    # Frame 1: "MDPCxy" (six bytes, MDP/Client x.y)
    # Frame 2: Service name (printable string)
    request = ['', MDP::C_CLIENT, service].concat(request)
    @client.send_strings request
    nil
  end

  ########################################
  #
  ########################################
  def recv
    items = @poller.poll(@timeout)
    if items
      messages = []
      begin
      Timeout::timeout(@resp_timeout) {
          @client.recv_strings messages
      }
      rescue
          puts "reached timeout limit of " + @resp_timeout.to_s + " seconds when waiting for response. throwing"
          log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reached timeout limit of " + @resp_timeout.to_s + " seconds when waiting for response. throwing"}
          #raise Timeout::Error
      end
      messages.shift # empty

      # header
      if messages.shift != MDP::C_CLIENT
        #raise RuntimeError, "Not a valid MDP message"
      end

      messages.shift # service
      return messages
    end

    #nil
    return  "ERROR: malformed request"
  end

  ########################################
  #
  ########################################
  def reconnect_to_broker
    if @client
      @poller.deregister @client, ZMQ::DEALER
    end

    @client = @context.socket ZMQ::DEALER
    @client.setsockopt ZMQ::LINGER, 0
    # set dealer identity on the socket, like: 4 Hex-4 Hex (ABCD-12EF)
    dealer_identity = SecureRandom.hex(2).upcase + "-" + SecureRandom.hex(2).upcase
    @client.setsockopt ZMQ::IDENTITY, dealer_identity
    @client.connect @broker
    @poller.register @client, ZMQ::POLLIN
  end

  ########################################
  # push session to the end of sessions list
  ########################################
  def add_session(session)
      @sessions.push(session)
  end

  ########################################
  # start a new session with the broker.
  # session starts with 'start_session' action. as response, the broker sends the seesion id
  # inside the 'res' attrubute. this session id should be part of any message after the start_session
  ########################################
  def start_session(session)

    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"starting session"}

    if session.session_items[0].action != "start_session"
        log.error ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"first session_item is not start_session!!! add start_session item as first request for session: " + session.session_name}
        return
    end
    start_session_str = Message_Formater.instance.start_session_str(session)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"sending: \n" + start_session_str}
    session.session_items[0].req_msg = start_session_str
    send("echo", start_session_str)

    reply = recv()
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"recieved: \n" + (reply[0].nil? ? "nil" : reply[0])}
    #log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"recieved: \n" + reply[0]}
    reply_json = JSON.parse(reply[0])
    session.session_items[0].resp_msg = reply_json.to_json.to_s

    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"will verify results"}
    res = @verifier.verify(session.session_items[0])
    short_dump =  short_after_run_dump(session, session.session_items[0])
    puts short_dump
    # the "res" holds the session_id, determained and received by the broker
    session_id = reply_json['msgs'][0]['res'] #works with trustwave's broker + worker
    #session_id = reply_json['H']['session_id'] # works with zmq ruby broker + worker
    session.session_id = session_id
  end

  ########################################
  #  send a single action to the broker and wait to the response.
  ########################################
  def execute_session_item(session, session_item)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"executing session item: \n" + session_item.dump_before_reply}
    action_str = Message_Formater.instance.action_str(session, session_item)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"sending: \n" + action_str}
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"rotem finish sending \n" } #rotem to delete
    session_item.req_msg = action_str
    send("echo", action_str)

    reply = recv()
    if (reply[0].nil? || reply[0].empty? )
        log.error ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"recieved empty response"}
        @verifier.set_failed_verification(session_item)
    else
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"recieved: \n" + reply[0]}
        reply_json = JSON.parse(reply[0])
        session_item.resp_msg =  reply_json.to_json.to_s
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"will verify results"}
        res = @verifier.verify(session_item)
    end


    short_dump =  short_after_run_dump(session, session_item)
    puts short_dump
    #puts session_item.verification_ctx.dump
  end

  ########################################
  #  send a single action to the broker and wait to the response.
  #  session_item will be selected based on the session_item id
  ########################################
  def execute_session_item_by_session_id(session, session_item_id)
      session_item_index = session.session_items.index {|session_item| session_item.req_id == session_item_id}
      execute_session_item(session,session.session_items[session_item_index])
  end

  ########################################
  # send a single action with the index i, to the broker and wait to the response.
  # the index is the session_item position in the session_ites container.
  # index 0 is always the start_session item
  ########################################
  def execute_session_item_by_index(session, index)
    execute_session_item(session, session.session_items[index])
  end

  ########################################
  # run all the sessions, for each session run all the session_items sequentially
  # NOTE: will also run the start_session at begining of each session
  ########################################
  def execute_sessions_sequentially
      @sessions.each { |session|
          start_session(session)
          for session_item_index in 1..session.session_items.size-1
              execute_session_item_by_index(session,session_item_index)
          end
      }
  end


  ########################################
  # store the list of sessions to run
  ########################################
  def load_sessions(sessions)
      @sessions = sessions
  end

  ########################################
  #
  ########################################
  def dump
      dump_str = "MajorDomoClient Dump of all sessions:"
      @sessions.each { |session| dump_str += session.dump}
      return dump_str
  end

  ########################################
  #
  ########################################
  def short_after_run_dump (session, session_item)
      dump_str = "result:" + session_item.verification_ctx.vm_result + " ip:" + session.asset_details.remote + " action_name:" + session_item.action + " session_name:" + session.session_name + " req_id:" + session_item.req_id + " reason:[" + session_item.verification_ctx.verification_result.vm_result_message + "]\n"
      return dump_str
  end

  ########################################
  #
  ########################################
  def set_resp_timeout (timeout)
      @resp_timeout = timeout.to_i
  end
end # end of class MajorDomoClient


###########################################################################
# Class: Session
#  contains the data of the session with a single asset
#  session always start with the start_session request to the broker.
#  so on initialization, the first session_item is created for the start_session request
#  and placed as the first item in session_items
###########################################################################
class Session

    attr_accessor :session_id, :session_items, :asset_details, :session_name, :session_description

    ########################################
    #
    ########################################
    def initialize(asset_details = nil)
        @asset_details=asset_details
        @session_id="N\A"
        @session_name
        @session_description
        @session_items = Array.new
    end

    ########################################
    # adding new session item to the session_items container
    ########################################
    def add_session_item(action, action_params, verification_ctx=nil)
        new_sesion_item = Session_Item.new(action, action_params, verification_ctx)
        @session_items.push(new_sesion_item)
        return new_sesion_item.req_id
    end

    ########################################
    # adding new session item to the session_items container
    ########################################
    def remove_session_item(required_req_id)
      res = @session_items.delete_if { |item| item.req_id ==  required_req_id}
      log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"new session item removed. total " + @session_items.size.to_s + " session item in this session"}
    end

    ########################################
     # dump the session details
    ########################################
    def dump

     #log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {dump_str}
      dump_str = "\n==========================\n"
      dump_str += "Session Dump\n"
      dump_str += "session_id: " + @session_id + "\n"
      dump_str += "session_name: " + @session_name + "\n"
      dump_str += "session_description: " + @session_description + "\n"
      dump_str += @asset_details.dump()
      dump_str += "\nnumber of session items (including the start_session item): " + @session_items.size.to_s
      @session_items.each { |item| dump_str += item.dump}
      return dump_str
    end
end  #end of class Session



###########################################################################
# Class: Session_Item
#
#
###########################################################################
class Session_Item
    attr_accessor :verification_ctx, :action_params, :resp_msg, :req_msg
    attr_reader :req_id, :action

    def initialize(action, action_params, verification_ctx=nil)
        @req_id = SecureRandom.uuid
        @action = action
        @action_params = action_params #array of pairs. param_name, param_value
        @req_msg = ""
        @resp_msg = ""
        if verification_ctx.nil?
            @verification_ctx = Verification_Ctx.new
        else
            @verification_ctx = verification_ctx
        end
    end

    ########################################
    # dump the session_item details
    ########################################
    def dump
      dump_str = "\n==============================================\n"
      dump_str += "req_id: " + @req_id + "\naction: " + @action + "\naction_params: " + @action_params.to_s + "\nreq_msg: " + @req_msg + "\nresp_msg: " + @resp_msg
      dump_str += "\nverification context:\n" + @verification_ctx.dump
      dump_str +="\n==============================================\n"

      return dump_str
    end

    ########################################
    # dump the details that relevant before getting reply
    ########################################
    def dump_before_reply
      dump_str = "\n==============================================\n"
      dump_str += "req_id: " + @req_id + "\naction: " + @action + "\naction_params: " + @action_params.to_s
      dump_str +="\n==============================================\n"

      return dump_str
    end
end  #end of class Session_Item

###########################################################################
# Class: AssetDetails
# contains all the details about the asset:
#  remote (ip), username, password, domain, workstation
#
###########################################################################
class Asset_Details

    attr_reader :remote, :username, :password, :domain, :workstation

    def initialize(remote, username, password, domain, workstation)
        @remote, @username, @password, @domain, @workstation = remote, username, password, domain, workstation
    end

   ########################################
   # dump the content of Asset Details
   ########################################
   def dump
      tmp_str = "Asset Details: remote: " + @remote + ", username: " + @username + ", password: " + @password + ", domain: " + @domain + ", workstation: " + @workstation
      return tmp_str
   end
end  #end of class AssetDetails

###########################################################################
# Class: Message_Formater
# use this singltion class to get formatted message to send to broker.
# the returned message does not contain the zmq protocol frames, but only the
# content of the message frame
###########################################################################
class Message_Formater
    include Singleton

    ########################################
    # return the request boby for start_session action formatted as json
    ########################################
    def start_session_str(session)
      asset_details = session.asset_details
      session_item = session.session_items[0]
      start_session_str = {:H=>{:session_id=> "N/A"},
                        :msgs=>[ {:start_session=> {:id=> session_item.req_id().to_s,
                                                    :remote=> asset_details.remote,
                                                    :domain=> asset_details.domain,
                                                    :username=> asset_details.username,
                                                    :password=> asset_details.password,
                                                    :workstation => asset_details.workstation
                                                   }
                                }
                              ]
                      }
      return start_session_str.to_json
    end

    ########################################
    # return the request boby for general action formatted as json
    ########################################
    def action_str(session, session_item)
        action_str = '{"H": {"session_id":"' + session.session_id + '"}, "msgs":[ { "' + session_item.action + '" : { "id" : "' + session_item.req_id + '"'
        session_item.action_params.each {
            |param_pair|
            # if the param value is empty (valid use case, for example pattern value is empty for )
            sub_str = ', "' + param_pair[0] + '" : "' + (param_pair[1].nil? ? "" : param_pair[1]) + '" '
            action_str +=  sub_str
        }
        action_str += '} } ]}'

      return action_str
    end


end #end of class Message_Formater
