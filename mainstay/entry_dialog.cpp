// mainstay/entry_dialog.cpp

#include "tt_message_box.h"

#include "parameter_property_sheet.h"

#include "mainstay/entry_dialog.h"

using namespace BMX2WAV;


// -- EntryDialog --------------------------------------------------------
Mainstay::EntryDialog::EntryDialog( Entry* entry, MainFrame& parent ) :
entry_( entry ),
parent_( parent )
{
}


DWORD
Mainstay::EntryDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_SYSMENU | WS_SIZEBOX;
}

DWORD
Mainstay::EntryDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
Mainstay::EntryDialog::Created( void )
{
  this->SetText( "エントリー詳細ダイアログ" );

  struct CommandID {
    enum ID : int {
      UseCommonParameterButton = 14001,
      UseIndividualParameterButton,
      ConvertButton,
    };
  };

  path_label_.Create( {this} );
  using_parameter_title_label_.Create( {this} );
  using_parameter_label_.Create( {this} );
  use_common_parameter_button_.Create( {this, CommandID::UseCommonParameterButton} );
  use_individual_parameter_button_.Create( {this, CommandID::UseIndividualParameterButton} );
  convert_button_.Create( {this, CommandID::ConvertButton} );
  list_.Create( {this} );
  list_.SetFullRowSelect( true );
  list_.SetHasGridLines( true );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    path_label_.SetPositionSize(                               8,   8,     w - 16,      18 );
    using_parameter_title_label_.SetPositionSize(              8,  30,         80,      18 );
    using_parameter_label_.SetPositionSize(                   92,  27,        100,      18 );
    use_common_parameter_button_.SetPositionSize(             24,  52, w / 2 - 36,      24 );
    use_individual_parameter_button_.SetPositionSize( w / 2 + 12,  52, w / 2 - 36,      24 );
    convert_button_.SetPositionSize(                          24,  84,     w - 48,      24 );
    list_.SetPositionSize(                                     4, 116,     w -  8, h - 120 );
    path_label_.SetText( path_label_.GetText() );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 330, 200 ) );
  this->SetClientSize( 330, 400, false );
  this->SetCenterRelativeToParent();

  using_parameter_title_label_.SetText( "使用する設定 :" );
  use_common_parameter_button_.SetText( "共通設定を使用する" );
  use_individual_parameter_button_.SetText( "個別設定を使用する" );
  convert_button_.SetText( "変換する" );
  {
    auto column_name = list_.MakeNewColumn();
    column_name.SetText( "名称" );
    column_name.SetWidth( 120 );

    auto column_value = list_.MakeNewColumn();
    column_value.SetText( "値" );
    column_value.SetWidth( 180 );
  }

  path_label_.SetText( entry_->path_ );
  this->SetUsingParameterLabelFromParameter();
  parent_.SquirrelErrorHandling( [&] ( void ) {
    for ( auto& one : parent_.vm_->GetEntryDialogListItems( entry_ ) ) {
      auto item = list_.MakeNewItem();
      item.SetText( one.first );
      item.SetSubItemText( 1, one.second );
    }
  } );

  this->AddCommandHandler( CommandID::UseCommonParameterButton, [&] ( int, HWND ) -> WMResult {
    if ( entry_->parameter_ ) {
      TtMessageBoxYesNo box;
      box.SetMessage( "個別設定を廃棄して共通設定を使用します。よろしいですか？" );
      box.SetCaption( "個別設定廃棄の確認" );
      box.SetIcon( TtMessageBox::Icon::QUESTION );
      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        entry_->parameter_.reset();
        this->SetUsingParameterLabelFromParameter();
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::UseIndividualParameterButton, [&] ( int, HWND ) -> WMResult {
    if ( NOT( entry_->parameter_ ) ) {
      TtMessageBoxYesNoCancel box;
      box.SetMessage( "共通設定をコピーして個別設定を使用しますか？（使用しない場合はデフォルト設定になります）" );
      box.SetCaption( "確認" );
      box.SetIcon( TtMessageBox::Icon::QUESTION );
      auto ret = box.ShowDialog( *this );
      if ( ret == TtMessageBox::Result::CANCEL ) {
        return {WMResult::Done};
      }
      if ( ret == TtMessageBox::Result::YES ) {
        entry_->parameter_.emplace( parent_.common_parameter_ );
      }
      else {
        entry_->parameter_.emplace();
      }
    }
    ParameterPropertySheet sheet( *entry_->parameter_, false );
    sheet.ShowDialog( *this );

    this->SetUsingParameterLabelFromParameter();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ConvertButton, [&] ( int, HWND ) -> WMResult {
    Core::ConvertParameter parameter = parent_.common_parameter_;
    if ( entry_->parameter_ ) {
      parameter = entry_->parameter_.value();
    }
    parameter.input_file_path_ = entry_->path_;
    ConverterDialog dialog( parameter );
    dialog.ShowDialog( *this );
    return {WMResult::Done};
  } );

  path_label_.Show();
  using_parameter_title_label_.Show();
  using_parameter_label_.Show();
  use_common_parameter_button_.Show();
  use_individual_parameter_button_.Show();
  convert_button_.Show();
  list_.Show();
  return true;
}


void
Mainstay::EntryDialog::SetUsingParameterLabelFromParameter( void )
{
  using_parameter_label_.SetText( entry_->parameter_ ? "個別設定" : "共通設定" );
}
