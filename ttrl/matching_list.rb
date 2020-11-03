## TTRL : matching_list.rb

require 'ttrl/utility.rb'


module TTRL
  ## -- MatchingList -----------------------------------------------------
  class MatchingList
    def initialize( path = nil )
      @root = {}
      @files = []
      self.read_file( path ) {|one| block_given? ? (yield one) : path } if path
    end

    attr_reader( :files )

    def read_file( path )
      @files << path
      Utility.loadfile_as_standard_format( path ) {|str|
        self.add( str ) {|one| block_given? ? (yield one) : path }
      }
    end

    def add( str )
      current = @root
      str.each_byte {|c|
        current[c] = {} unless current.has_key?( c )
        current = current[c]
      }
      current[nil] = block_given? ? (yield str) : true
    end

    def match( str )
      current = @root
      str.each_byte {|c|
        return false unless current.has_key?( c )
        current = current[c]
        return current[nil] if current.has_key?( nil )
      }
      current[nil]
    end
  end

end
