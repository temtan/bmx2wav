
require './ttrl/releaser.rb'

base = File.dirname( File.expand_path( __FILE__ ) )
project_name = "bmx2wav"

version = lambda {
  File.read( "./common.h" ).each_line{|line|
    if ( /VERSION/.match( line ) )
      return /\"([^\"]*)\"/.match( line )[1]
    end
  }
}.call


## -- exe -----
STDERR.puts( "== exe ====================" )

releaser = TTRL::Releaser.new( project_name, base )
releaser.strftime_template = ''
releaser.additional_string = "-#{version}"

[
"Release/bmx2wav.exe",
"bmx2wav.base.nut",
"bmx2wav.class.nut",
"Release_searcher/bmx2wav_searcher.exe",
"bmx2wav_searcher.class.nut",
"bmx2wav.txt",
].each {|file| releaser.add_file( file ) }

[
"script_tools",
].each {|dir| releaser.add_dir( dir ) }

releaser.add_pre_copy {|file_utils, to_base|
  [
  ].each {|one| file_utils.mkdir( "#{to_base}/#{one}" ) }

  file_utils.mkdir( "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/japanese.dll", "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/english.dll",  "#{to_base}/lang" )
  ## yet test
  file_utils.cp( "./lang/chinese_simplified.dll",  "#{to_base}/lang" )
  file_utils.cp( "./lang/chinese_traditional.dll", "#{to_base}/lang" )

  file_utils.mkdir( "#{to_base}/manual" )
  Dir.glob( "./man/html/*" ) {|file|
    file_utils.cp_r( file, "#{to_base}/manual" )
  }
}

releaser.release_dir = "../backups/#{project_name}/web"
releaser.make_zip
