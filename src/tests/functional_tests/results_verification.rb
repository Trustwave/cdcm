#!/usr/bin/env ruby

folder = File.expand_path('.',__dir__)
$:.unshift(folder) unless $:.include?(folder)
folder = File.expand_path('./functional_tests',__dir__)
$:.unshift(folder) unless $:.include?(folder)
require "logger"
require 'myLogger.rb'


def log
    MyLogger.instance.log
end

########################################
# extract the 'key' value from the json response
########################################
def extract_response_key(session_item)
    response = session_item.resp_msg
    if response.empty?
        ver_result = Verification_Result.new(false, "the text to search upon is empty")
        return ver_result
    end

    response_key = JSON.parse(response)
    response_key = response_key['msgs'][0]['res']
    return response_key
end

########################################
# find if the key exist in the text
# after downcase all chars
# return value is a pair of boolean and an string with the result message
########################################
def verify_key_exist(session_item)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"verify_key_exist callled"}

    key_name = session_item.verification_ctx.vm_params[0][0]
    key_value = session_item.verification_ctx.vm_params[0][1]
    if key_value.nil? || key_value.empty?
        ver_result = Verification_Result.new(false, "the key to find is empty")
        return ver_result
    end
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"looking for the key: " + key_value}
    response_key = extract_response_key(session_item)

    #if response_key.downcase.include? key_value.downcase
    if response_key.to_s.downcase.include? key_value.downcase    
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"key was found in text"}
        ver_result = Verification_Result.new(true, "key was found in text")
        return ver_result
    else
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"key was NOT found in text"}
        ver_result = Verification_Result.new(false, "key was NOT found in text")
        return ver_result
    end
end

########################################
#
########################################
def compare_result_to_bool(session_item)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"compare_result_to_bool callled"}
    key_name = session_item.verification_ctx.vm_params[0][0]
    key_value = session_item.verification_ctx.vm_params[0][1]
    if key_value.nil? || key_value.empty?
        ver_result = Verification_Result.new(false, "the key to find is empty")
        return ver_result
    end
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"copmare result with expected value of: " + key_value}
    response_key = extract_response_key(session_item)

    #if response_key.downcase == key_value.downcase
    if response_key.to_s.downcase == key_value.downcase
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"response is as expected"}
        ver_result = Verification_Result.new(true, "response is as expected: " + key_value.downcase)
        return ver_result
    else
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"response val is not as expected"}
        ver_result = Verification_Result.new(false, "response val is not as expected")
        return ver_result
    end
end

########################################
# find if the key exist in the text
# after downcase all chars
# return value is a pair of boolean and an string with the result message
########################################
def reg_match(session_item)
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"reg_match called"}

    key_name = session_item.verification_ctx.vm_params[0][0]
    key_value = session_item.verification_ctx.vm_params[0][1]
    if key_value.nil? || key_value.empty?
        ver_result = Verification_Result.new(false, "the regex to find is empty")
        return ver_result
    end
    log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"looking for the key: " + key_value}
    response_key = extract_response_key(session_item)

    #regex_match_res = response_key.match(key_value)
    regex_match_res = response_key.to_s.match(key_value)

    if regex_match_res.nil?
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"regex does not match the response"}
        ver_result = Verification_Result.new(false, "regex does not match the response")
        return ver_result
    else
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"regex match to the response"}
        ver_result = Verification_Result.new(true, "regex " + key_value + " match to the response")
        return ver_result
    end

end

########################################
#
########################################
class Verifier

    def verify(session_item)

        case session_item.verification_ctx.vm_name
        when "substr_exist"
            ver_result = verify_key_exist(session_item)
            session_item.verification_ctx.verification_result = ver_result
            if ver_result.vm_result.to_s == "true"
                session_item.verification_ctx.vm_result = "passed"
            else
                session_item.verification_ctx.vm_result = "failed"
            end
        when "boolean"
            ver_result = compare_result_to_bool(session_item)
            session_item.verification_ctx.verification_result = ver_result
            if ver_result.vm_result.to_s == "true"
                session_item.verification_ctx.vm_result = "passed"
            else
                session_item.verification_ctx.vm_result = "failed"
            end
        when "regex_match"
            ver_result = reg_match(session_item)
            session_item.verification_ctx.verification_result = ver_result
            if ver_result.vm_result.to_s == "true"
                session_item.verification_ctx.vm_result = "passed"
            else
                session_item.verification_ctx.vm_result = "failed"
            end
        when "do_not_verify"
            session_item.verification_ctx.vm_result = "passed"
        else
            log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"ERROR: unknown method used: " + session_item.verification_ctx.vm_name}
            puts "ERROR: unknown method used"
            session_item.verification_ctx.verification_result.vm_result_message = "ERROR: unknown method: #{session_item.verification_ctx.vm_name}"
            session_item.verification_ctx.vm_result = "failed"
        end
        log.info ("#{File.basename(__FILE__)}::#{__LINE__} #{self.class.name}::#{__callee__}") {"verification result: " + session_item.verification_ctx.dump}

    end

    def set_failed_verification(session_item)
        session_item.verification_ctx.verification_result = Verification_Result.new(false, "malformed request")
        session_item.verification_ctx.vm_result = "failed"
    end
end #end of class Verifier

class Verification_Result
    attr_accessor :vm_result, :vm_result_message
    def initialize (vm_result="", vm_result_message="")
        @vm_result = vm_result
        @vm_result_message = vm_result_message
    end
end

########################################
# vm_params is array of pair holding [param_name, param_value]
# after downcase all chars
########################################
class  Verification_Ctx
  attr_accessor :vm_name, :vm_params, :verification_result, :vm_result

  def initialize (verification_method_name="do_not_verify", vm_params=[])
      @vm_name=verification_method_name
      @vm_params = vm_params
      @verification_result = Verification_Result.new
      # if we should not verify response => test considered as passed successfully
      if verification_method_name=="do_not_verify"
          @vm_result="passed"
      else
          @vm_result = ""
      end
  end

  def dump
      #dump_str = "\n==========================\n"
      #dump_str = "Verification_Ctx Dump\n"
      dump_str = "verification method: " + @vm_name + "\n"
      dump_str += "expected verification parameters: " + @vm_params.to_s + "\n"
      dump_str += "verification result: " + @verification_result.vm_result.to_s + "\n"
      dump_str += "verification message: " + @verification_result.vm_result_message.to_s + "\n"
      dump_str += "final result: " + @vm_result + "\n"
      return dump_str
  end


end #end of class verification_ctx
