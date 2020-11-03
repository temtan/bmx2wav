## TTRL : text_template.rb

require 'nkf'

require 'ttrl/utility.rb'


module TTRL
  ## -- TextTemplate -----------------------------------------------------
  class TextTemplate
    ## Usage : define at inherited class
    ## [ FILENAME = <filepath> ]
    ## [ ENCODE = <encode proc> ]
    ## PARAMS_STRUCT = Struct.new( <members ...> )

    module EncodeProcess
      NONE         = lambda {|str| str }
      SJIS_TO_EUC  = lambda {|str| NKF.nkf( '-S -e', str ) }
      SJIS_TO_UTF8 = lambda {|str| NKF.nkf( '-S -w', str ) }
      EUC_TO_SJIS  = lambda {|str| NKF.nkf( '-E -s', str ) }
      EUC_TO_UTF8  = lambda {|str| NKF.nkf( '-E -w', str ) }
      UTF8_TO_SJIS = lambda {|str| NKF.nkf( '-W -s', str ) }
      UTF8_TO_EUC  = lambda {|str| NKF.nkf( '-W -e', str ) }
    end

    def self.define_child( parameter_name, klass )
      self.const_set( :CHILDREN, {} ) unless self.const_defined?( :CHILDREN )
      self::CHILDREN[parameter_name] = klass

      self::PARAMS_STRUCT.class_eval {
        define_method( parameter_name ) {|&block|
          @__template.child_make_text( parameter_name, block )
        }
      }
    end

    def child_make_text( parameter_name, block )
      template = @children_template[parameter_name]
      template.reset
      block.call( template )
      @children_result[parameter_name] << template.make_text
    end

    def self.define_new_child( parameter_name, child_parameters )
      klass = Class.new( TextTemplate )
      klass.const_set( :PARAMS_STRUCT, Struct.new( *child_parameters ) )
      self.define_child( parameter_name, klass )
      klass
    end


    def initialize
      @params = (self.class)::PARAMS_STRUCT.new
      @params.instance_variable_set( :@__template, self )
      @children_result = {}
      @children_template = {}

      tmp = block_given? ? yield : File.read( (self.class)::FILENAME )
      @source = self.class.const_defined?( 'ENCODE' ) ? (self.class)::ENCODE.call( tmp ) : tmp

      tmp = @source
      loop {
        m = /\%\%([a-zA-Z_][0-9A-Za-z_]*)\%\%\{(.*?)\}\%\%\1\%\%/m.match( tmp )
        break if m.nil?
        name = m[1].to_sym
        @children_template[name] = (self.class)::CHILDREN[name].new { m[2] }
        tmp = "#{m.pre_match}@@#{name}@@#{m.post_match}"
      }
      @base = tmp
      self.reset
    end

    def reset
      @params.members.each {|one| @params[one] = '' }
      @children_template.each_key {|key| @children_result[key] = '' }
    end

    def make_text
      result = @base.dup
      @params.each_pair {|param, value|
        result.gsub!( Regexp.compile( Regexp.escape( "@@#{param}@@" ) ) ) { value.to_s }
      }
      @children_result.each_pair {|param, value|
        result.gsub!( Regexp.compile( Regexp.escape( "@@#{param}@@" ) ) ) { value.to_s }
      }
      result
    end

    attr_accessor( :params )
    attr_reader( :children_result )

    def write_to_file( path )
      Utility.file_puts( path, "w", self.make_text )
    end
  end
end
