## TTRL : ssi_portal.rb
## using fork()


module TTRL
  ## -- SSIPortal --------------------------------------------------------
  class SSIPortal
    def initialize
      @remove_request_method = false
      @change_stdout_to_null = false
      @pid = nil
    end

    attr_accessor( :remove_request_method, :change_stdout_to_null )

    def start
      @pid = ::Process.fork {
        ENV['REQUEST_METHOD'] = nil if @remove_request_method
        STDOUT.reopen( "/dev/null", "w" ) if @change_stdout_to_null
        yield
      }
    end

    def wait
      ::Process.waitpid( @pid )
    end

    ## -- Process --------------------------------------------------------
    class Process
      def initialize( command, *args )
        @command = command
        @args = args
        @pid = nil
      end

      attr_reader( :command, :args, :pid )

      def start
        @pid = ::Process.fork {
          ::Process.exec( @command, *@args )
        }
      end

      def wait
        ::Process.waitpid( @pid )
      end
    end
  end
end
