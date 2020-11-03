## TTRL : enum.rb


module TTRL
  class Enum
    def self.new( arg1 = nil, *rest )
      name, entry =
      if ( arg1.is_a?( String ) or arg1.nil? )
        [arg1, rest]
      else
        [nil, rest.unshift( arg1 )]
      end

      klass = Class.new( self )
      self.const_set( name, klass ) unless name.nil?

      klass.instance_eval{|klass|
        self.extend( Enumerable )
        @index = {}
        @members = []

        def new( value )
          obj = self.allocate
          obj.__send__( :initialize, value )
          obj
        end
        alias :[]   :new

        def members
          @members.dup
        end

        def member?( value )
          @index.has_key?( value.is_a?( self ) ? value.value : value )
        end

        def define( arg1, arg2 = nil )
          if ( arg1.is_a?( Hash ) )
            arg1.each_pair{|name, value|
              self.define( name, value )
            }
          elsif ( arg1.is_a?( Array ) )
            arg1.each{|name|
              self.define( name )
            }
          else
            name = arg1
            value = if arg2.nil? then name.to_sym else arg2 end
            @members << self.new( value )
            self.const_set( name, @members.last )
            @index[value] = name.to_s.dup.freeze
          end
        end

        def each_member
          self.members.each{|one| yield( one ) }
        end
        alias :each :each_member

        def length
          self.members.length
        end
        alias :size :length
      }

      entry.each{|one| klass.define( one ) }
      klass
    end


    def initialize( value )
      @value = value
    end

    def hash
      @value.hash
    end

    def eql?( other )
      @value.eql?( other )
    end

    def ==( other )
      @value == other
    end

    def proper?
      self.class.member?( self.value )
    end

    def name
      self.class.instance_variable_get( :@index )[self.value]
    end
    alias :to_s :name

    def value
      @value
    end

    def inspect
      "\#<enum #{self.class} #{self.proper? ? self.name : nil.inspect}, #{self.value.inspect}>"
    end

  end
end


## test code
if __FILE__ == $0
  Foo = Enum.new
  Foo.define( :Bar, 1 )
  Foo.define( :Baz )

  ## そのクラスの定数として登録される
  p Foo::Bar         # => #<enum Foo Bar, 1>
  p Foo::Bar.name    # => "Bar"
  p Foo::Bar.value   # => 1

  p Foo.new( 1 )     # => #<enum Foo Bar, 1>
  p Foo.member?( 1 ) # => true
  p Foo.member?( 2 ) # => false
  p Foo.members      # => [#<enum Foo Baz, :Baz>, #<enum Foo Bar, 1>]

  ## 存在しない値から作成できるが正しくない
  p Foo.new( 2 ).proper? # => false

  ## 他の定義の仕方
  ## Hash を使う
  Hoge = Enum.new( {
    :Foo => 100,
    :Bar => 200,
    :Baz => 300,
  } )

  ## Array を使う
  Fuga = Enum.new( [:Foo, :Bar, :Baz] )

  ## 可変長引数を使う
  Piyo = Enum.new(
    :Foo,
    :Bar,
    :Baz
  )

  [Hoge, Fuga, Piyo].each{|one| p one.members }
end
