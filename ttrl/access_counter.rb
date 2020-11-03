## TTRL : access_counter.rb

require 'pstore'


module TTRL
  ## -- AccessCounter ----------------------------------------------------
  class AccessCounter
    DEFAULT_INTERVAL_MINIT = 30

    def initialize( path, interval = DEFAULT_INTERVAL_MINIT )
      @database = PStore.new( path )
      @interval_minit = interval
      @last_count = nil
    end

    attr_reader( :inverval_minit, :last_count )

    def get_address
      address = ENV['REMOTE_ADDR'] || ENV['REMOTE_HOST'] || ''
    end

    def count_up
      now = Time.now
      address = self.get_address
      @database.transaction {
        @last_count = @database.fetch( 'count', 0 )
        history = @database.fetch( 'history', {} )
        new_history = {}
        history.each {|address, time|
          new_history[address] = time if now - time < @interval_minit * 60
        }
        unless new_history.has_key?( address )
          @last_count += 1
          new_history[address] = now
        end
        @database['count'] = @last_count
        @database['history'] = new_history
      }
      @last_count
    end

    def get_count
      @database.transaction {
        @last_count = @database.fetch( 'count', 0 )
      }
      @last_count
    end

    def set_count( count )
      @last_count = count
      @database.transaction {
        @database['count'] = @last_count
      }
    end
  end
end
