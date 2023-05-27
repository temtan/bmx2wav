
def get_most_newer_dir( dir )
  tmp = Dir.glob( dir + "/*" )
  tmp = tmp.delete_if{|one| /[^0-9\.]/.match( File.basename( one ) ) }
  tmp.sort.reverse.first
end

tmp = "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"
tmp = get_most_newer_dir( tmp )
tmp += "/bin/Hostx64/x64/nmake.exe"
make_command = tmp.gsub( /\//, "\\" )

this_dir = File.dirname( File.expand_path( __FILE__ ) )

Dir.glob( this_dir + "/*.tdp" ) {|file|
  data = File.read( file )
  data.each_line{|line|
    m = /^OutputDirectoryName=(.*)$/.match( line )
    if m
      target_makefile = this_dir + "/" + m[1] + "/Makefile"
      system( "\"#{make_command}\" /f #{target_makefile} clean" )
    end
  }
}
