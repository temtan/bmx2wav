// mainstay/settings.h

#pragma once

namespace BMX2WAV::Mainstay {
  // -- Settings ---------------------------------------------------------
  class Settings {
  public:
    static std::string GetLanguageFromFile( const std::string& path );

  public:
    explicit Settings( void );

    void ReadFromFile( const std::string& path );
    void WriteToFile( const std::string& path ) const;

  public:
    std::string language_;
  };
}
