// mainstay/multiple_convert_dialog.cpp

#include "converter_dialog.h"

#include "string_table.h"
#include "mainstay/main_frame.h"

#include "mainstay/multiple_convert_dialog.h"

using namespace BMX2WAV;

// -- MultipleConvertDialog ----------------------------------------------
Mainstay::MultipleConvertDialog::MultipleConvertDialog( MainFrame& parent, const std::vector<Entry*>& entries ) :
parent_( parent ),
entries_( entries ),
log_table_( entries.size() ),

converting_dialog_( nullptr ),

need_to_abort_( false ),
is_converting_( false ),
lock_(),

show_log_menu_( TtSubMenu::Create() )
{
}


DWORD
Mainstay::MultipleConvertDialog::GetStyle( void )
{
  return WS_SYSMENU | WS_SIZEBOX;
}

DWORD
Mainstay::MultipleConvertDialog::GetExtendedStyle( void )
{
  return WS_EX_APPWINDOW;
}


bool
Mainstay::MultipleConvertDialog::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::ConvertAllBms] );
  this->SetText( StrT::Multiple::Title.Get() );

  struct CommandID {
    enum ID : int {
      AbortButton = 17001,
      List,
      MenuShowLog,
    };
  };

  status_label_.Create( {this} );
  abort_button_.Create( {this, CommandID::AbortButton} );
  list_.Create( {this, CommandID::List} );
  list_.SetFullRowSelect( true );
  list_.SetHasGridLines( true );

  show_log_menu_.AppendNewItem( CommandID::MenuShowLog, StrT::Multiple::MenuShowLog.Get() );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    status_label_.SetPositionSize( 8,  4, w - 8,     18 );
    abort_button_.SetPositionSize( 4, 28, w - 8,     24 );
    list_.SetPositionSize(         4, 60, w - 8, h - 64 );
    status_label_.SetText( status_label_.GetText() );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 200, 200 ) );
  this->SetClientSize( 420, 400, false );
  this->SetPosition( parent_.GetPoint().x, parent_.GetPoint().y );

  abort_button_.SetText( StrT::Multiple::AbortButton.Get() );
  {
    auto make_column = [&] ( const std::string& title, unsigned int width ) {
      auto tmp = list_.MakeNewColumn();
      tmp.SetText( title );
      tmp.SetWidth( width );
    };
    make_column( StrT::Multiple::ColumnBms.Get(),              240 );
    make_column( StrT::Multiple::ColumnResult.Get(),            80 );
    make_column( StrT::Multiple::ColumnMostSeriousError.Get(),  80 );
  }

  for ( auto& entry : entries_ ) {
    auto item = list_.MakeNewItem();
    item.SetText( entry->path_ );
    item.SetSubItemText( 1, StrT::Multiple::ColumnDefaultTextUnconverted.Get() );
  }

  this->AddCommandHandler( CommandID::AbortButton, [&] ( int, HWND ) -> WMResult {
    lock_.EnterExecute( [&] ( void ) {
      if ( is_converting_.load() && converting_dialog_ ) {
        converting_dialog_->RequestAbort();
      }
    } );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::MenuShowLog, [&] ( int, HWND ) -> WMResult {
    this->ShowLogDialog();
    return {WMResult::Done};
  } );

  // -- リスト要素ドラッグ&ドロップ
  this->AddNotifyHandler( CommandID::List, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // アイテム右クリック
    case NM_RCLICK: {
      POINT cursor_point = TtWindow::GetCursorPosition();
      TtListViewItem hit_item = list_.HitTest( cursor_point.x - list_.GetPoint().x, cursor_point.y - list_.GetPoint().y );
      if ( hit_item.IsValid() ) {
        hit_item.SetSelected( true );
      }
      TtListViewItem item = list_.GetSelected();
      if ( item.IsValid() ) {
        show_log_menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
      return {WMResult::Done};
    }

      // アイテムダブルクリック
    case LVN_ITEMACTIVATE: {
      this->ShowLogDialog();
      return {WMResult::Done};
    }

      // カラムクリック
    case LVN_COLUMNCLICK: {
      return {WMResult::Done};
    }

      // ドラッグ & ドロップ周り
    case LVN_BEGINDRAG: {
      return {WMResult::Done};
    }
    }
    return {WMResult::Incomplete};
  } );

  // 閉じるとき。特殊な処理が必要
  this->RegisterWMClose( [this] ( void ) -> WMResult {
    if ( is_converting_.load() ) {
      return {WMResult::Done};
    }
    parent_.Show();
    this->EndDialog( 0 );
    return {WMResult::Done};
  }, false );

  status_label_.Show();
  abort_button_.Show();
  list_.Show();

  // Show しないと表示されないので注意
  log_dialog_.ShowDialog( *this );
  log_dialog_.SetText( StrT::Multiple::LogDioalogTitle.Get() );

  return {true};
}


void
Mainstay::MultipleConvertDialog::ShowLogDialog( void )
{
  auto item = list_.GetSelected();
  if ( item.IsValid() ) {
    log_dialog_.ClearText();
    log_dialog_.AddText( log_table_[item.GetIndex()] );
    log_dialog_.Show();
  }
}


void
Mainstay::MultipleConvertDialog::ConvertStart( void )
{
  need_to_abort_.store( false );
  lock_.Enter();
  is_converting_.store( true );
  TtUtility::DestructorCall converting_flag_finalizer( [&] ( void ) {
    is_converting_.store( false );
    lock_.Leave();
  } );
  status_label_.SetText( StrT::Multiple::StatusTextConverting.Get() );

  unsigned int i = 0;
  for ( auto& entry : entries_ ) {
    Core::ConvertParameter parameter = parent_.common_parameter_;
    if ( entry->parameter_ ) {
      parameter = entry->parameter_.value();
    }
    parameter.input_file_path_ = entry->path_;

    ConverterDialogCustom dialog( parameter, lock_ );
    dialog.SetStartPosition( {this->GetRectangle().right + 4, this->GetPoint().y} );

    converting_dialog_ = &dialog;
    dialog.ShowDialog( parent_ );
    converting_dialog_ = nullptr;

    log_table_[i] = dialog.GetOutputtedString();

    TtEnum<ConverterDialog::Result> result = dialog.GetResult();
    if ( result == ConverterDialog::Result::UserAborted ) {
      need_to_abort_.store( true );
    }
    list_.GetItem( i ).SetSubItemText( 1, result.ToString() );

    TtEnum<ErrorLevel> most_serious_error_level = dialog.GetMostSeriousErrorLevel();
    list_.GetItem( i ).SetSubItemText( 2, most_serious_error_level.ToString() );

    log_table_[i] = dialog.GetOutputtedString();

    if ( need_to_abort_.load() ) {
      break;
    }
    i += 1;
  }

  abort_button_.SetEnabled( false );
  if ( need_to_abort_.load() ) {
    status_label_.SetText( StrT::Multiple::StatusTextAborted.Get() );
  }
  else {
    status_label_.SetText( StrT::Multiple::StatusTextCompleted.Get() );
  }
}

// -- ConverterDialogCustom ------------------------------------------
Mainstay::MultipleConvertDialog::ConverterDialogCustom::ConverterDialogCustom( const Core::ConvertParameter& parameter, TtCriticalSection& lock ) :
ConverterDialog( parameter, true ),
lock_( lock )
{
}

bool
Mainstay::MultipleConvertDialog::ConverterDialogCustom::Created( void )
{
  auto ret = this->ConverterDialog::Created();

  this->RegisterSingleHandler( WM_CLOSE, [&, base_handler = this->GetSingleHandler( WM_CLOSE )] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    if ( NOT( this->IsConverting() ) ) {
      lock_.Enter();
    }
    return base_handler( w_param, l_param );
  }, false );

  lock_.Leave();
  return ret;
}
