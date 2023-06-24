// base/bms_data.h

#pragma once

#include "tt_enum.h"

#include "base/register_array.h"
#include "base/bar.h"


namespace BMX2WAV::BL {
  // -- BmsData -----------------------------------------------------------
  class BmsData {
  public:
    explicit BmsData( void );

    unsigned int GetBarNumberLastObjectExists( void );

    unsigned int GetObjectCount( void );
    unsigned int GetObjectCountOf( const std::vector<BL::Word>& should_count_channel );
    unsigned int GetObjectCountOf( std::function<bool ( BL::Word channel_number )> should_count_channel );
    unsigned int GetObjectCountOf1P( void );
    unsigned int GetObjectCountOf2P( void );

    unsigned int GetMaxResolution( void );
    unsigned int GetBarNumberOfMaxResolution( void );

    double CalculatePlayingTime( void );

    std::string ConvertToFileFormat( void );

  public:
    std::unordered_map<std::string, std::string> headers_;
    BL::RegisterArray                            wav_array_;
    BL::RegisterArray                            bmp_array_;
    BL::RegisterArray                            extended_bpm_array_;
    BL::RegisterArray                            stop_sequence_array_;
    std::array<BL::Bar, Const::BAR_MAX_COUNT>    bars_;

    std::string        path_;
    TtEnum<ErrorLevel> most_serious_error_level_;
    bool               has_random_statement_;
  };
}
