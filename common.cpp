// common.cpp

#include <stdio.h>

#include "tt_enum.h"
#include "tt_ini_file.h"
#include "tt_time.h"

#include "common.h"

using namespace BMX2WAV;


// -- Enums --------------------------------------------------------------
template <>
TtEnumTable<ErrorLevel>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( ErrorLevel::NAME, #NAME )
  REGISTER( Internal );
  REGISTER( ImmediatelyAbort );
  REGISTER( Fatal );
  REGISTER( NeedFix );
  REGISTER( Tiny );
  REGISTER( Warning );
  REGISTER( None );
#undef REGISTER
}

TtEnumTable<ErrorLevel>
TtEnumBase<ErrorLevel>::TABLE;


// -- IniFileOperation ---------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( BMX2WAV );

  DEFINE_PARAMETER_NAME_STRING( Placement );
  DEFINE_PARAMETER_NAME_STRING( TestMode );
  DEFINE_PARAMETER_NAME_STRING( AutoDisplayCells );
  DEFINE_PARAMETER_NAME_STRING( ErrorLog );
  DEFINE_PARAMETER_NAME_STRING( Dump );
  DEFINE_PARAMETER_NAME_STRING( Time );
  DEFINE_PARAMETER_NAME_STRING( Language );
}

void
IniFileOperation::SavePlacement( const WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  section.SetString( Tag::Placement, TtUtility::Serialize( placement ) );
}

bool
IniFileOperation::LoadPlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  auto keys = section.GetKeys();
  if ( std::find( keys.begin(), keys.end(), Tag::Placement ) == keys.end() ) {
    return false;
  }
  return TtUtility::Deserialize( section.GetString( Tag::Placement ), placement );
}

void
IniFileOperation::SaveAutoDisplayCells( bool flag )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  section.SetBoolean( Tag::AutoDisplayCells, flag );
}

bool
IniFileOperation::LoadAutoDisplayCells( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  return section.GetBoolean( Tag::AutoDisplayCells, true );
}


std::string
IniFileOperation::LoadLanguage( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  return section.GetString( Tag::Language, "japanese" );
}


void
IniFileOperation::SaveErrorLogDump( TtException& e )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::ErrorLog];
  section.SetString( Tag::Time, TtTime::GetNow().GetDateTimeString() );
  section.SetString( Tag::Dump, e.Dump() );
}


bool
IniFileOperation::LoadTestMode( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::BMX2WAV];
  return section.GetBoolean( Tag::TestMode, false );
}


// -- Image --------------------------------------------------------------
#include "resource.h"

std::vector<TtIcon>
Image::ICONS;

std::vector<TtBmpImage>
Image::BMPS;

std::optional<TtSystemImageList>
Image::LIST( std::nullopt );

void
Image::Initialize( void )
{
  Image::LIST.emplace( TtSystemImageList::GetSmallSystemImageList() );

  auto add = [] ( DWORD id ) {
    TtIcon icon = TtIcon::CreateFromResourceID( id );
    Image::ICONS.push_back( icon );
    Image::BMPS.push_back( icon.GetARGBBitmapAsSmall() );
    Image::LIST->AddIcon( icon );
  };

  add( RSID_ICON_MAIN );
  add( RSID_ICON_MAIN_SMALL );
  add( RSID_ICON_BMS_FILE );
  add( RSID_ICON_AUTO_DISPLAY_CELLS );
  add( RSID_ICON_DISPLAY_CELLS );
  add( RSID_ICON_RELOAD );
  add( RSID_ICON_EDIT_COMMON_PARAMETER );
  add( RSID_ICON_ADD_ENTRY );
  add( RSID_ICON_DELETE_ENTRY );
  add( RSID_ICON_DELETE_ALL_ENTRY );
  add( RSID_ICON_SHOW_PROPERTY_OF_ENTRY );
  add( RSID_ICON_EDIT_INDIVIDUAL_PARAMETER );
  add( RSID_ICON_CONVERT_ONE_BMS );
  add( RSID_ICON_CONVERT_ALL_BMS );
  add( RSID_ICON_EXECUTE_SQUIRREL_SCRIPT );
  add( RSID_ICON_RELOAT_SQUIRREL_SCRIPT );
  add( RSID_ICON_SHOW_SQUIRREL_OUTPUT_DIALOG );
  add( RSID_ICON_SEARCHER_MAIN_SMALL );
}
