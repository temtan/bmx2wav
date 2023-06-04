// utility.h

#pragma once

#include <string>
#include <optional>


namespace BMX2WAV::Utility {
  int  Hex36ToInteger( char c );
  char IntegerToHex36( int x );

  bool IsDigit( char c );
  bool IsNotDigit( char c );
  bool IsAlphabet( char c );
  bool IsNotAlphabet( char c );
  bool IsHex36( char c );
  bool IsNotHex36( char c );

  double Round( double x );

  unsigned int GetDenominator( double x, unsigned int max = 65536 );

  bool KeyEqual( const std::string& s1, const std::string& s2 );

  bool MakeDirectoryOfFileIfNotExist( const std::string& path );

  int64_t GetCurrentProcessWorkingSetSize( void );

  bool UserDefaultLocaleIsJapanese( void );
  std::string GetLanguageDirectoryPath( void );

  std::string GetNotExistPathFrom( const std::string& path );
  std::string RemoveCanNotUseCharacterAsFilePathFrom( const std::string& path );

  // -- Format -----------------------------------------------------------
  class FormatEndMark {};

  inline std::string Format( const std::string& format ) {
    return format;
  };
  inline std::string Format( const char* format ) {
    return format;
  };

  template <class... Rest>
  std::string FormatSubstance( const char* format, Rest&&... rest ) {
    char tmp[1024];
    ::sprintf_s( tmp, sizeof( tmp ), format, rest... );
    return tmp;
  };

  template <class... Rest>
  std::string Format( const char* format, Rest&&... rest ) {
    return FormatArgumentTurning( format, std::forward<Rest>( rest )..., FormatEndMark() );
  }
  template <class... Rest>
  std::string Format( const std::string& format, Rest&&... rest ) {
    return Format( format.c_str(), rest... );
  };

  template <class Head, class... Args>
  std::string FormatArgumentTurning( const char* format, Head&& head, Args&&... args ) {
    return FormatArgumentTurning( format, std::forward<Args>( args )..., head );
  }
  template <class... Args>
  std::string FormatArgumentTurning( const char* format, std::string& str, Args&&... args ) {
    return FormatArgumentTurning( format, std::forward<Args>( args )..., str.c_str() );
  }
  template <class... Args>
  std::string FormatArgumentTurning( const char* format, const std::string& str, Args&&... args ) {
    return FormatArgumentTurning( format, std::forward<Args>( args )..., str.c_str() );
  }
  template <class... Args>
  std::string FormatArgumentTurning( const char* format, FormatEndMark&&, Args&&... args ) {
    return FormatSubstance( format, std::forward<Args>( args )... );
  }

  // -- TextFileReader ---------------------------------------------------
  class TextFileReader {
  public:
    explicit TextFileReader( const std::string& path );
    ~TextFileReader();

    void SetReadAsUTF8( bool flag );

    void Close( void );

    std::optional<std::string> ReadLine( void );

  private:
    const std::string path_;
    FILE* file_;
    bool  read_as_utf8_;
  };
}
