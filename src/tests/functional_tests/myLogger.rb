#!/usr/bin/env ruby
require 'fileutils'
# https://ruby-doc.org/stdlib-2.6.3/libdoc/logger/rdoc/Logger.html
# TODO: write destructor that close the log file: @logger.close https://stackoverflow.com/questions/5956067/ruby-destructors
class MyLogger
    include Singleton
    def initialize
        response = FileUtils.mkdir_p('/var/log/cdcm/testsRunner')

        #log_file = File.open('cdcm_client.log', File::WRONLY |  File::CREAT | File::APPEND )
        log_file = File.open("/var/log/cdcm/testsRunner/cdcm_client#{$client_id}.log", File::WRONLY | File::CREAT | File::TRUNC)
        log_file.sync = true
        @logger = Logger.new( log_file )
        @logger.datetime_format = '%d-%m-%Y %H:%M:%S::%3N'
        @logger.formatter = proc do |severity, datetime, progname, msg|
           "[#{severity}] [#{datetime.strftime(@logger.datetime_format)}] [#{progname}]: #{msg}\n"

        end
    end

    def log
        @logger
    end

end #end of class MyLogger
