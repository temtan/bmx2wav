// base/parser_exception.h

#pragma once


namespace BMX2WAV::BL::Parser {
  // -- ParseException ---------------------------------------------------
  class ParseException : public BMX2WAV::Exception {};

  // -- FileAccessException -----
  class FileAccessException : public ParseException {
  public:
    explicit FileAccessException( TtFileAccessException& ex );

    const std::string& GetPath( void ) const;
    int                GetErrorNumber( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    std::string path_;
    int         error_number_;
  };

  // -- BmsDescriptionException ------------------------------------------
  class BmsDescriptionException : public ParseException {
  public:
    explicit BmsDescriptionException( RawLine& line, ErrorLevel error_level );

  public:
    const RawLine      GetLine( void ) const;
    TtEnum<ErrorLevel> GetErrorLevel( void ) const;

    void ResetErrorLevel( TtEnum<ErrorLevel> error_level );

    // std::string GetStandardFormatedMessage( const std::string& path );

  protected:
    RawLine    line_;
    ErrorLevel error_level_;
  };

  // -- BmsDescriptionCustomMessageException -----------------------------
  class BmsDescriptionCustomMessageException : public BmsDescriptionException {
  public:
    explicit BmsDescriptionCustomMessageException( RawLine& line, ErrorLevel level, const std::string& message );
    virtual std::string GetMessage( void ) override;

  private:
    std::string message_;
  };

  template <ErrorLevel default_level>
  class BmsDescriptionExceptionWith : public BmsDescriptionException {
  public:
    explicit BmsDescriptionExceptionWith( RawLine& line, ErrorLevel level = default_level ) : BmsDescriptionException( line, level ) {}
  };

  // Header ï€éùóp
  class WithHeader {
  protected:
    explicit WithHeader( Header& header ) : header_( header ) {}
  public:
    const Header& GetHeader( void ) const { return header_; }
  private:
    Header header_;
  };

  // Channel ï€éùóp
  class WithChannel {
  protected:
    explicit WithChannel( Channel& channel ) : channel_( channel ) {}
  public:
    const Channel& GetChannel( void ) const { return channel_; }
  private:
    Channel channel_;
  };

  // -- NoBeginningEndRandomException ------------------------------------
  class NoBeginningEndRandomException : public BmsDescriptionExceptionWith<ErrorLevel::Tiny> {
  public:
    explicit NoBeginningEndRandomException( EndRandom& end_random );

    const EndRandom& GetEndRandom( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    EndRandom end_random_;
  };

  // -- HeaderCollisionException -----------------------------------------
  class HeaderCollisionException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                   public WithHeader {
  public:
    explicit HeaderCollisionException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidBpmException ----------------------------------------------
  class InvalidBpmException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                              public WithHeader {
  public:
    explicit InvalidBpmException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidLnobjException --------------------------------------------
  class InvalidLnobjException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                public WithHeader {
  public:
    explicit InvalidLnobjException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- ObjectCollisionException -----------------------------------------
  class ObjectCollisionException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                   public WithChannel {
  public:
    explicit ObjectCollisionException( Channel& channel, BL::Word old_word, BL::Word new_word );

    BL::Word GetOldWord( void ) const;
    BL::Word GetNewWord( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    BL::Word old_word_;
    BL::Word new_word_;
  };

  // -- BarLengthChangeCollisionException --------------------------------
  class BarLengthChangeCollisionException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit BarLengthChangeCollisionException( BarLengthChange& bar_length_change );

    const BarLengthChange& GetBarLengthChange( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    BarLengthChange bar_length_change_;
  };

  // -- InvalidChannelOrHeaderSyntaxException ----------------------------
  class InvalidChannelOrHeaderSyntaxException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidChannelOrHeaderSyntaxException( RawLine& raw_line );

    virtual std::string GetMessage( void ) override;
  };

  // -- CorrespondingEndIfIsNotingException ------------------------------
  class CorrespondingEndIfIsNotingException : public BmsDescriptionExceptionWith<ErrorLevel::Tiny> {
  public:
    explicit CorrespondingEndIfIsNotingException( Block& block );

    const Block& GetBlock( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    Block block_;
  };

  // -- InvalidRandomStatementValueException -----------------------------
  class InvalidRandomStatementValueException : public BmsDescriptionExceptionWith<ErrorLevel::Fatal> {
  public:
    explicit InvalidRandomStatementValueException( RawLine& raw_line );

    virtual std::string GetMessage( void ) override;
  };

  // -- CorrespondingIfIsNotingException ---------------------------------
  class CorrespondingIfIsNotingException : public BmsDescriptionExceptionWith<ErrorLevel::Tiny> {
  public:
    explicit CorrespondingIfIsNotingException( RawLine& raw_line );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidBarLengthChangeValueException -----------------------------
  class InvalidBarLengthChangeValueException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidBarLengthChangeValueException( RawLine& raw_line );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidObjectExpressionException ---------------------------------
  class InvalidObjectExpressionException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidObjectExpressionException( RawLine& raw_line );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidExtendedBpmException --------------------------------------
  class InvalidExtendedBpmException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                      public WithHeader {
  public:
    explicit InvalidExtendedBpmException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidStopSequenceException -------------------------------------
  class InvalidStopSequenceException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                       public WithHeader {
  public:
    explicit InvalidStopSequenceException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- UndefinedObjectUsedException -------------------------------------
  class UndefinedObjectUsedException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                       public WithChannel {
  public:
    explicit UndefinedObjectUsedException( Channel& channel, BL::Word used_object );

    BL::Word GetUsedObject( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    BL::Word used_object_;
  };

  // -- DefinedObjectNotUsedException ------------------------------------
  class DefinedObjectNotUsedException : public BmsDescriptionExceptionWith<ErrorLevel::NeedFix>,
                                        public WithHeader {
  public:
    explicit DefinedObjectNotUsedException( Header& header );

    virtual std::string GetMessage( void ) override;
  };

  // -- ResolutionExceededLimitException ---------------------------------
  class ResolutionExceededLimitException : public BmsDescriptionExceptionWith<ErrorLevel::ImmediatelyAbort>,
                                           public WithChannel {
  public:
    explicit ResolutionExceededLimitException( Channel& channel, unsigned int required_resolution );

    unsigned int GetRequiredResolution( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    unsigned int required_resolution_;
  };
}


namespace BMX2WAV {
  // -- ConvertBmsDescriptionException -----------------------------------
  class ConvertBmsDescriptionException : public ConvertException {
  public:
    explicit ConvertBmsDescriptionException( BL::Parser::BmsDescriptionException& origin );

    const BL::Parser::RawLine GetLine( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    BL::Parser::RawLine line_;
    std::string         message_;
  };
}
