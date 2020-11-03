## TTRL : schedule_executor.rb

require 'pstore'

require 'ttrl/utility.rb'


module TTRL
  class ScheduleExecutor
    DATABASE_KEY = 'last'

    def initialize( path )
      @database = PStore.new( path )
    end

    def execute_once_a_day
      now = Time.now
      @database.transaction {
        last = @database.fetch( DATABASE_KEY, Time.at( 0 ) )
        unless last.year == now.year && last.month == now.month && last.day == now.day
          @database[DATABASE_KEY] = now if yield last
        end
      }
    end
  end
end
