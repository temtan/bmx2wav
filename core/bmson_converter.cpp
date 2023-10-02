// core/bmson_converter.cpp

#include <numeric>
#include <filesystem>

#include "tt_file_stream.h"
#include "tt_path.h"

#include "utility.h"

#include "base/word.h"

#include "core/wave.h"
#include "core/wave_maker.h"

#include "core/bmson_converter.h"

using namespace BMX2WAV;


namespace {
  using namespace BL::Bmson;
  using BL::operator "" _hex36;

  using BmsonPosition = uint64_t;
  using LaneNumber    = unsigned int;
  using WavePosition  = unsigned int;

  class PositionConverter {
  public:
    explicit PositionConverter( BL::BmsData& bms );

    void InitializeAuto( BmsonData& bmson );
    void InitializeAsDefault( uint64_t resolution );
    void InitializeByBmson( const std::vector<BmsonData::BarLine>& lines, uint64_t resolution );

    using Callback = std::function<void ( BL::Bar& bar, unsigned int position_of_bar )>;
    void ConvertPositionAndCall( BmsonPosition position_of_bmson, Callback func );

  private:
    struct BarInformation {
      unsigned int  bar_number_;
      BmsonPosition start_position_;
      BmsonPosition length_;
    };

    BL::BmsData& bms_;
    std::array<BarInformation, Const::BAR_MAX_COUNT + 1> table_;
    int max_use_bar_number_ = -1;
  };

  void ConvertBmsonInformationToBmsData( BL::Bmson::BmsonData::Information& info, BL::BmsData& bms );
  void ConvertBpmEventToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms );
  void ConvertStopEventToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms );
  void ConvertBgaToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms );

  struct WaveFileTrimingInformation {
    BmsonData::SoundChannel* sound_channel_;
    WavePosition start_;
    WavePosition end_;
  };

  class WaveSynthesisInformation : private std::shared_ptr<std::vector<std::shared_ptr<WaveFileTrimingInformation>>> {
  public:
    using Base = std::shared_ptr<std::vector<std::shared_ptr<WaveFileTrimingInformation>>>;
    using Base::operator ->;
    using Base::operator *;

    explicit WaveSynthesisInformation( void ) : Base( std::make_shared<Base::element_type>() ) {}

    bool operator ==( const WaveSynthesisInformation& other ) const {
      if ( this->get() == other.get() ) {
        return true;
      }
      if ( this->get()->size() != other->size() ) {
        return false;
      }
      for ( unsigned int i = 0; std::shared_ptr<WaveFileTrimingInformation> x : **this ) {
        std::shared_ptr<WaveFileTrimingInformation> y = other->at( i );
        if ( x->sound_channel_ != y->sound_channel_ ) {
          return false;
        }
        if ( x->start_ != y->start_ ) {
          return false;
        }
        if ( x->end_ != y->end_ ) {
          return false;
        }
        ++i;
      }
      return false;
    }

    std::string CreateWaveFileName( void ) const {
      std::string tmp;
      for ( std::shared_ptr<WaveFileTrimingInformation> info : **this ) {
        tmp.append( info->sound_channel_->name_ );
        tmp.append( "-" );
        tmp.append( TtUtility::ToStringFrom( info->start_ ) );
        tmp.append( "-" );
        tmp.append( TtUtility::ToStringFrom( info->end_ ) );
        tmp.append( "+" );
      }
      tmp.pop_back();
      return tmp;
    }

    struct Hash {
      size_t operator ()( const WaveSynthesisInformation& info ) const {
        return reinterpret_cast<size_t>( info.get() );
      }
    };
  };

  struct BmsObjectInformation {
    LaneNumber    lane_;
    BmsonPosition position_;

    std::vector<std::pair<BmsonData::SoundChannel*, BmsonData::Note>> notes_;

    WavePosition time_;
    WaveSynthesisInformation wave_synthesis_information_;
  };
}

// -- PositionConverter --------------------------------------------------
::PositionConverter::PositionConverter( BL::BmsData& bms ) :
bms_( bms )
{
}

void
::PositionConverter::InitializeAuto( BmsonData& bmson )
{
  if ( bmson.lines_.empty() ) {
    this->InitializeAsDefault( bmson.info_.resolution_ );
  }
  else {
    this->InitializeByBmson( bmson.lines_, bmson.info_.resolution_ );
  }
}

void
::PositionConverter::InitializeAsDefault( uint64_t resolution )
{
  for ( unsigned int i = 0; i < table_.size(); ++i ) {
    table_[i].bar_number_ = i;
    table_[i].start_position_ = resolution * 4 * i;
    table_[i].length_ = resolution * 4;
  }
  max_use_bar_number_ = static_cast<int>( table_.size() - 2 );
}

void
::PositionConverter::InitializeByBmson( const std::vector<BmsonData::BarLine>& lines, uint64_t resolution )
{
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

void
::PositionConverter::ConvertPositionAndCall( ::BmsonPosition position_of_bmson, Callback func )
{
  auto get_bar_info = [&] ( BmsonPosition p ) -> BarInformation& {
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

// -- BmsonConverter -----------------------------------------------------
Core::BmsonConverter::BmsonConverter( void )
{
}


std::shared_ptr<BL::BmsData>
Core::BmsonConverter::ConvertForWaveConvert( BL::Bmson::BmsonData& bmson )
{
  std::shared_ptr<BL::BmsData> bms = std::make_shared<BL::BmsData>();

  ::ConvertBmsonInformationToBmsData( bmson.info_, *bms );

  PositionConverter position_converter( *bms );
  position_converter.InitializeAuto( bmson );

  ::ConvertBpmEventToBmsData( bmson, position_converter, *bms );
  ::ConvertStopEventToBmsData( bmson, position_converter, *bms );

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
::ConvertBmsonInformationToBmsData( BL::Bmson::BmsonData::Information& info, BL::BmsData& bms )
{
  bms.headers_["TITLE"]    = info.title_;
  bms.headers_["SUBTITLE"] = info.sub_title_;
  bms.headers_["ARTIST"]   = info.artist_;
  {
    std::string tmp;
    for ( auto& str : info.sub_artists_ ) {
      if ( NOT( tmp.empty() ) ) {
        tmp.append( ", " );
      }
      tmp.append( str );
    }
    bms.headers_["SUBARTIST"] = tmp;
  }
  bms.headers_["GENRE"]          = info.genre_;
  bms.headers_["MODE_HINT"]      = info.mode_hint_;
  bms.headers_["BPM"]            = TtUtility::ToStringFrom( info.init_bpm_ );
  bms.headers_["JUDGE_RANK"]     = TtUtility::ToStringFrom( info.judge_rank_ );
  bms.headers_["TOTAL"]          = TtUtility::ToStringFrom( info.total_ );
  bms.headers_["BACKBMP"]        = info.back_image_;
  bms.headers_["STAGEFILE"]      = info.eyecatch_image_;
  bms.headers_["BANNER"]         = info.banner_image_;
  bms.headers_["PREVIWE_MUSIC"]  = info.preview_music_;
  bms.headers_["RESOLUTION"]     = TtUtility::ToStringFrom( info.resolution_ );
}

void
::ConvertBpmEventToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms )
{
  std::vector<BmsonData::BpmEvent> bpm_events = bmson.bpm_events_;

  std::sort( bpm_events.begin(), bpm_events.end(), [] ( auto& x, auto& y ) {
    return x.position_ < y.position_;
  } );
  for ( unsigned int i = 0; BmsonData::BpmEvent& event : bpm_events ) {
    ++i;
    if ( i > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "BpmEvent" );
    }

    bms.headers_["BPM" + BL::Word( i ).ToString()] = TtUtility::ToStringFrom( event.bpm_ );
    bms.extended_bpm_array_.GetArray()[i] = std::make_optional<std::string>( TtUtility::ToStringFrom( event.bpm_ ) );

    position_converter.ConvertPositionAndCall( event.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
      bar.GetChannelBy( "08"_hex36 )[position_of_bar] = BL::Word( i );
    } );
  }
}

void
::ConvertStopEventToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms )
{
  std::vector<BmsonData::StopEvent> stop_events = bmson.stop_events_;

  std::sort( stop_events.begin(), stop_events.end(), [] ( auto& x, auto& y ) {
    return x.position_ < y.position_;
  } );

  for ( unsigned int i = 0; BmsonData::StopEvent& event : stop_events ) {
    ++i;
    if ( i > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "StopEvent" );
    }

    std::string duration_of_bms_string = TtUtility::ToStringFrom( static_cast<double>( event.duration_ ) / ( bmson.info_.resolution_ * 4.0 / 192.0 ) );
    bms.headers_["STOP" + BL::Word( i ).ToString()] = duration_of_bms_string;
    bms.stop_sequence_array_.GetArray()[i] = std::make_optional<std::string>( duration_of_bms_string );

    position_converter.ConvertPositionAndCall( event.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
      bar.GetChannelBy( "09"_hex36 )[position_of_bar] = BL::Word( i );
    } );
  }
}

void
::ConvertBgaToBmsData( BL::Bmson::BmsonData& bmson, PositionConverter& position_converter, BL::BmsData& bms )
{
  std::unordered_map<uint64_t, BL::Word> id_table;
  auto get_new_bmp_number = [number = BL::Word( 0 )] ( void ) mutable -> BL::Word {
    number.Increase();
    if ( number > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "BgaHeader" );
    }
    return number;
  };

  for ( BmsonData::BgaHeader& header : bmson.bga_.bga_header_ ) {
    BL::Word new_id = get_new_bmp_number();
    id_table[header.id_] = new_id;
    bms.bmp_array_.Register( new_id, header.name_ );
  }

  auto bga_event_process = [&position_converter, &id_table] ( std::vector<BmsonData::BgaEvent>& events_, BL::Word channel_number ) {
    for ( BmsonData::BgaEvent& event : events_ ) {
      position_converter.ConvertPositionAndCall( event.position_, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
        bar.GetChannelBy( channel_number )[position_of_bar] = id_table[event.id_];
      } );
    }
  };
  bga_event_process( bmson.bga_.bga_events_,   "04"_hex36 );
  bga_event_process( bmson.bga_.layer_events_, "07"_hex36 );
  bga_event_process( bmson.bga_.poor_events_,  "06"_hex36 );
}


// =======================================================================
void
Core::BmsonConverter::ConvertToFileAndWave( BL::Bmson::BmsonData& original, const std::string& input_file, const std::string& output_dir )
{
  const std::string input_dir = TtPath::DirName( input_file );

  // clone
  BL::Bmson::BmsonData bmson = original;

  std::shared_ptr<BL::BmsData> bms = std::make_shared<BL::BmsData>();

  ::ConvertBmsonInformationToBmsData( bmson.info_, *bms );

  PositionConverter position_converter( *bms );
  position_converter.InitializeAuto( bmson );

  ::ConvertBpmEventToBmsData( bmson, position_converter, *bms );
  ::ConvertStopEventToBmsData( bmson, position_converter, *bms );
  ::ConvertBgaToBmsData( bmson, position_converter, *bms );

  std::unordered_map<BmsonPosition, std::unordered_map<LaneNumber, BmsObjectInformation>> object_table;

  struct PositionEventInformation {
    std::optional<BmsonData::BpmEvent>  bpm_event_;
    std::optional<BmsonData::StopEvent> stop_event_;
    bool notes_exist_;
  };
  std::map<BmsonPosition, PositionEventInformation> position_event_table;

  for ( BmsonData::SoundChannel& channel : bmson.sound_channels_ ) {
    std::vector<BmsonData::Note> longnote_ends_;

    for ( BmsonData::Note& note : channel.notes_ ) {
      auto register_note = [&] ( BmsonData::Note& tmp ) {
        ::BmsObjectInformation& info = object_table[tmp.position_][tmp.lane_];
        info.lane_     = tmp.lane_;
        info.position_ = tmp.position_;
        info.notes_.push_back( {&channel ,tmp} );

        position_event_table[tmp.position_].notes_exist_ = true;
      };

      // BMSON => 0  : BGM   1-8   : 1P note   9-16  : 2P note
      // BMS   => 01 : BGM   11-1Z : 1P note   21-2Z : 2P note
      switch ( note.lane_ ) {
      case 0:
        note.lane_ = 1;
        break;

      case  9: case 10: case 11: case 12: case 13:
        note.lane_ += 8;
      case 1: case 2: case 3: case 4: case 5:
        note.lane_ += 36;
        break;

      case 14: case 15:
        note.lane_ += 8;
      case 6: case 7:
        note.lane_ += 36 + 2;
        break;

      case 16:
        note.lane_ += 8;
      case 8:
        note.lane_ += 36 - 2;
        break;
      }

      // longnote
      if ( note.length_ != 0 && 1 <= note.lane_ && note.lane_ <= 16 ) {
        // (11 => 51) (1Z => 5Z) (21 => 61) (2Z => 6Z)
        note.lane_ += 4 * 36;

        BmsonData::Note end_note = note;
        end_note.position_ += note.length_;
        register_note( end_note );
      }
      register_note( note );
    }
  }
  for ( BmsonData::BpmEvent& event : bmson.bpm_events_ ) {
    position_event_table[event.position_].bpm_event_ = event;
  }
  for ( BmsonData::StopEvent& event : bmson.stop_events_ ) {
    if ( position_event_table[event.position_].stop_event_ ) {
      position_event_table[event.position_].stop_event_->duration_ += event.duration_;
    }
    else {
      position_event_table[event.position_].stop_event_ = event;
    }
  }

  std::unordered_map<BmsonData::SoundChannel*, std::pair<WavePosition, WaveFileTrimingInformation*>> prev_sound_table;

  {
    double        bpm           = bmson.info_.init_bpm_;
    BmsonPosition prev_position = 0;
    double        prev_time     = 0;

    auto position_span_to_time = [resolution = bmson.info_.resolution_] ( BmsonPosition span, double bpm ) -> double {
      return ( static_cast<double>( span ) *
               (static_cast<double>( 44100 * 60 ) / bpm) /
               (static_cast<double>( resolution )) );
    };

    for ( auto& [position, event_info] : position_event_table ) {
      BmsonPosition position_span = position - prev_position;
      double current_time = prev_time + position_span_to_time( position_span, bpm );

      if ( event_info.notes_exist_ ) {
        for ( auto& [lane_number, object_info] : object_table[position] ) {
          object_info.time_ = static_cast<WavePosition>( current_time );

          for ( auto& [channel, note] : object_info.notes_ ) {
            if ( note.continuation_flag_ ) {
              if ( NOT( prev_sound_table.contains( channel ) ) ) {
                throw BmsonDataContradictionException( "An note without a continuation flag does not exist before an note with a continuation flag." );
              }
              auto& [prev_ch_time, prev_ch_trim] = prev_sound_table[channel];
              // prev_ch_trim->end_ = object_info.time_ - prev_ch_time - prev_ch_trim->start_ - 1;
              prev_ch_trim->end_ = object_info.time_ - prev_ch_time - 1;

              object_info.wave_synthesis_information_->push_back( std::make_shared<WaveFileTrimingInformation>( channel, object_info.time_ - prev_ch_time, 0 ) );
              prev_sound_table[channel] = {prev_ch_time, object_info.wave_synthesis_information_->back().get()};
            }
            else {
              object_info.wave_synthesis_information_->push_back( std::make_shared<WaveFileTrimingInformation>( channel, 0, 0 ) );
              prev_sound_table[channel] = {object_info.time_, object_info.wave_synthesis_information_->back().get()};
            }
          }
        }
      }

      if ( event_info.bpm_event_ ) {
        bpm = event_info.bpm_event_->bpm_;
      }
      prev_time = current_time;
      if ( event_info.stop_event_ ) {
        prev_time += position_span_to_time( event_info.stop_event_->duration_, bpm );
      }
      prev_position = position;
    }
  }

  Core::WaveMaker wave_maker( true );
  std::unordered_map<BmsonData::SoundChannel*, std::shared_ptr<Core::Wave>> origin_wave_table;
  std::unordered_map<WaveSynthesisInformation, std::pair<BL::Word, std::shared_ptr<Core::Wave>>, WaveSynthesisInformation::Hash> new_wave_table;
  auto get_new_wave_number = [number = BL::Word( 0 )] ( void ) mutable -> BL::Word {
    number.Increase();
    if ( number > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "SoundChannel" );
    }
    return number;
  };

  for ( auto& [position, info] : position_event_table ) {
    position_converter.ConvertPositionAndCall( position, [&] ( BL::Bar& bar, unsigned int position_of_bar ) {
      if ( info.notes_exist_ ) {
        for ( auto& [lane_number, object_info] : object_table[position] ) {
          if ( NOT( new_wave_table.contains( object_info.wave_synthesis_information_ ) ) ) {
            new_wave_table[object_info.wave_synthesis_information_] = {get_new_wave_number(), std::make_shared<Core::Wave>()};

            for ( std::shared_ptr<WaveFileTrimingInformation>& trim_info : *object_info.wave_synthesis_information_ ) {
              if ( NOT( origin_wave_table.contains( trim_info->sound_channel_ ) ) ) {
                std::string tmp = input_dir + "\\" + trim_info->sound_channel_->name_;
                origin_wave_table[trim_info->sound_channel_] = wave_maker.MakeNewWaveFromPathAutoExtension( tmp );
              }
            }
            Core::Wave& wave = *new_wave_table[object_info.wave_synthesis_information_].second;
            for ( std::shared_ptr<WaveFileTrimingInformation>& trim_info : *object_info.wave_synthesis_information_ ) {
              // clone
              Core::Wave tmp = *origin_wave_table[trim_info->sound_channel_];
              tmp.Trim( trim_info->start_, trim_info->end_ == 0 ? tmp.GetLength() : trim_info->end_ );
              wave.MixinAt( 0, tmp );
            }
          }

          BL::Channel& channel = bar.GetChannelBy( BL::Word( lane_number ) );
          channel[position_of_bar] = BL::Word( new_wave_table[object_info.wave_synthesis_information_].first );
        }
      }
    } );
  }

  Utility::MakeDirectoryOfFileIfNotExist( output_dir );
  std::string basename = TtPath::RemoveExtension( TtPath::BaseName( input_file ) );
  for ( auto& [info, wave_number_and_wave] : new_wave_table ) {
    auto& [wave_number, wave] = wave_number_and_wave;
    std::string audio_file_name = info.CreateWaveFileName() + ".wav";
    wave->WriteToFile( output_dir + "\\" + audio_file_name );
    bms->wav_array_.Register( wave_number, audio_file_name );
    bms->headers_[std::string( "WAV" ) + wave_number.ToCharPointer()] = audio_file_name;
  }
  {
    TtFileWriter writer( output_dir + "\\" + basename + ".bms", false, false );
    writer.WriteString( bms->ConvertToFileFormat() );
  }
  {
    auto copy_file = [&input_file, &output_dir] ( const std::string& target ) {
      if ( target.empty() ) {
        return;
      }
      std::string from = TtPath::DirName( input_file ) + "\\" + TtPath::BaseName( target );
      std::string to = output_dir + "\\" + TtPath::BaseName( target );
      std::filesystem::copy( from, to, std::filesystem::copy_options::overwrite_existing );
    };

    copy_file( bmson.info_.back_image_ );
    copy_file( bmson.info_.eyecatch_image_ );
    copy_file( bmson.info_.banner_image_ );
    copy_file( bmson.info_.preview_music_ );

    for ( BmsonData::BgaHeader& header : bmson.bga_.bga_header_ ) {
      copy_file( header.name_ );
    }
  }
}
