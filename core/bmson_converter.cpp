// core/bmson_converter.cpp

#include <numeric>

#include "base/word.h"

#include "core/bmson_converter.h"

using namespace BMX2WAV;


namespace {
  using namespace BL::Bmson;

  class PositionConverter {
  public:
    explicit PositionConverter( BL::BmsData& bms ) : bms_( bms ) {}

    void InitializeAsDefault( uint64_t resolution ) {
      for ( unsigned int i = 0; i < table_.size(); ++i ) {
        table_[i].bar_number_ = i;
        table_[i].start_position_ = resolution * 4 * i;
        table_[i].length_ = resolution * 4;
      }
      max_use_bar_number_ = static_cast<int>( table_.size() - 2 );
    }

    void InitializeByBmson( const std::vector<BmsonData::BarLine>& lines, uint64_t resolution ) {
      std::vector<BmsonData::BarLine> tmp = lines;
      std::sort( tmp.begin(), tmp.end(), [] ( auto& x, auto& y ) {
        return x.position_ < y.position_;
      } );

      unsigned int lines_start_bar_number = 0;
      if ( tmp.front().position_ != 0 ) {
        table_[0].bar_number_ = 0;
        table_[0].start_position_ = 0;
        max_use_bar_number_ = -1;
        lines_start_bar_number = 1;
      }
      for ( unsigned int i = lines_start_bar_number; BmsonData::BarLine& line : tmp ) {
        if ( i > Const::BAR_MAX_COUNT ) {
          throw BarIsOutOfBmsRangeException();
        }
        table_[i].bar_number_ = i;
        table_[i].start_position_ = line.position_;
        max_use_bar_number_ = static_cast<int>( i ) - 1;

        if ( i > 0 ) {
          uint64_t length = table_[i].start_position_ - table_[i - 1].start_position_;
          table_[i - 1].length_ = length;
          if ( length != resolution * 4 ) {
            bms_.bars_[i - 1].SetRatio( static_cast<double>( length ) / static_cast<double>( resolution * 4 ) );
          }
        }

        ++i;
      }
    }

    void ConvertPositionAndCall( uint64_t position_of_bmson, std::function<void ( BL::Bar& bar, unsigned int position_of_bar )> func ) {
      auto get_bar_info = [&] ( uint64_t p ) -> BarInformation& {
        for ( unsigned int i = 1; i < table_.size(); ++i ) {
          if ( p < table_[i].start_position_ ) {
            return table_[i - 1];
          }
          if ( i > max_use_bar_number_ ) {
            throw BmsonObjectIsOutOfBmsonLineRangeException();
          }
        }
        throw BarIsOutOfBmsRangeException();
      };

      const BarInformation& bar_info = get_bar_info( position_of_bmson );
      BL::Bar& bar = bms_.bars_[bar_info.bar_number_];
      auto position_of_bar = position_of_bmson - bar_info.start_position_;

      unsigned int new_resolution = std::lcm( bar.GetResolution(), static_cast<unsigned int>( std::gcd( bar_info.length_, position_of_bar ) ) );
      bar.MultiplyResolution( new_resolution / bar.GetResolution() );

      func( bar, static_cast<unsigned int>( position_of_bar * new_resolution / bar_info.length_ ) );
    }

  public:
    struct BarInformation {
      unsigned int bar_number_;
      uint64_t start_position_;
      uint64_t length_;
    };

    BL::BmsData& bms_;
    std::array<BarInformation, Const::BAR_MAX_COUNT + 1> table_;
    int max_use_bar_number_ = -1;
  };
}


// -- BmsonConverter -----------------------------------------------------
Core::BmsonConverter::BmsonConverter( void )
{
}


std::shared_ptr<BL::BmsData>
Core::BmsonConverter::ConvertForWaveConvert( BL::Bmson::BmsonData& bmson )
{
  using BL::operator "" _hex36;

  std::shared_ptr<BL::BmsData> bms = std::make_shared<BL::BmsData>();

  // -- Information
  bms->headers_["TITLE"]    = bmson.info_.title_;
  bms->headers_["SUBTITLE"] = bmson.info_.sub_title_;
  bms->headers_["ARTIST"]   = bmson.info_.artist_;
  {
    std::string tmp;
    for ( auto& str : bmson.info_.sub_artists_ ) {
      if ( NOT( tmp.empty() ) ) {
        tmp.append( ", " );
      }
      tmp.append( str );
    }
    bms->headers_["SUBARTIST"] = tmp;
  }
  bms->headers_["GENRE"]          = bmson.info_.genre_;
  bms->headers_["MODE_HINT"]      = bmson.info_.mode_hint_;
  bms->headers_["BPM"]            = TtUtility::ToStringFrom( bmson.info_.init_bpm_ );
  bms->headers_["JUDGE_RANK"]     = TtUtility::ToStringFrom( bmson.info_.judge_rank_ );
  bms->headers_["TOTAL"]          = TtUtility::ToStringFrom( bmson.info_.total_ );
  bms->headers_["BACK_IMAGE"]     = bmson.info_.back_image_;
  bms->headers_["EYECATCH_IMAGE"] = bmson.info_.eyecatch_image_;
  bms->headers_["BANNER"]         = bmson.info_.banner_image_;
  bms->headers_["PREVIWE_MUSIC"]  = bmson.info_.preview_music_;
  bms->headers_["RESOLUTION"]     = TtUtility::ToStringFrom( bmson.info_.resolution_ );

  // -- BarLine
  PositionConverter position_converter( *bms );
  if ( bmson.lines_.empty() ) {
    position_converter.InitializeAsDefault( bmson.info_.resolution_ );
  }
  else {
    position_converter.InitializeByBmson( bmson.lines_, bmson.info_.resolution_ );
  }

  // -- BpmEvent
  {
    std::sort( bmson.bpm_events_.begin(), bmson.bpm_events_.end(), [] ( auto& x, auto& y ) {
      return x.position_ < y.position_;
    } );
    for ( unsigned int i = 0; BmsonData::BpmEvent& event : bmson.bpm_events_ ) {
      ++i;
      if ( i > Const::WORD_MAX_VALUE ) {
        throw NumberOfObjectsIsOutOfBmsRangeException( "BpmEvent" );
      }

      bms->headers_["BPM" + BL::Word( i ).ToString()] = TtUtility::ToStringFrom( event.bpm_ );
      bms->extended_bpm_array_.GetArray()[i] = std::make_optional<std::string>( TtUtility::ToStringFrom( event.bpm_ ) );

      position_converter.ConvertPositionAndCall( event.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
        bar.GetChannelBy( "08"_hex36 )[position_of_bar] = BL::Word( i );
      } );
    }
  }

  // -- StopEvent
  {
    std::sort( bmson.stop_events_.begin(), bmson.stop_events_.end(), [] ( auto& x, auto& y ) {
      return x.position_ < y.position_;
    } );

    for ( unsigned int i = 0; BmsonData::StopEvent& event : bmson.stop_events_ ) {
      ++i;
      if ( i > Const::WORD_MAX_VALUE ) {
        throw NumberOfObjectsIsOutOfBmsRangeException( "StopEvent" );
      }

      std::string duration_of_bms_string = TtUtility::ToStringFrom( static_cast<double>( event.duration_ ) / ( bmson.info_.resolution_ * 4.0 / 192.0 ) );
      bms->headers_["STOP" + BL::Word( i ).ToString()] = duration_of_bms_string;
      bms->stop_sequence_array_.GetArray()[i] = std::make_optional<std::string>( duration_of_bms_string );

      position_converter.ConvertPositionAndCall( event.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
        bar.GetChannelBy( "09"_hex36 )[position_of_bar] = BL::Word( i );
      } );
    }
  }

  // -- SoundChannel
  for ( unsigned int i = 0; BmsonData::SoundChannel& channel : bmson.sound_channels_ ) {
    ++i;
    if ( i > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "SoundChannel" );
    }

    bms->headers_["WAV" + BL::Word( i ).ToString()] = channel.name_;
    bms->wav_array_.GetArray()[i] = std::make_optional<std::string>( channel.name_ );

    for ( BmsonData::Note& note : channel.notes_ ) {
      if ( note.continuation_flag_ ) {
        continue;
      }
      if ( BL::Channel::NumberIsInvisibilityObjectChannel( BL::Word( note.lane_ ) ) ||
           BL::Channel::NumberIsLandmineObjectChannel( BL::Word( note.lane_ ) ) ) {
        continue;
      }
      position_converter.ConvertPositionAndCall( note.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
        auto overwrite_word = [&] ( void ) -> bool {
          for ( std::shared_ptr<BL::Channel> channel : bar.GetBgmChannels() ) {
            if ( (*channel)[position_of_bar] == "00"_hex36 ) {
              (*channel)[position_of_bar] = BL::Word( i );
              return true;
            }
          }
          return false;
        };

        if ( NOT( overwrite_word() ) ) {
          BL::Channel& channel = bar.MakeNewBgmChannel();
          channel[position_of_bar] = BL::Word( i );
        }
      } );
    }
  }

  return bms;
}

std::shared_ptr<BL::BmsData>
Core::BmsonConverter::ConvertForWaveConvertFromFile( const std::string& path )
{
  BL::Bmson::Parser bmson_parser;
  std::shared_ptr<BL::Bmson::BmsonData> bmson_data = bmson_parser.Parse( path );
  return this->ConvertForWaveConvert( *bmson_data );
}


void
Core::BmsonConverter::ConvertToFileAndWave( BL::Bmson::BmsonData& bmson )
{
  (void)bmson;
}
