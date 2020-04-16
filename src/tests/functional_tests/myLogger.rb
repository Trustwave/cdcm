#!/usr/bin/env ruby
require 'fileutils'
require 'json'
# https://ruby-doc.org/stdlib-2.6.3/libdoc/logger/rdoc/Logger.html
# TODO: write destructor that close the log file: @logger.close https://stackoverflow.com/questions/5956067/ruby-destructors
class MyLogger
    include Singleton
    def initialize
        if Dir.pwd.end_with?"functional_tests"
            settings_file = File.open Dir.pwd+"/testsRunner_settings.json"
        else
            settings_file = File.open Dir.pwd+"/functional_tests/testsRunner_settings.json"
        end
   	    settings_data = JSON.load settings_file
	    settings_file.close
	    logs_dir = settings_data["logger"]["logs_dir"].to_s
	    FileUtils.mkdir_p(logs_dir)
	    log_file = File.open("#{logs_dir}/cdcm_client#{$client_id}.log", File::WRONLY | File::CREAT | File::TRUNC)
	    log_file.sync = true
        @logger = Logger.new( log_file )
        @logger.datetime_format = '%d-%m-%Y %H:%M:%S::%3N'
        @logger.formatter = proc do |severity, datetime, progname, msg|"[#{severity}] [#{datetime.strftime(@logger.datetime_format)}] [#{progname}]: #{msg}\n"
    end
end
    def log
        @logger
    end
end #end of class MyLogger
