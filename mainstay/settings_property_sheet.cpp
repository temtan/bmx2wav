// mainstay/settings_property_sheet.cpp

#include "tt_path.h"
#include "tt_directory.h"

#include "common.h"
#include "string_table.h"
#include "utility.h"

#include "mainstay/settings_property_sheet.h"

using namespace BMX2WAV;


// -- LanguagePage -------------------------------------------------------
Mainstay::SettingsPropertySheet::LanguagePage::LanguagePage( Settings& settings ) :
Page( StrT::SPS::Language.Get() ),
settings_( settings ),
language_string_( "" )
{
  this->PresetSizeAtPageCreate( 268, 220 );
}

bool
Mainstay::SettingsPropertySheet::LanguagePage::Created( void )
{
  language_label_.Create( {this} );
  notice_label_.Create( {this} );
  language_group_.Create( {this} );
  select_button_.Create( {this, CommandID::LanguageSelectButton} );
  dll_list_box_.Create( {this} );

  language_label_.SetPositionSize(  28,  30, 160,  24 );
  notice_label_.SetPositionSize(   230,  30, 170,  24 );
  language_group_.SetPositionSize(   8,   8, 210,  48 );
  select_button_.SetPositionSize(   36,  64, 150,  24 );
  dll_list_box_.SetPositionSize(    16, 100, 190, 160 );

  language_group_.SetText( StrT::SPS::LanguageLanguageGroup.Get() );
  notice_label_.SetText(   StrT::SPS::LanguageNoticeLabel.Get() );
  select_button_.SetText(  StrT::SPS::LanguageSelectButton.Get() );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.language_ = language_string_;
    return true;
  };

  this->AddCommandHandler( CommandID::LanguageSelectButton, [&] ( int, HWND ) -> WMResult {
    if ( dll_list_box_.GetCurrent() != LB_ERR ) {
      if ( dll_list_box_.GetCurrent() == 0 ) {
        language_string_ = "";
      }
      else {
        language_string_ = dll_list_box_.GetCurrentText();
      }

      language_label_.SetText( language_string_.empty() ? StrT::SPS::LanguageAuto.Get() : language_string_ );
    }
    return {WMResult::Done};
  } );

  language_string_ = settings_.language_;
  language_label_.SetText( settings_.language_.empty() ? StrT::SPS::LanguageAuto.Get() : settings_.language_ );
  {
    dll_list_box_.Push( StrT::SPS::LanguageAuto.Get() );

    TtDirectory dir( Utility::GetLanguageDirectoryPath() );
    auto files = dir.GetEntries( "*.dll", TtDirectory::FileOnly );
    for ( auto& path : files ) {
      dll_list_box_.Push( TtPath::BaseName( path ) );
    }
  }

  language_label_.Show();
  notice_label_.Show();
  language_group_.Show();
  select_button_.Show();
  dll_list_box_.Show();

  return true;
}

// -- SettingsPropertySheet ----------------------------------------------
Mainstay::SettingsPropertySheet::SettingsPropertySheet( Settings& settings ) :
TtPropertySheet( false ),

language_page_( settings )
{
  this->AddPage( language_page_ );
}

bool
Mainstay::SettingsPropertySheet::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( StrT::SPS::Title.Get() );

  return true;
}
