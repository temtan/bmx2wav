// searcher_settings_property_sheet.cpp

#include "tt_folder_browser_dialog.h"

#include "string_table.h"

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
home_label_;
home_edit_;
home_ref_button_;
auto_display_check_;
asynchronous_check_;


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

// -- ParameterPropertySheet ---------------------------------------------
Searcher::SettingsPropertySheet::SettingsPropertySheet( Settings& settings ) :
TtPropertySheet( false ),

settings_page_( settings )
{
  this->AddPage( settings_page_ );
}

bool
Searcher::SettingsPropertySheet::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( StrT::Searcher::SPS::Title.Get() );

  return true;
}
