// base/bmson_parser.cpp

#include <numeric>

#include "tt_json.h"

#include "string_table.h"
#include "utility.h"

#include "base/bmson_parser.h"

#pragma warning(disable : 4672)
#pragma warning(disable : 4673)

using namespace BMX2WAV;


// -- Exceptions ---------------------------------------------------------
// -- UnexpectedTokenException -------------------------------------------
BL::Bmson::UnexpectedTokenException::UnexpectedTokenException( TtJson::UnexpectedTokenException& origin ) :
TtJson::UnexpectedTokenException( origin )
{
}

std::string
BL::Bmson::UnexpectedTokenException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::UnexpectedToken.Get(), this->GetLine(), this->GetToken() );
}


// -- NumberFormatException ----------------------------------------------
BL::Bmson::NumberFormatException::NumberFormatException( TtJson::NumberFormatException& origin ) :
TtJson::NumberFormatException( origin )
{
}

std::string
BL::Bmson::NumberFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::NumberFormatError.Get(), this->GetLine() );
}

// -- UnicodeFormatException ---------------------------------------------
BL::Bmson::UnicodeFormatException::UnicodeFormatException( TtJson::UnicodeFormatException& origin ) :
TtJson::UnicodeFormatException( origin )
{
}

std::string
BL::Bmson::UnicodeFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::UnicodeFormatError.Get(), this->GetLine() );
}

// -- BadCastException ---------------------------------------------------
BL::Bmson::BadCastException::BadCastException( TtJson::BadCastException& origin ) :
TtJson::BadCastException( origin )
{
}

std::string
BL::Bmson::BadCastException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::DataTypeError.Get(), this->GetLine(), this->GetExpectedType().name(), this->GetActualType().name() );
}

// -- OutOfBmsRangeException ---------------------------------------------

// -- BarIsOutOfBmsRangeException ----------------------------------------
std::string
BL::Bmson::BarIsOutOfBmsRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::BarIsOutOfBmsRange.Get() );
}

// -- NumberOfObjectsIsOutOfBmsRangeException ----------------------------
BL::Bmson::NumberOfObjectsIsOutOfBmsRangeException::NumberOfObjectsIsOutOfBmsRangeException( const std::string& object_kind ) :
object_kind_( object_kind )
{
}

const std::string&
BL::Bmson::NumberOfObjectsIsOutOfBmsRangeException::GetObjectKind( void )
{
  return object_kind_;
}

std::string
BL::Bmson::NumberOfObjectsIsOutOfBmsRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::NumberOfObjectsIsOutOfBmsRange.Get(), this->GetObjectKind() );
}

// -- RequiredKeyIsNothingException --------------------------------------
BL::Bmson::RequiredKeyIsNothingException::RequiredKeyIsNothingException( const std::string& key ) :
required_key_( key )
{
}

const std::string&
BL::Bmson::RequiredKeyIsNothingException::GetRequiredKey( void )
{
  return required_key_;
}

std::string
BL::Bmson::RequiredKeyIsNothingException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::RequiredKeyIsNothing.Get(), this->GetRequiredKey() );
}

// -- BmsonObjectIsOutOfBmsonLineRangeException --------------------------
std::string
BL::Bmson::BmsonObjectIsOutOfBmsonLineRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::BmsonObjectIsOutOfBmsonLineRange.Get() );
}


// -- ConvertBmsonException ----------------------------------------------
ConvertBmsonException::ConvertBmsonException( BL::Bmson::BmsonException& origin ) :
ConvertException( ErrorLevel::ImmediatelyAbort ),
message_( origin.GetMessage() )
{
}

std::string
ConvertBmsonException::GetMessage( void )
{
  return message_;
}



// -- JsonToBmsonDataConverter -------------------------------------------
BL::Bmson::JsonToBmsonDataConverter::JsonToBmsonDataConverter( void ) :
bmson_()
{
}

BL::Bmson::BmsonData
BL::Bmson::JsonToBmsonDataConverter::Convert( TtJson::Value& root )
{
  bmson_ = {};

  try {
    this->ProcessRoot( root.CastTo<TtJson::HashTable>() );
  }
  catch ( TtJson::BadCastException& e ) {
    throw BadCastException( e );
  }

  return bmson_;
}


template <class TYPE>
auto
BL::Bmson::JsonToBmsonDataConverter::HashTableGetValueAs( TtJson::HashTable& table, const std::string& key )
{
  if ( NOT( table.GetData().contains( key ) ) ) {
    throw RequiredKeyIsNothingException( key );
  }
  return table.GetData()[key]->CastTo<TYPE>();
}

template <class TYPE>
void
BL::Bmson::JsonToBmsonDataConverter::IfHashTableContainsKeyDoValueAs( TtJson::HashTable& table, const std::string& key, std::function<void ( TYPE& )> func )
{
  if ( table.GetData().contains( key ) ) {
    func( this->HashTableGetValueAs<TYPE>( table, key ) );
  }
}


void
BL::Bmson::JsonToBmsonDataConverter::ProcessRoot( TtJson::HashTable& root )
{
  bmson_.version_ = this->HashTableGetValueAs<TtJson::String>( root, "version" ).GetData();

  this->ProcessInfo( this->HashTableGetValueAs<TtJson::HashTable>( root, "info" ) );

  this->IfHashTableContainsKeyDoValueAs<TtJson::Array>( root, "lines", [&] ( auto& array ) {
    this->ProcessLines( array );
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::Array>( root, "bpm_events", [&] ( auto& array ) {
    this->ProcessBpmEvents( array );
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::Array>( root, "stop_events", [&] ( auto& array ) {
    this->ProcessStopEvents( array );
  } );

  this->ProcessSoundChannels( this->HashTableGetValueAs<TtJson::Array>( root, "sound_channels" ) );

  // bga ‚Í–{—ˆ‚Í•K{€–Ú‚¾‚¯‚Ç”CˆÓ€–Ú‚Æ‚µ‚Äˆµ‚¤
  // this->ProcessBga( this->HashTableGetValueAs<TtJson::HashTable>( root, "bga" ) );
  this->IfHashTableContainsKeyDoValueAs<TtJson::HashTable>( root, "bga", [&] ( auto& table ) {
    this->ProcessBga( table );
  } );
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessInfo( TtJson::HashTable& info )
{
  bmson_.info_.title_ = this->HashTableGetValueAs<TtJson::String>( info, "title" ).GetData();

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "subtitle", [&] ( auto& subtitle ) {
    bmson_.info_.sub_title_ = subtitle.GetData();
  } );

  bmson_.info_.artist_ = this->HashTableGetValueAs<TtJson::String>( info, "artist" ).GetData();

  this->IfHashTableContainsKeyDoValueAs<TtJson::Array>( info, "subartists", [&] ( auto& sub_artists ) {
    for ( auto value : sub_artists.GetData() ) {
      bmson_.info_.sub_artists_.push_back( value->CastTo<TtJson::String>().GetData() );
    }
  } );

  bmson_.info_.genre_ = this->HashTableGetValueAs<TtJson::String>( info, "genre" ).GetData();

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "mode_hint", [&] ( auto& mode_hint ) {
    bmson_.info_.mode_hint_ = mode_hint.GetData();
  } );

  bmson_.info_.chart_name_ = this->HashTableGetValueAs<TtJson::String>( info, "chart_name" ).GetData();
  bmson_.info_.level_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( info, "level" ).GetData() );
  bmson_.info_.init_bpm_ = this->HashTableGetValueAs<TtJson::Number>( info, "init_bpm" ).GetData();

  this->IfHashTableContainsKeyDoValueAs<TtJson::Number>( info, "judge_rank", [&] ( auto& judge_rank ) {
    bmson_.info_.judge_rank_ = judge_rank.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::Number>( info, "total", [&] ( auto& total ) {
    bmson_.info_.total_ = total.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "back_image", [&] ( auto& back_image ) {
    bmson_.info_.back_image_ = back_image.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "eyuecatch_image", [&] ( auto& eyecatch_image ) {
    bmson_.info_.eyecatch_image_ = eyecatch_image.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "banner_image", [&] ( auto& banner_image ) {
    bmson_.info_.banner_image_ = banner_image.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::String>( info, "preview_music", [&] ( auto& preview_music ) {
    bmson_.info_.preview_music_ = preview_music.GetData();
  } );

  this->IfHashTableContainsKeyDoValueAs<TtJson::Number>( info, "resolution", [&] ( auto& resolution  ) {
    bmson_.info_.resolution_ = static_cast<uint64_t>( resolution.GetData() );
  } );
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessLines( TtJson::Array& lines )
{
  for ( auto value : lines.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    uint64_t tmp = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "y" ).GetData() );
    bmson_.lines_.push_back( BmsonData::BarLine( {tmp} ) );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessBpmEvents( TtJson::Array& bpm_events )
{
  for ( auto value : bpm_events.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    uint64_t position = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "y" ).GetData() );
    double bpm = this->HashTableGetValueAs<TtJson::Number>( table, "bpm" ).GetData();
    bmson_.bpm_events_.push_back( BmsonData::BpmEvent( {position, bpm} ) );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessStopEvents( TtJson::Array& stop_events )
{
  for ( auto value : stop_events.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    uint64_t position = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "y" ).GetData() );
    uint64_t duration = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "duration" ).GetData() );
    bmson_.stop_events_.push_back( BmsonData::StopEvent( {position, duration} ) );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessSoundChannels( TtJson::Array& sound_channels )
{
  for ( auto value : sound_channels.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    BmsonData::SoundChannel tmp;
    tmp.name_ = this->HashTableGetValueAs<TtJson::String>( table, "name" ).GetData();
    this->ProcessNotes( this->HashTableGetValueAs<TtJson::Array>( table, "notes" ), tmp.notes_ );
    bmson_.sound_channels_.push_back( tmp );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessNotes( TtJson::Array& notes, std::vector<BmsonData::Note>& v )
{
  for ( auto value : notes.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    BmsonData::Note tmp;
    tmp.lane_ = static_cast<unsigned int>( this->HashTableGetValueAs<TtJson::Number>( table, "x" ).GetData() );
    tmp.position_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "y" ).GetData() );
    tmp.length_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "l" ).GetData() );
    tmp.continuation_flag_ = this->HashTableGetValueAs<TtJson::Boolean>( table, "c" ).GetData();
    v.push_back( tmp );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessBga( TtJson::HashTable& bga )
{
  this->ProcessBgaHeader( this->HashTableGetValueAs<TtJson::Array>( bga, "bga_header" ) );
  this->ProcessBgaEvents( this->HashTableGetValueAs<TtJson::Array>( bga, "bga_events" ), bmson_.bga_.bga_events_ );
  this->ProcessBgaEvents( this->HashTableGetValueAs<TtJson::Array>( bga, "layer_events" ), bmson_.bga_.layer_events_ );
  this->ProcessBgaEvents( this->HashTableGetValueAs<TtJson::Array>( bga, "poor_events" ), bmson_.bga_.poor_events_ );
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessBgaHeader( TtJson::Array& bga_header )
{
  for ( auto value : bga_header.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    BmsonData::BgaHeader tmp;
    tmp.id_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "id" ).GetData() );
    tmp.name_ = this->HashTableGetValueAs<TtJson::String>( table, "name" ).GetData();
    bmson_.bga_.bga_header_.push_back( tmp );
  }
}

void
BL::Bmson::JsonToBmsonDataConverter::ProcessBgaEvents( TtJson::Array& bga_events, std::vector<BmsonData::BgaEvent>& v )
{
  for ( auto value : bga_events.GetData() ) {
    auto& table = value->CastTo<TtJson::HashTable>();
    BmsonData::BgaEvent tmp;
    tmp.position_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "y" ).GetData() );
    tmp.id_ = static_cast<uint64_t>( this->HashTableGetValueAs<TtJson::Number>( table, "id" ).GetData() );
    v.push_back( tmp );
  }
}

// -- Parser -------------------------------------------------------------
BL::Bmson::Parser::Parser( void )
{
}

std::shared_ptr<BL::BmsData>
BL::Bmson::Parser::Parse( const std::string& path )
{
  std::string string_data = [&path] ( void ) -> std::string {
    Utility::TextFileReader reader( path );
    reader.read_as_utf8_ = true;
    return reader.ReadAllAutoEncode();
  }();

  std::shared_ptr<TtJson::Value> json_data;
  try {
    json_data = TtJson::Parser::Parse( string_data );
  }
  catch ( TtJson::UnexpectedTokenException& e ) {
    throw UnexpectedTokenException( e );
  }
  catch ( TtJson::NumberFormatException& e ) {
    throw NumberFormatException( e );
  }
  catch ( TtJson::UnicodeFormatException& e ) {
    throw UnicodeFormatException( e );
  }

  JsonToBmsonDataConverter converter;
  BmsonData bmson_data = converter.Convert( *json_data );

  return this->BmsonDataToBmsData( bmson_data );
}


// -- BmsonData => BmsData -----------------------------------------------
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


std::shared_ptr<BL::BmsData>
BL::Bmson::Parser::BmsonDataToBmsData( BmsonData& bmson )
{
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

      bms->headers_["BPM" + Word( i ).ToString()] = TtUtility::ToStringFrom( event.bpm_ );
      bms->extended_bpm_array_.GetArray()[i] = std::make_optional<std::string>( TtUtility::ToStringFrom( event.bpm_ ) );

      position_converter.ConvertPositionAndCall( event.position_, [&] ( Bar& bar, unsigned int position_of_bar ) {
        bar.GetChannelBy( "08"_hex36 )[position_of_bar] = Word( i );
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
      bms->headers_["STOP" + Word( i ).ToString()] = duration_of_bms_string;
      bms->stop_sequence_array_.GetArray()[i] = std::make_optional<std::string>( duration_of_bms_string );

      position_converter.ConvertPositionAndCall( event.position_, [&] ( Bar& bar, unsigned int position_of_bar ) {
        bar.GetChannelBy( "09"_hex36 )[position_of_bar] = Word( i );
      } );
    }
  }

  // -- SoundChannel
  for ( unsigned int i = 0; BmsonData::SoundChannel& channel : bmson.sound_channels_ ) {
    ++i;
    if ( i > Const::WORD_MAX_VALUE ) {
      throw NumberOfObjectsIsOutOfBmsRangeException( "SoundChannel" );
    }

    bms->headers_["WAV" + Word( i ).ToString()] = channel.name_;
    bms->wav_array_.GetArray()[i] = std::make_optional<std::string>( channel.name_ );

    for ( BmsonData::Note& note : channel.notes_ ) {
      if ( note.continuation_flag_ ) {
        continue;
      }
      position_converter.ConvertPositionAndCall( note.position_, [&] ( Bar& bar, unsigned int position_of_bar ) {
        auto overwrite_word = [&] ( void ) -> bool {
          for ( std::shared_ptr<BL::Channel> channel : bar.GetBgmChannels() ) {
            if ( (*channel)[position_of_bar] == "00"_hex36 ) {
              (*channel)[position_of_bar] = Word( i );
              return true;
            }
          }
          return false;
        };

        if ( NOT( overwrite_word() ) ) {
          BL::Channel& channel = bar.MakeNewBgmChannel();
          channel[position_of_bar] = Word( i );
        }
      } );
    }
  }

  return bms;
}
