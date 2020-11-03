## TTRL : process_mutex.rb

module TTRL
  class ProcessMutex
    def initialize( path )
      @path = path
    end

    def lock
      File.open( @path, 'w' ) {|file|
        file.flock( File::LOCK_EX )
        begin
          yield
        ensure
          file.flock( File::LOCK_UN )
        end
      }
    end

    def locked?
      File.open( @path, 'r' ) {|file|
        ret = file.flock( File::LOCK_SH | File::LOCK_NB )
        file.flock( File::LOCK_UN ) if ret == 0
        return ret ? false : true
      }
    end

  end
end
