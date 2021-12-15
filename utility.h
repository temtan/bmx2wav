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

  std::string GetNotExistPathFrom( const std::string& path );
  std::string RemoveCanNotUseCharacterAsFilePathFrom( const std::string& path );

  inline std::string Format( const std::string& format ) {
    return format;
  };
  inline std::string Format( const char* format ) {
    return format;
  };
  template <class... Rest>
  std::string Format( const std::string& format, Rest&&... rest ) {
    return Format( format.c_str(), rest... );
  };
  template <class... Rest>
  std::string Format( const char* format, Rest&&... rest ) {
    char tmp[1024];
    ::sprintf_s( tmp, sizeof( tmp ), format, rest... );
    return tmp;
  };

  // -- TextFileReader ---------------------------------------------------
  class TextFileReader {
  public:
    explicit TextFileReader( const std::string& path );
    ~TextFileReader();

    void Close( void );

    std::optional<std::string> ReadLine( void );

  private:
    const std::string path_;
    FILE* file_;
  };
}
