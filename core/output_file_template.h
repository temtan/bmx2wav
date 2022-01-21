// core/output_file_template.h

#pragma once

#include <string>
#include <optional>
#include <vector>
#include <memory>
#include <functional>


namespace BMX2WAV::Core::OutputFileTemplate {
  class Maker;

  // -- Element ----------------------------------------------------------
  class Element {
    friend class Parser;

  public:
    virtual std::string MakeText( Maker& maker ) = 0;
    void AppendToText( char c );

  public:
    std::string text_;
  };

  // -- Text -------------------------------------------------------------
  class Text : public Element {
  public:
    virtual std::string MakeText( Maker& maker );
  };

  // -- Function ---------------------------------------------------------
  class Function : public Element {
    friend class Maker;
    friend class Parser;

  public:
    explicit Function( bool is_root );

    virtual std::string MakeText( Maker& maker );

    bool BlockExist( void );

  public:
    bool                       is_root_;
    std::string                name_;
    std::optional<int>         argument_integer_;
    std::optional<std::string> argument_string_;
    std::string                block_string_;

  private:
    std::vector<std::shared_ptr<Element>> block_;
  };

  // -- FunctionCloser ---------------------------------------------------
  class FunctionCloser : public Element {
  public:
    virtual std::string MakeText( Maker& maker );
  };

  // -- Parser -----------------------------------------------------------
  class Parser {
  public:
    std::shared_ptr<Function> Parse( const std::string& str );

  private:
    class IllegalFunction {};

    void MoveNext( void );

    std::shared_ptr<Function> ParseAsRoot( void );
    void ParseAsBlock( Function& current_function );
    void ParseAsFunction( Function& child );
    void ParseAsArgument( Function& child );
    void ParseAsArgumentInteger( Function& child );
    void ParseAsArgumentString( Function& child );
    void SkipSpace( void );

  private:
    const char*              current_;
    std::shared_ptr<Element> target_;
  };

  // -- Maker ------------------------------------------------------------
  class Maker {
    friend std::string Function::MakeText( Maker& maker );

  public:
    using FunctionCallback = std::function<std::string ( Function& )>;

    void SetFunction( const std::string& key, FunctionCallback callback );
    FunctionCallback& operator []( const std::string& key );

    std::string MakeText( const std::string& str );

  private:
    std::unordered_map<std::string, FunctionCallback> table_;
  };
}
