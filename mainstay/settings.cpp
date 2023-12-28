// mainstay/settings.cpp

#include "tt_ini_file.h"

#include "mainstay/settings.h"

using namespace BMX2WAV;


// -----------------------------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( Settings );

  DEFINE_PARAMETER_NAME_STRING( Language );
}


// -- Settings -----------------------------------------------------------
std::string
Mainstay::Settings::GetLanguageFromFile( const std::string& path )
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];
  return section.GetString( Tag::Language, "" );
}


Mainstay::Settings::Settings( void ) :
language_( "" )
{
}

void
Mainstay::Settings::ReadFromFile( const std::string& path )
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];

  language_ = section.GetString( Tag::Language, "" );

  ini_file.Flush();
}

void
Mainstay::Settings::WriteToFile( const std::string& path ) const
{
  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::Settings];

  section.SetString( Tag::Language, language_ );

  ini_file.Flush();
}
