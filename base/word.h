// base/word.h

#include <compare>

#pragma once

#include <string>
#include <iostream>


namespace BMX2WAV::BL {
  // -- Word -------------------------------------------------------------
  class Word {
  public:
    static const BL::Word MIN;
    static const BL::Word MAX;

    static bool CanConstructAsWord( const char* str );
    static bool CanConstructAsWord( const std::string& str );

  public:
    explicit Word( void );
    explicit Word( int value );
    explicit Word( char msb, char lsb );
    explicit Word( const char* str );
    explicit Word( const std::string& str );

    operator int() const;

    void Increase( void );

    int ToInteger( void ) const;
    char GetMSB( void ) const;
    char GetLSB( void ) const;
    const char* ToCharPointer( void ) const;
    std::string ToString( void ) const;

    void Validate( void ) const;

    friend std::ostream& operator <<( std::ostream& os, const BL::Word& word );

    auto operator <=>( const BL::Word& ) const = default;

  private:
    int value_;
  };

  // User-defined literals
  BL::Word operator "" _hex36( const char* str, std::size_t size );


  // -- ObjectWithLocation -----------------------------------------------
  class ObjectWithLocation {
  public:
    ObjectWithLocation( void ) = default;
    explicit ObjectWithLocation( unsigned int bar_number, BL::Word channel_number, BL::Word object_number );

    unsigned int bar_number_;
    BL::Word     channel_number_;
    BL::Word     object_number_;
  };
}

// hash
namespace std {
  template <>
  class hash<BMX2WAV::BL::Word> {
  public:
    std::size_t operator ()( const BMX2WAV::BL::Word &word ) const { return static_cast<std::size_t>( word.ToInteger() ); }
  };
}
