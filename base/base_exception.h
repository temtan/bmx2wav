// base/base_exception.h

#pragma once

#include "exception.h"


namespace BMX2WAV::BL {
  // -- BaseLibraryException --------------------------------------------------------
  class BaseLibraryException : public BMX2WAV::Exception {
  public:
    explicit BaseLibraryException( void );
  };

  // -- InvalidCharUsedAsWordException -----------------------------------
  class InvalidCharUsedAsWordException : public BaseLibraryException {
  public:
    explicit InvalidCharUsedAsWordException( char msb, char lsb );

    char GetMsb( void ) const;
    char GetLsb( void ) const;
    std::string GetAsString( void ) const;

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    char msb_;
    char lsb_;
  };

  // -- InvalidWordValueUsedException ------------------------------------
  class InvalidWordValueUsedException : public BaseLibraryException {
  public:
    explicit InvalidWordValueUsedException( int value );

    int GetValue( void ) const;

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    int value_;
  };

  // -- BufferOutOfRangeAccessException ----------------------------------
  class BufferOutOfRangeAccessException : public BaseLibraryException {
  public:
    explicit BufferOutOfRangeAccessException( unsigned int position );

    unsigned int GetPosition( void ) const;

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    unsigned int position_;
  };

  // -- RequiredResolutionIsTooBigException ------------------------------
  class RequiredResolutionIsTooBigException : public BaseLibraryException {
  public:
    explicit RequiredResolutionIsTooBigException( unsigned int required_resolution );

    unsigned int GetRequiredResolution( void ) const;

    virtual std::string GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    unsigned int required_resolution_;
  };
}
