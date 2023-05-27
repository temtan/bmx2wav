// searcher/searcher_settings.cpp

#include "tt_ini_file.h"

#include "searcher/searcher_settings.h"

using namespace BMX2WAV;


// -----------------------------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( Settings );

  DEFINE_PARAMETER_NAME_STRING( HomeFolder );
  DEFINE_PARAMETER_NAME_STRING( AutoDisplayCells );
  DEFINE_PARAMETER_NAME_STRING( AsynchronousDisplay );
  DEFINE_PARAMETER_NAME_STRING( Language );
}


// -- Settings -----------------------------------------------------------
std::string
Searcher::Settings::GetLanguageFromFile( const std::string& path )
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];
  return section.GetString( Tag::Language, "" );
}


Searcher::Settings::Settings( void ) :
home_folder_( "" ),
auto_display_cells_( true ),
asynchronous_display_( true ),

language_( "" )
{
}

void
Searcher::Settings::ReadFromFile( const std::string& path )
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];

  home_folder_          = section.GetString(  Tag::HomeFolder,          "" );
  auto_display_cells_   = section.GetBoolean( Tag::AutoDisplayCells,    true ),
  asynchronous_display_ = section.GetBoolean( Tag::AsynchronousDisplay, true );

  language_ = section.GetString( Tag::Language, "" );

  ini_file.Flush();
}

void
Searcher::Settings::WriteToFile( const std::string& path ) const
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];

  section.SetString(  Tag::HomeFolder,          home_folder_ );
  section.SetBoolean( Tag::AutoDisplayCells,    auto_display_cells_ );
  section.SetBoolean( Tag::AsynchronousDisplay, asynchronous_display_ );

  section.SetString( Tag::Language, language_ );

  ini_file.Flush();
}
