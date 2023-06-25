// base/parser.h

#pragma once

#include "tt_enum.h"

#include "exception.h"

#include "base/bms_data.h"


namespace BMX2WAV::BL::Parser {
  class Parser;

  // -- RawLine ----------------------------------------------------------
  struct RawLine {
    unsigned int number_;
    std::string  orignal_;
    enum class Type {
      Undefined,
      Header,
      Channel,
      FlowControl,
      Other,
    } type_;
    std::string  key_;
    std::string  value_;
  };

  // -- Statement and other ----------------------------------------------
  class Statement {
  public:
    explicit Statement( std::shared_ptr<RawLine> raw_line );
    virtual ~Statement();

    virtual void EvaluateBy( Parser& parser );

  public:
    std::shared_ptr<RawLine> raw_line_;
  };

  // -- RandomStatement -----
  class RandomStatement : public Statement {
  public:
    using Statement::Statement;

  public:
    virtual unsigned int Generate( void ) = 0;

    virtual void EvaluateBy( Parser& parser ) override;
  };

  // -- GenerateRandom -----
  class GenerateRandom : public RandomStatement {
  public:
    explicit GenerateRandom( std::shared_ptr<RawLine> raw_line, unsigned int max );

    virtual unsigned int Generate( void );

  public:
    unsigned int max_;
  };

  // -- SetRandom -----
  class SetRandom : public RandomStatement {
  public:
    explicit SetRandom( std::shared_ptr<RawLine> raw_line, unsigned int value );

    virtual unsigned int Generate( void );

  public:
    unsigned int value_;
  };

  // -- EndRandom -----
  class EndRandom : public Statement {
  public:
    using Statement::Statement;

    virtual void EvaluateBy( Parser& parser ) override;
  };

  // -- Block -----
  class Block : public Statement {
  public:
    explicit Block( std::shared_ptr<RawLine> raw_line, Block* parent );

    virtual void EvaluateBy( Parser& parser ) override;

  public:
    Block*                                  parent_;
    std::shared_ptr<RawLine>                end_line_;
    std::vector<std::shared_ptr<Statement>> statements_;
  };

  // -- RootBlock -----
  class RootBlock : public Block {
  public:
    explicit RootBlock( void );

    void RootEvaluation( Parser& parser );
  };

  // -- IfBlock -----
  class IfBlock : public Block {
  public:
    explicit IfBlock( std::shared_ptr<RawLine> raw_line, Block* parent, unsigned int prerequisite );

    virtual void EvaluateBy( Parser& parser ) override;

  public:
    unsigned int   prerequisite_;
    enum class EvaluationType {
      Default = 0,
      MustApply,
      MustIgnore,
    } evaluation_type_;
  };

  // -- Command -----
  class Command : public Statement {
  public:
    using Statement::Statement;
  };

  // -- Header -----
  class Header : public Command {
  public:
    explicit Header( std::shared_ptr<RawLine> raw_line, const std::string& key, const std::string& value );

    virtual void EvaluateBy( Parser& parser ) override;

  public:
    std::string  key_;
    std::string  value_;
  };

  // -- Channel -----
  class Channel : public Command {
  public:
    explicit Channel( std::shared_ptr<RawLine> raw_line, unsigned int bar_number, BL::Word channel_number );

    virtual void EvaluateBy( Parser& parser ) override;

  public:
    unsigned int bar_number_;
    BL::Word     channel_number_;
    BL::Buffer   objects_;
  };

  // -- BarLengthChange -----
  class BarLengthChange : public Command {
  public:
    explicit BarLengthChange( std::shared_ptr<RawLine> raw_line, unsigned int bar_number, double ratio );

    virtual void EvaluateBy( Parser& parser ) override;

  public:
    unsigned int bar_number_;
    double       ratio_;
  };
}

#include "base/parser_exception.h"


namespace BMX2WAV::BL::Parser {
  // -- Parser -----------------------------------------------------------
  class Parser {
  public:
    explicit Parser( void );

    // throw FileAccessException
    std::shared_ptr<BmsData> Parse( const std::string& path );

  private:
    void Initialize( const std::string& path );
    void ReadFile( void );
    void RandomSyntacticAnalysis( void );
    void StatementEvaluating( void );
    void Finalize( void );

  public:
    struct AbortController {};

    // callback
    struct Callbacks {
      template <class... Args>
      using FunctionOf = std::function<void ( Parser&, Args... )>;

      // abort
      FunctionOf<> aborted_;

      // reaction or progress
      FunctionOf<Channel&, unsigned int> resolution_extended_;
      FunctionOf<const std::string&>     header_detected_;
      FunctionOf<BL::Word>               channel_number_detected_;
      FunctionOf<>                       complete_random_syntactic_analysis_;

      // exception
      FunctionOf<BmsDescriptionException&>               exception_occurred_for_converter_;
      FunctionOf<BmsDescriptionException&>               exception_occurred_;

      FunctionOf<NoBeginningEndRandomException&>         no_beginning_end_random_;
      FunctionOf<HeaderCollisionException&>              header_collision_;
      FunctionOf<InvalidBpmException&>                   invalid_bpm_;
      FunctionOf<InvalidLnobjException&>                 invalid_lnobj_;
      FunctionOf<ObjectCollisionException&>              object_collision_;
      FunctionOf<BarLengthChangeCollisionException&>     bar_length_change_collision_;
      FunctionOf<InvalidChannelOrHeaderSyntaxException&> invalid_channel_or_header_syntax_;
      FunctionOf<CorrespondingEndIfIsNotingException&>   corresponding_end_if_is_nothing_;
      FunctionOf<InvalidRandomStatementValueException&>  invalid_random_statement_value_;
      FunctionOf<CorrespondingIfIsNotingException&>      corresponding_if_is_nothing_;
      FunctionOf<InvalidBarLengthChangeValueException&>  invalid_bar_length_change_;
      FunctionOf<InvalidObjectExpressionException&>      invalid_object_expression_;
      FunctionOf<InvalidExtendedBpmException&>           invalid_extended_bpm_;
      FunctionOf<InvalidStopSequenceException&>          invalid_stop_sequence_;
      FunctionOf<UndefinedObjectUsedException&>          undefined_object_used_;
      FunctionOf<DefinedObjectNotUsedException&>         defined_object_not_used_;
      FunctionOf<ResolutionExceededLimitException&>      resolution_exceeded_limit_;
    };

    template <class Exception>
    void SafeErrorCallbackOf( std::shared_ptr<Exception> exception, Callbacks::FunctionOf<Exception&> f );

    template <auto Callbacks::* callback, class... Args>
    void SafeErrorCallback( Args... args );

    void CauseErrorWithoutCallback( std::shared_ptr<BmsDescriptionException> exception );

    template <auto Callbacks::* callback, class... Args>
    void SafeCallback( Args... args );

  public:
    // parse settings
    bool               must_read_as_utf8_;
    bool               not_nesting_if_statement_;
    TtEnum<ErrorLevel> must_abort_error_level_;
    unsigned int       bar_resolution_max_;

    // parsing info
    struct Frame {
      explicit Frame( void );

      BmsData                                                bms_data_;
      std::vector<std::shared_ptr<RawLine>>                  raw_data_;
      RootBlock                                              root_;
      std::vector<unsigned int>                              random_value_stack_;
      std::array<bool, Const::BAR_MAX_COUNT>                 bar_length_changed_;
      std::vector<std::function<void ( Parser& parser )>>    lazy_eavluation_;
      std::vector<std::shared_ptr<BmsDescriptionException>>  exceptions_;
      bool                                                   need_to_abort_;
      std::unordered_map<BL::Word, std::pair<Header&, bool>> wav_used_table_;
      std::unordered_map<BL::Word, std::pair<Header&, bool>> bmp_used_table_;
      std::unordered_map<BL::Word, std::pair<Header&, bool>> extended_bpm_used_table_;
      std::unordered_map<BL::Word, std::pair<Header&, bool>> stop_sequence_used_table_;
      BL::Word                                               lnobj_word_;
      bool                                                   skip_parse_line_;
    };

    std::optional<Frame> frame_;
    Callbacks            callbacks_;
  };
}
