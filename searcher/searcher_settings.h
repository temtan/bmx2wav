// searcher/searcher_settings.h

#pragma once

namespace BMX2WAV::Searcher {
  // -- Settings ---------------------------------------------------------
  class Settings {
  public:
    explicit Settings( void );

    void ReadFromFile( const std::string& path );
    void WriteToFile( const std::string& path ) const;

  public:
    std::string home_folder_;
    bool        auto_display_cells_;
    bool        asynchronous_display_;
  };
}
