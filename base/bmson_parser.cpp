// base/bmson_parser.cpp

#include <numeric>

#include "tt_json.h"

#include "string_table.h"
#include "utility.h"

#include "base/word.h"
#include "base/channel.h"

#include "base/bmson_parser.h"

using namespace BMX2WAV;


// -- Exceptions ---------------------------------------------------------

// -- BmsonDescriptionException ------------------------------------------
BL::Bmson::BmsonDescriptionException::BmsonDescriptionException( unsigned int line ) :
line_( line )
{
}

unsigned int
BL::Bmson::BmsonDescriptionException::GetLine( void ) const
{
  return line_;
}

// -- UnexpectedTokenException -------------------------------------------
BL::Bmson::UnexpectedTokenException::UnexpectedTokenException( TtJson::UnexpectedTokenException& origin ) :
BmsonDescriptionException( origin.GetLine() ),
token_( origin.GetToken() )
{
}

char
BL::Bmson::UnexpectedTokenException::GetToken( void ) const
{
  return token_;
}

std::string
BL::Bmson::UnexpectedTokenException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::UnexpectedToken.Get(), this->GetLine(), this->GetToken() );
}

// -- NumberFormatException ----------------------------------------------
BL::Bmson::NumberFormatException::NumberFormatException( TtJson::NumberFormatException& origin ) :
BmsonDescriptionException( origin.GetLine() )
{
}

std::string
BL::Bmson::NumberFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::NumberFormatError.Get(), this->GetLine() );
}

// -- UnicodeFormatException ---------------------------------------------
BL::Bmson::UnicodeFormatException::UnicodeFormatException( TtJson::UnicodeFormatException& origin ) :
BmsonDescriptionException( origin.GetLine() )
{
}

std::string
BL::Bmson::UnicodeFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::UnicodeFormatError.Get(), this->GetLine() );
}

// -- BadCastException ---------------------------------------------------
BL::Bmson::BadCastException::BadCastException( TtJson::BadCastException& origin ) :
BmsonDescriptionException( origin.GetLine() ),
expected_( origin.GetExpectedType() ),
actual_( origin.GetActualType() )
{
}

const type_info&
BL::Bmson::BadCastException::GetExpectedType( void ) const
{
  return expected_;
}

const type_info&
BL::Bmson::BadCastException::GetActualType( void ) const
{
  return actual_;
}

std::string
BL::Bmson::BadCastException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::DataTypeError.Get(), this->GetLine(), this->GetExpectedType().name(), this->GetActualType().name() );
}


// -- RequiredKeyIsNothingException --------------------------------------
BL::Bmson::RequiredKeyIsNothingException::RequiredKeyIsNothingException( TtJson::HashTable& table, const std::string& key ) :
BmsonDescriptionException( table.GetLine() ),
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
  return Utility::Format( StrT::Message::BmsonParser::RequiredKeyIsNothing.Get(), this->GetLine(), this->GetRequiredKey() );
}


// -- ConvertBmsonDescriptionException -----------------------------------
ConvertBmsonDescriptionException::ConvertBmsonDescriptionException( BL::Bmson::BmsonDescriptionException& origin ) :
ConvertException( ErrorLevel::ImmediatelyAbort ),
line_( origin.GetLine() ),
message_( origin.GetMessage() )
{
}

unsigned int
ConvertBmsonDescriptionException::GetLine( void ) const
{
  return line_;
}


std::string
ConvertBmsonDescriptionException::GetMessage( void )
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
    throw RequiredKeyIsNothingException( table, key );
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


std::shared_ptr<BL::Bmson::BmsonData>
BL::Bmson::Parser::Parse( const std::string& path )
{
  std::shared_ptr<TtJson::Value> tmp = this->FileToJsonData( path );
  return this->JsonDataToBmsonData( *tmp );
}


std::shared_ptr<TtJson::Value>
BL::Bmson::Parser::FileToJsonData( const std::string& path )
{
  std::string string_data = [&path] ( void ) -> std::string {
    try {
      Utility::TextFileReader reader( path );
      reader.read_as_utf8_ = true;
      return reader.ReadAllAutoEncode();
    }
    catch ( TtFileAccessException& ex ) {
      throw BL::Parser::FileAccessException( ex );
    }
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
  return json_data;
}

std::shared_ptr<BL::Bmson::BmsonData>
BL::Bmson::Parser::JsonDataToBmsonData( TtJson::Value& json_data )
{
  JsonToBmsonDataConverter converter;
  return std::make_shared<BL::Bmson::BmsonData>( converter.Convert( json_data ) );
}
