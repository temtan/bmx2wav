// mainstay/entry.cpp

#include "tt_string.h"

#include "base/parser.h"

#include "core/bmson_converter.h"

#include "mainstay/entry.h"

using namespace BMX2WAV;


// -- Entry --------------------------------------------------------------
Mainstay::Entry::Entry( void ) :
Entry( "" )
{
}

Mainstay::Entry::Entry( const std::string& path ) :
path_( path ),
parameter_( std::nullopt ),
bms_data_( nullptr ),

squirrel_object_( std::nullopt )
{
}


void
Mainstay::Entry::ParseAsBmsData( void )
{
  // TODO èàóùå©íºÇµ
  BL::Parser::Parser parser;
  parser.not_nesting_if_statement_ = true;
  try {
    if ( TtString::EndWith( TtString::ToLower( path_ ), ".bmson" ) ) {
      Core::BmsonConverter bmson_converter;
      bms_data_ = bmson_converter.ConvertForWaveConvertFromFile( path_ );
    }
    else {
      bms_data_ = parser.Parse( path_ );
    }
  }
  catch ( TtException& ) {
    bms_data_ = std::make_shared<BL::BmsData>();
    bms_data_->path_ = path_;
    bms_data_->most_serious_error_level_ = ErrorLevel::Internal;
  }
  catch ( ... ) {
    bms_data_ = std::make_shared<BL::BmsData>();
    bms_data_->path_ = path_;
    bms_data_->most_serious_error_level_ = ErrorLevel::Internal;
  }
}

void
Mainstay::Entry::ParseAsBmsDataOnce( void )
{
  if ( NOT( bms_data_ ) ) {
    this->ParseAsBmsData();
  }
}
