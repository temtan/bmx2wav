## TTRL : releaser.rb

require 'fileutils'

require "fiddle/import"


module TTRL
  module ZipDll
    extend Fiddle::Importer
    dlload "ZIP32J.DLL"
    extern "int Zip( long, void*, void*, long )"
  end

  class Releaser
    def initialize( name, base, test_mode = false )
      @name = name
      @base = base
      @files = {}
      @dirs = {}
      @pre_copy_procs = []
      @post_copy_procs = []
      @test_mode = test_mode
      @strftime_template = '%y%m%d'
      @release_dir = nil
      @additional_string = nil
    end

    attr_accessor( :strftime_template, :release_dir, :additional_string )

    def add_file( file, to = "./" )
      @files[file] = to
    end

    def add_dir( dir, to = "./" )
      @dirs[dir] = to
    end

    def add_pre_copy_proc( proc )
      @pre_copy_procs << proc
    end

    def add_post_copy_proc( proc )
      @post_copy_procs << proc
    end

    def add_pre_copy( &block )
      self.add_pre_copy_proc( block )
    end

    def add_post_copy( &block )
      self.add_post_copy_proc( block )
    end

    def make_zip
      throw "release_dir is nil" if @release_dir.nil?
      today = Time.now.strftime( @strftime_template )
      file_utils = @test_mode ? FileUtils::DryRun : FileUtils::Verbose

      Dir.chdir( @base ) {
        to_base = "./#{@name}"
        file_utils.mkdir( to_base )
        @pre_copy_procs.each {|proc| proc.call( file_utils, to_base ) }
        begin
          @files.each {|file, to|
            file_utils.copy( "./#{file}", "#{to_base}/#{to}", {:preserve => true} )
          }
          @dirs.each {|dir, to|
            file_utils.cp_r( "./#{dir}", "#{to_base}/#{to}", {:preserve => true} )
          }
          @post_copy_procs.each {|proc| proc.call( file_utils, to_base ) }

          command = "-r -q ./#{@release_dir}/#{@name}#{@additional_string.to_s}#{today}.zip #{to_base}"
          STDERR.puts( "zip command : #{command}" )
          unless ( @test_mode )
            buf = "\0" * 65536
            ret = ZipDll.Zip( 0, command, buf, buf.size )
            ## ret = zip_func.call( 0, command, buf, buf.size )
            if ret != 0
              puts buf.strip
              raise
            end
          end
        ensure
          file_utils.rm_r( to_base )
        end
      }
    end

  end
end
