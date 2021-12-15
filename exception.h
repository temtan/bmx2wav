// exception.h

#pragma once

#include "tt_exception.h"
#include "tt_enum.h"

#include "tt_squirrel_object.h" 

#include "base/word.h"

#include "common.h"


namespace BMX2WAV {
  // -- 多重継承用 群 -----
  // FilePath 保持用
  class WithFilePathException {
  protected:
    explicit WithFilePathException( const std::string& path ) : path_( path ) {}
  public:
    const std::string& GetFilePath( void ) const { return path_; }
  private:
    const std::string path_;
  };

  // BL::Word 保持用
  class WithWordException {
  protected:
    explicit WithWordException( BL::Word word ) : word_( word ) {}
  public:
    BL::Word GetWord( void ) const { return word_; }
  private:
    BL::Word word_;
  };

  // Channel Number 保持用
  class WithChannelNumberException : private WithWordException {
  protected:
    explicit WithChannelNumberException( BL::Word channel_number ) : WithWordException( channel_number ) {}
  public:
    BL::Word GetChannelNumber( void ) const { return this->GetWord(); }
  };

  // Object Number 保持用
  class WithObjectNumberException : private WithWordException {
  protected:
    explicit WithObjectNumberException( BL::Word object_number ) : WithWordException( object_number ) {}
  public:
    BL::Word GetObjectNumber( void ) const { return this->GetWord(); }
  };

  // ObjectWithLocation 保持用
  class WithObjectWithLocationException {
  protected:
    explicit WithObjectWithLocationException( BL::ObjectWithLocation object ) : object_( object ) {}
  public:
    BL::ObjectWithLocation GetObjectWithLocation( void ) const { return object_; }
  private:
    BL::ObjectWithLocation object_;
  };

  // 多重継承用
  // errno 保持用
  class WithErrorNumberException {
  protected:
    explicit WithErrorNumberException( int error_number ) : error_number_( error_number ) {}
  public:
    int GetErrorNumber( void ) const { return error_number_; }
    std::string GetSystemErrorMessage( void ) const {
      return TtUtility::GetANSIErrorMessage( error_number_ );
    }
  private:
    int error_number_;
  };

  // 多重継承用
  // bar number 保持用
  class WithBarNumberException {
  protected:
    explicit WithBarNumberException( unsigned int bar_number  ) : bar_number_( bar_number ) {}
  public:
    unsigned int GetBarNumber( void ) const { return bar_number_; }
  private:
    unsigned int bar_number_;
  };

  // -- Exception --------------------------------------------------------
  class Exception : public TtException {
  public:
    explicit Exception( void );
    virtual std::string GetMessage( void ) = 0;
  };

  // -- InternalException ------------------------------------------------
  class InternalException : public Exception {
  public:
    explicit InternalException( const char* file, unsigned int line );

    const char*  GetFile( void ) const;
    unsigned int GetLine( void ) const;

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    const char*  file_;
    unsigned int line_;
  };
#define BMX2WAV_INTERNAL_EXCEPTION BMX2WAV::InternalException( __FILE__, __LINE__ )

  // -- LanguageException ------------------------------------------------
  class LanguageException : public Exception {
  public:
    explicit LanguageException( const std::string& language );

    const std::string& GetLanguage( void );

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    const std::string language_;
  };

  // =====================================================================
  // -- ConvertException -------------------------------------------------
  class ConvertException : public Exception {
  public:
    explicit ConvertException( ErrorLevel error_level );

    TtEnum<ErrorLevel> GetErrorLevel( void ) const;

    void ResetErrorLevel( TtEnum<ErrorLevel> error_level );

  protected:
    ErrorLevel error_level_;

  public:
    std::optional<TtSquirrel::Object> squirrel_object_;
  };

  template <ErrorLevel default_level>
  class ConvertExceptionWith : public ConvertException {
  public:
    explicit ConvertExceptionWith( void ) : ConvertException( default_level ) {}
  };

  // -- MessageOnlyException -------------------------------------------------
  class MessageOnlyException : public ConvertExceptionWith<ErrorLevel::Warning> {
  public:
    explicit MessageOnlyException( const std::string& message );

    virtual std::string GetMessage( void ) override;

  private:
    std::string message_;
  };

  // -- ConvertBmsDescriptionException -----------------------------------
  class ConvertBmsDescriptionException;
  // -> base/parser.h

  // == For Wav File =====================================================
  // -- AudioFileError ---------------------------------------------------
  class AudioFileError : public ConvertException,
                         public WithFilePathException {
  public:
    explicit AudioFileError( ErrorLevel error_level, const std::string& path );

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) = 0;
  };

  // -----
  template <ErrorLevel default_level>
  class AudioFileErrorWith : public AudioFileError {
  public:
    explicit AudioFileErrorWith( const std::string& path ) : AudioFileError( default_level, path ) {}
  };

  // -- EntriedAudioFileNotFoundException ----------------------------------
  class EntriedAudioFileNotFoundException : public AudioFileErrorWith<ErrorLevel::NeedFix>,
                                            public WithObjectNumberException {
  public:
    explicit EntriedAudioFileNotFoundException( BL::Word object_number, const std::string& path );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }
  };

  // -- AudioFileOpenException ---------------------------------------------
  class AudioFileOpenException : public AudioFileErrorWith<ErrorLevel::NeedFix>,
                                 public WithErrorNumberException {
  public:
    explicit AudioFileOpenException( const std::string& path, int error_number );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }
  };

  // -- WavFileReadException ---------------------------------------------
  class WavFileReadException : public AudioFileErrorWith<ErrorLevel::NeedFix>,
                               public WithErrorNumberException {
  public:
    explicit WavFileReadException( const std::string& path, int error_number );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }
  };

  // -- WavFileWriteException --------------------------------------------
  class WavFileWriteException : public AudioFileErrorWith<ErrorLevel::Fatal>,
                                public WithErrorNumberException {
  public:
    explicit WavFileWriteException( const std::string& path, int error_number );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }
  };

  // -- OggFileOpenException ---------------------------------------------
  class OggFileOpenException : public AudioFileErrorWith<ErrorLevel::NeedFix>,
                               public WithErrorNumberException {
  public:
    explicit OggFileOpenException( const std::string& path, int error_number );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }

  private:
    int error_number_;
  };

  // -- OggFileReadException ---------------------------------------------
  class OggFileReadException : public AudioFileErrorWith<ErrorLevel::NeedFix> {
  public:
    explicit OggFileReadException( const std::string& path );

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }
  };

  // -- InvalidWavFileFormatException ------------------------------------
  class InvalidWavFileFormatException : public AudioFileErrorWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidWavFileFormatException( const std::string& path, const std::string& reason );

    const std::string& GetReason( void ) const;

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }

  private:
    std::string reason_;
  };

  // -- UnsupportedWavFileFormatException ------------------------------------
  class UnsupportedWavFileFormatException : public AudioFileErrorWith<ErrorLevel::NeedFix> {
  public:
    explicit UnsupportedWavFileFormatException( const std::string& path, const std::string& reason );

    const std::string& GetReason( void ) const;

    virtual std::string GetMessage( void ) override;

    virtual std::shared_ptr<AudioFileError> ToSharedPointer( void ) override {
      return std::shared_ptr<AudioFileError>( new std::remove_pointer<decltype( this )>::type( *this ) );
    }

  private:
    std::string reason_;
  };
  // == End - For Wav File ===============================================

  // -- BadAllocationException -------------------------------------------
  class BadAllocationException : public ConvertExceptionWith<ErrorLevel::ImmediatelyAbort> {
  public:
    explicit BadAllocationException( std::bad_alloc source );

    virtual std::string GetMessage( void ) override;

  private:
    std::bad_alloc source_;
  };

  // -- BmsFileAccessException -------------------------------------------
  class BmsFileAccessException : public ConvertExceptionWith<ErrorLevel::ImmediatelyAbort>,
                                 public WithFilePathException {
  public:
    explicit BmsFileAccessException( const std::string& path, int error_number );

    int GetErrorNumber( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    int error_number_;
  };

  // -- OutputFileIsInputFilePathException -------------------------------
  class OutputFileIsInputFilePathException : public ConvertExceptionWith<ErrorLevel::Fatal>,
                                             public WithObjectNumberException,
                                             public WithFilePathException {
  public:
    explicit OutputFileIsInputFilePathException( BL::Word object_number, const std::string& path );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidFormatAsBpmHeaderException --------------------------------
  class InvalidFormatAsBpmHeaderException : public ConvertExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidFormatAsBpmHeaderException( void );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidFormatAsExtendedBpmChangeValueException -------------------
  class InvalidFormatAsExtendedBpmChangeValueException : public ConvertExceptionWith<ErrorLevel::NeedFix>,
                                                         public WithObjectNumberException {
  public:
    explicit InvalidFormatAsExtendedBpmChangeValueException( BL::Word object_number );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidFormatAsStopSequenceException -----------------------------
  class InvalidFormatAsStopSequenceException : public ConvertExceptionWith<ErrorLevel::NeedFix>,
                                               public WithObjectNumberException {
  public:
    explicit InvalidFormatAsStopSequenceException( BL::Word object_number );

    virtual std::string GetMessage( void ) override;
  };

  // -- InvalidFormatAsLongNoteObjectHeaderException ---------------------
  class InvalidFormatAsLongNoteObjectHeaderException : public ConvertExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit InvalidFormatAsLongNoteObjectHeaderException( void );

    virtual std::string GetMessage( void ) override;
  };

  // -- LongNoteObjectInvalidEncloseException ----------------------------
  class LongNoteObjectInvalidEncloseException : public ConvertExceptionWith<ErrorLevel::NeedFix> {
  public:
    explicit LongNoteObjectInvalidEncloseException( BL::ObjectWithLocation start, BL::ObjectWithLocation end );

    virtual std::string GetMessage( void ) override;

    BL::ObjectWithLocation GetStartObject( void );
    BL::ObjectWithLocation GetEndObject( void );

  private:
    BL::ObjectWithLocation start_;
    BL::ObjectWithLocation end_;
  };

  // -- LongNoteObjectNotEnclosedException -------------------------------
  class LongNoteObjectNotEnclosedException : public ConvertExceptionWith<ErrorLevel::Tiny>,
                                             public WithObjectWithLocationException {
  public:
    explicit LongNoteObjectNotEnclosedException( BL::ObjectWithLocation object );

    virtual std::string GetMessage( void ) override;
  };

  // -- NotEntriedWavWasUsedException ------------------------------------
  class NotEntriedWavWasUsedException : public ConvertExceptionWith<ErrorLevel::Tiny>,
                                        public WithObjectWithLocationException {
  public:
    explicit NotEntriedWavWasUsedException( BL::ObjectWithLocation object );

    virtual std::string GetMessage( void ) override;
  };
 
  // -- InvalidFormatAsBpmChangeValueException ---------------------------
  class InvalidFormatAsBpmChangeValueException : public ConvertExceptionWith<ErrorLevel::NeedFix>,
                                                 public WithBarNumberException,
                                                 public WithObjectNumberException {
  public:
    explicit InvalidFormatAsBpmChangeValueException( unsigned int bar_number, BL::Word object_number );

    virtual std::string GetMessage( void ) override;
  };

  // -- ExtendedBpmChangeEntryNotExistException --------------------------
  class ExtendedBpmChangeEntryNotExistException : public ConvertExceptionWith<ErrorLevel::NeedFix>,
                                                  public WithBarNumberException,
                                                  public WithObjectNumberException {
  public:
    explicit ExtendedBpmChangeEntryNotExistException( unsigned int bar_number, BL::Word object_number );

    virtual std::string GetMessage( void ) override;
  };

  // -- StopSequenceEntryNotExistException -------------------------------
  class StopSequenceEntryNotExistException : public ConvertExceptionWith<ErrorLevel::NeedFix>,
                                             public WithBarNumberException,
                                             public WithObjectNumberException {
  public:
    explicit StopSequenceEntryNotExistException( unsigned int bar_number, BL::Word object_number );

    virtual std::string GetMessage( void ) override;
  };

  // -- OutputFileAccessException ----------------------------------------
  class OutputFileAccessException : public ConvertExceptionWith<ErrorLevel::ImmediatelyAbort>,
                                    public WithFilePathException,
                                    public WithErrorNumberException {
  public:
    explicit OutputFileAccessException( AudioFileOpenException& parent );

    virtual std::string GetMessage( void ) override;
  };
}
