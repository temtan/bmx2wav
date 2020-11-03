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

  std::string GetNotExistPathFrom( const std::string& path );
  std::string RemoveCanNotUseCharacterAsFilePathFrom( const std::string& path );

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
