// searcher/searcher_settings_property_sheet.cpp

#include "tt_folder_browser_dialog.h"
#include "tt_directory.h"
#include "tt_path.h"

#include "string_table.h"
#include "utility.h"

#include "searcher/searcher_common.h"

#include "searcher/searcher_settings_property_sheet.h"

using namespace BMX2WAV;


// -- SettingsPage -------------------------------------------------------
Searcher::SettingsPropertySheet::SettingsPage::SettingsPage( Settings& settings ) :
Page( StrT::Searcher::SPS::Settings.Get() ),
settings_( settings )
{
  this->PresetSizeAtPageCreate( 268, 220 );
}

bool
Searcher::SettingsPropertySheet::SettingsPage::Created( void )
{
  home_label_.Create( {this} );
  home_edit_.Create( {this} );
  home_ref_button_.Create( {this, CommandID::HomeReferenceButton} );
  auto_display_check_.Create( {this} );
  asynchronous_check_.Create( {this} );

  home_label_.SetPositionSize(             4,   8, 316, 16 );
  home_edit_.SetPositionSize(              4,  28, 350, 20 );
  home_ref_button_.SetPositionSize(      366,  28,  50, 20 );
  auto_display_check_.SetPositionSize(     4,  60, 316, 16);
  asynchronous_check_.SetPositionSize(     4,  84, 316, 16 );

  home_label_.SetText(         StrT::Searcher::SPS::SettingsHomeLabel.Get() );
  home_ref_button_.SetText(    StrT::Searcher::SPS::SettingsHomeRefButton.Get() );
  auto_display_check_.SetText( StrT::Searcher::SPS::SettingsAutoDisplayCheck.Get() );
  asynchronous_check_.SetText( StrT::Searcher::SPS::SettingsAsynchronousCheck.Get() );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.home_folder_          = home_edit_.GetText();
    settings_.auto_display_cells_   = auto_display_check_.GetCheck();
    settings_.asynchronous_display_ = asynchronous_check_.GetCheck();
    return true;
  };

  this->AddCommandHandler( CommandID::HomeReferenceButton, [&] ( int, HWND ) -> WMResult {
    TtFolderBrowserDialog dialog;
    dialog.SetDescription( StrT::Searcher::SPS::SettingsHomeRefFolderDialogDescription.Get() );
    if ( NOT( settings_.home_folder_.empty() ) ) {
      dialog.SetSelectedPath( settings_.home_folder_ );
    }
    if ( dialog.ShowDialog( *this ) ) {
      home_edit_.SetText( dialog.GetSelectedPath() );
    }
    return {WMResult::Done};
  } );

  home_edit_.SetText( settings_.home_folder_ );
  auto_display_check_.SetCheck( settings_.auto_display_cells_ );
  asynchronous_check_.SetCheck( settings_.asynchronous_display_ );

  home_label_.Show();
  home_edit_.Show();
  home_ref_button_.Show();
  auto_display_check_.Show();
  asynchronous_check_.Show();

  return true;
}

// -- LanguagePage -------------------------------------------------------
Searcher::SettingsPropertySheet::LanguagePage::LanguagePage( Settings& settings ) :
Page( StrT::Searcher::SPS::Language.Get() ),
settings_( settings ),
language_string_( "" )
{
}

bool
Searcher::SettingsPropertySheet::LanguagePage::Created( void )
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

  language_group_.SetText( StrT::Searcher::SPS::LanguageLanguageGroup.Get() );
  notice_label_.SetText(   StrT::Searcher::SPS::LanguageNoticeLabel.Get() );
  select_button_.SetText(  StrT::Searcher::SPS::LanguageSelectButton.Get() );

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

      language_label_.SetText( language_string_.empty() ? StrT::Searcher::SPS::LanguageAuto.Get() : language_string_ );
    }
    return {WMResult::Done};
  } );

  language_string_ = settings_.language_;
  language_label_.SetText( settings_.language_.empty() ? StrT::Searcher::SPS::LanguageAuto.Get() : settings_.language_ );
  {
    dll_list_box_.Push( StrT::Searcher::SPS::LanguageAuto.Get() );

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
Searcher::SettingsPropertySheet::SettingsPropertySheet( Settings& settings ) :
TtPropertySheet( false ),

settings_page_( settings ),
language_page_( settings )
{
  this->AddPage( settings_page_ );
  this->AddPage( language_page_ );
}

bool
Searcher::SettingsPropertySheet::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( StrT::Searcher::SPS::Title.Get() );

  return true;
}
