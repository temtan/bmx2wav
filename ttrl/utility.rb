## TTRL : utility.rb


module TTRL
  module Utility
    self.extend( self )

    def filename_to_path( filename )
      m = /^(.+?):(\d+)(?::in \`(.*)\')?/.match( caller( 1 ).first )
      File.dirname( File.expand_path( m[1] ) ) + '/' + filename
    end

    def loadfile_as_standard_format( path )
      File.foreach( path ) {|line|
        line.strip!
        next if /^\#\#/.match( line )
        next if line.empty?
        yield line
      }
    end

    def file_puts( filepath, mode, data )
      File.open( filepath, mode ) {|file|
        file.puts( data )
      }
    end

    def spawn( cmd, *args )
      if RUBY_VERSION >= "1.9.0"
        return Object.__send__( "spawn", cmd, *args )
      else
        return ::Process.fork {
          ::Process.exec( cmd, *args )
        }
      end
    end

  end
end
