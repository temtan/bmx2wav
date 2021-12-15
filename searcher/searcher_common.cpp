// searcher/searcher_common.cpp

#include "tt_ini_file.h"
#include "tt_time.h"

#include "searcher/searcher_common.h"

using namespace BMX2WAV;


// -- IniFileOperation ---------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( BMX2WAVSeacher );

  DEFINE_PARAMETER_NAME_STRING( Placement );
  DEFINE_PARAMETER_NAME_STRING( SplitterPosition );
  DEFINE_PARAMETER_NAME_STRING( ErrorLog );
  DEFINE_PARAMETER_NAME_STRING( Time );
  DEFINE_PARAMETER_NAME_STRING( Dump );
  DEFINE_PARAMETER_NAME_STRING( TestMode );
}

void
Searcher::IniFileOperation::SavePlacement( const WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAVSeacher];
  section.SetString( Tag::Placement, TtUtility::Serialize( placement ) );
}

bool
Searcher::IniFileOperation::LoadPlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAVSeacher];
  auto keys = section.GetKeys();
  if ( std::find( keys.begin(), keys.end(), Tag::Placement ) == keys.end() ) {
    return false;
  }
  return TtUtility::Deserialize( section.GetString( Tag::Placement ), placement );
}

void
Searcher::IniFileOperation::SaveSplitterPosition( unsigned int width )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAVSeacher];
  section.SetInteger( Tag::SplitterPosition, width );
}

unsigned int
Searcher::IniFileOperation::LoadSplitterPosition( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAVSeacher];
  return section.GetInteger( Tag::SplitterPosition, Const::DefaultSplitterPosition );
}


void
Searcher::IniFileOperation::SaveErrorLogDump( TtException& e )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::ErrorLog];
  section.SetString( Tag::Time, TtTime::GetNow().GetDateTimeString() );
  section.SetString( Tag::Dump, e.Dump() );
}


bool
Searcher::IniFileOperation::LoadTestMode( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAVSeacher];
  return section.GetBoolean( Tag::TestMode, false );
}

// -- Image --------------------------------------------------------------
#include "searcher_resource.h"

std::vector<TtIcon>
Searcher::Image::ICONS;

std::vector<TtBmpImage>
Searcher::Image::BMPS;

std::optional<TtSystemImageList>
Searcher::Image::LIST( std::nullopt );

int
Searcher::Image::FOLDER_ICON_INDEX;

void
Searcher::Image::Initialize( void )
{
  Image::LIST.emplace( TtSystemImageList::GetSmallSystemImageList() );
  {
    SHFILEINFO info;
    char path[MAX_PATH];
    ::SHGetSpecialFolderPath( NULL, path, CSIDL_WINDOWS, 0 );
    ::SHGetFileInfo( path, 0, &info, sizeof( SHFILEINFO ), SHGFI_SYSICONINDEX );
    Image::FOLDER_ICON_INDEX = info.iIcon;
  }

  auto add = [] ( DWORD id ) {
    TtIcon icon = TtIcon::CreateFromResourceID( id );
    Image::ICONS.push_back( icon );
    Image::BMPS.push_back( icon.GetARGBBitmapAsSmall() );
    Image::LIST->AddIcon( icon );
  };

  add( RSID_ICON_MAIN );
  add( RSID_ICON_MAIN_SMALL );
  add( RSID_ICON_EXPORT );
  add( RSID_ICON_RELOAD_SQUIRREL_SCRIPT );
  add( RSID_ICON_SHOW_SQUIRREL_OUTPUT_DIALOG );
  add( RSID_ICON_AUTO_DISPLAY_CELLS );
  add( RSID_ICON_DISPLAY_CELLS );
  add( RSID_ICON_EXECUTE_SEARCH );
}
