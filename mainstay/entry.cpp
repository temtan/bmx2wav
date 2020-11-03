// mainstay/entry.cpp

#include "base/parser.h"

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
  // TODO ˆ—Œ©’¼‚µ
  BL::Parser::Parser parser;
  parser.not_nesting_if_statement_ = true;
  try {
    bms_data_ = parser.Parse( path_ );
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
