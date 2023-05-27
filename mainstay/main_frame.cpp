// mainstay/main_frame.cpp

#include "tt_path.h"
#include "tt_message_box.h"
#include "tt_file_dialog.h"
#include "tt_string.h"

#include "common.h"
#include "utility.h"
#include "utility_dialogs.h"
#include "parameter_property_sheet.h"
#include "converter_dialog.h"
#include "string_table.h"

#include "mainstay/entry_dialog.h"
#include "mainstay/multiple_convert_dialog.h"
#include "mainstay/settings_property_sheet.h"

#include "mainstay/main_frame.h"

using namespace BMX2WAV;


// -- MainFrame ----------------------------------------------------------
std::pair<unsigned int, unsigned int>
Mainstay::MainFrame::NoSorted = {std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()};


Mainstay::MainFrame::MainFrame( void )
{
}

DWORD
Mainstay::MainFrame::GetStyle( void )
{
  return WS_OVERLAPPEDWINDOW;
}

DWORD
Mainstay::MainFrame::GetExtendedStyle( void )
{
  return 0;
}


bool
Mainstay::MainFrame::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( std::string( APPLICATION_NAME ) + " " + VERSION );
  this->SetMenu( main_menu_ );

  // -- コントロール作成 -----
  tool_bar_.Create( {this, CommandID::Control::MainToolBar} );
  list_.Create( {this, CommandID::Control::MainList} );

#ifdef _WIN64
  // x64 だと自動でツールチップが表示されない為
  this->AddNotifyHandler( CommandID::Control::MainToolBar, tool_bar_.MakeNotifyHandlerForToolTipByStringTable() );
#endif

  status_bar_.Create( {this} );
  status_bar_.DivideInto( 1, {-1} );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int f, int w, int h ) -> WMResult {
    const int tool_bar_height = 28;
    tool_bar_.SetPositionSize(   0,                   0,     w,          tool_bar_height );
    list_.SetPositionSize(       1, tool_bar_height + 5, w - 2, h - tool_bar_height - 24 );

    status_bar_.SendMessage( WM_SIZE, f, MAKELPARAM( w, h ) );
    return {WMResult::Done};
  } );

  // ハンドラ登録
  this->RegisterHandlers();

  // -- コントロール表示 -----
  tool_bar_.Show();
  list_.Show();

  // Show しないと表示されないので注意
  squirrel_standard_output_dialog_.ShowDialog( *this );
  squirrel_standard_output_dialog_.SetText( StrT::Main::SquirrelStandardOutputDialogTitle.Get() );

  this->SetAccelerator();

  // 色々と初期化
  {
    bool flag = IniFileOperation::LoadAutoDisplayCells();
    main_menu_.view_menu_.GetItemAt( 0 ).SetCheck( flag );
    tool_bar_.GetButton( CommandID::MainToolBar::ID::AutoDisplayCells ).SetCheck( flag );

    common_parameter_.ReadFromFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );

    last_sort_parameter_ = MainFrame::NoSorted;
    this->SetSortMethodToStatusBar( StrT::Main::DefaultSortMethodName.Get() );
  }

  this->InitializeSquirrelVM();

  return true;
}

void
Mainstay::MainFrame::RegisterHandlers( void )
{
  // -- メインメニュー -----
  // -- ファイル
  this->AddCommandHandler( CommandID::Close, [this] ( int, HWND ) -> WMResult {
    this->Close();
    return {WMResult::Done};
  } );

  // -- 表示
  this->AddCommandHandler( CommandID::AutoDisplayCells, [this] ( int, HWND ) -> WMResult {
    bool flag = NOT( main_menu_.view_menu_.GetItemAt( 0 ).GetCheck() );
    main_menu_.view_menu_.GetItemAt( 0 ).SetCheck( flag );
    tool_bar_.GetButton( CommandID::MainToolBar::ID::AutoDisplayCells ).SetCheck( flag );
    this->IfToggleButtonIsPressedDisplayCell();

    return {WMResult::Done};
  } );

  // -- カラムグループ選択時
  {
    auto select_column_group = [this] ( int index ) {
      main_menu_.column_group_menu_.CheckRadioItem( index );
      tool_bar_.select_column_group_box_.SetSelect( index );
      this->DisplayColumn();
      this->DisplayEntriesFromEntryPool();
      this->IfToggleButtonIsPressedDisplayCell();
    };

    this->AddMenuCommandHandler( main_menu_.column_group_menu_, [select_column_group] ( TtMenuItem item ) -> WMResult {
      select_column_group( item.GetIndex() );
      return {WMResult::Done};
    } );
    this->AddCommandHandler( CommandID::SelectColumnGroup, [this, select_column_group] ( int code, HWND ) -> WMResult {
      if ( code == CBN_SELCHANGE ) {
        select_column_group( tool_bar_.select_column_group_box_.GetSelectedIndex() );
      }
      return {WMResult::Done};
    } );
  }

  this->AddCommandHandler( CommandID::AutoDisplayCells, [this] ( int, HWND ) -> WMResult {
    this->DisplayCell();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Reload, [this] ( int, HWND ) -> WMResult {
    this->ParseEachBmsOfEntry();
    this->DisplayCell();
    return {WMResult::Done};
  } );

  // -- 編集
  this->AddCommandHandler( CommandID::EditCommonParameter, [this] ( int, HWND ) -> WMResult {
    ParameterPropertySheet sheet( common_parameter_, std::nullopt );
    sheet.ShowDialog( *this );
    this->IfToggleButtonIsPressedDisplayCell();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AddEntry, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {StrT::Main::AddEntryFileDialogBmsBmeFile.Get(), "*.bms"} );
    dialog.GetFilters().back().GetExtensions().push_back( "*.bme" );
    dialog.GetFilters().push_back( {StrT::Main::AddEntryFileDialogAllFile.Get(), "*.*"} );
    dialog.SetMultiselect( true );
    if ( dialog.ShowDialog( *this ) ) {
      for ( auto& path : dialog.GetFileNames() ) {
        this->AddEntry( path );
      }
      this->IfToggleButtonIsPressedDisplayCell();
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DeleteEntry, [this] ( int, HWND ) -> WMResult {
    auto item = list_.GetSelected();
    if ( item.IsValid() ) {
      unsigned int item_index = item.GetIndex();
      this->DeleteEntry( item );

      if ( list_.GetItemCount() > 0 ) {
        unsigned int index = std::min( item_index, list_.GetItemCount() - 1 );
        list_.GetItem( index ).SetFocused( true );
        list_.GetItem( index ).SetSelected( true );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DeleteAllEntry, [this] ( int, HWND ) -> WMResult {
    if ( list_.GetItemCount() > 0 ) {
      TtMessageBoxOkCancel box;
      box.SetMessage( StrT::Main::MBDeleteAllEntryCautionMessage.Get() );
      box.SetCaption( StrT::Main::MBDeleteAllEntryCautionCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::QUESTION );
      if ( box.ShowDialog( *this ) == TtMessageBox::Result::OK ) {
        this->ClearEntries();
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ShowPropertyOfEntry, [this] ( int, HWND ) -> WMResult {
    this->ShowPropertyOfEntry();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::EditIndividualParameter, [this] ( int, HWND ) -> WMResult {
    MainList::Item item = list_.GetSelected();
    if ( item.IsValid() ) {
      Entry* entry = item.GetParameter();
      if ( NOT( entry->parameter_ ) ) {
        TtMessageBoxYesNoCancel box;
        box.SetMessage( StrT::Main::MBEditIndividualParameterCopyCautionMessage.Get() );
        box.SetCaption( StrT::Main::MBEditIndividualParameterCopyCautionCaption.Get() );
        box.SetIcon( TtMessageBox::Icon::QUESTION );
        auto ret = box.ShowDialog( *this );
        if ( ret == TtMessageBox::Result::CANCEL ) {
          return {WMResult::Done};
        }
        if ( ret == TtMessageBox::Result::YES ) {
          entry->parameter_.emplace( common_parameter_ );
        }
        else {
          entry->parameter_.emplace();
        }
      }
      ParameterPropertySheet sheet( *entry->parameter_, entry->path_ );
      sheet.ShowDialog( *this );
      this->IfToggleButtonIsPressedDisplayCell();
    }
    return {WMResult::Done};
  } );

  // -- 変換
  this->AddCommandHandler( CommandID::ConvertOneBms, [this] ( int, HWND ) -> WMResult {
    MainList::Item item = list_.GetSelected();
    if ( item.IsValid() ) {
      this->ConvertEntry( item.GetParameter() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ConvertAllBms, [this] ( int, HWND ) -> WMResult {
    std::vector<Entry*> entries;
    for ( unsigned int i = 0; i < list_.GetItemCount(); ++i ) {
      MainList::Item item = list_.GetItem( i );
      entries.push_back( item.GetParameter() );
    }
    this->ConvertEntries( entries );
    return {WMResult::Done};
  } );

  // -- ツール
  this->AddCommandHandler( CommandID::Settings, [this] ( int, HWND ) -> WMResult {
    SettingsPropertySheet sheet( settings_ );
    sheet.ShowDialog( *this );
    this->SaveSettingsToFile();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ReloadSquirrelScript, [this] ( int, HWND ) -> WMResult {
    this->InitializeSquirrelVM();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ShowSquirrelOutputDialog, [this] ( int, HWND ) -> WMResult {
    squirrel_standard_output_dialog_.Show();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::StartSearcher, [this] ( int, HWND ) -> WMResult {
    std::string searcher_path = TtPath::GetExecutingDirectoryPath() + "\\bmx2wav_searcher.exe";
    int64_t ret = reinterpret_cast<int64_t>( ::ShellExecute( NULL, "open", searcher_path.c_str(), NULL, NULL, SW_SHOWNORMAL ) );
    if ( ret <= 32 ) {
      DWORD error_code = ::GetLastError();
      TtMessageBoxOk box;
      box.SetMessage( Utility::Format( StrT::Main::MBStartSearcherErrorMessage.Get(), searcher_path, TtUtility::GetWindowsSystemErrorMessage( error_code ) ) );
      box.SetCaption( StrT::Main::MBStartSearcherErrorCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    return {WMResult::Done};
  } );

  // -- ヘルプ
  this->AddCommandHandler( CommandID::VersionInformation, [this] ( int, HWND ) -> WMResult {
    VersionDialog dialog;
    dialog.ShowDialog( *this );
    return {WMResult::Done};
  } );

  // -- リスト要素ドラッグ&ドロップ
  this->AddNotifyHandler( CommandID::Control::MainList, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // アイテム右クリック
    case NM_RCLICK: {
      POINT cursor_point = TtWindow::GetCursorPosition();
      MainList::Item hit_item = list_.HitTest( cursor_point.x - list_.GetPoint().x, cursor_point.y - list_.GetPoint().y - 2 ); // 2 ずらす必要アリ
      if ( hit_item.IsValid() ) {
        hit_item.SetSelected( true );
      }
      MainList::Item item = list_.GetSelected();
      if ( item.IsValid() ) {
        entry_menu_.last_selected_item_ = item;
        entry_menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
      return {WMResult::Done};
    }

      // アイテムダブルクリック
    case LVN_ITEMACTIVATE: {
      this->ShowPropertyOfEntry();
      return {WMResult::Done};
    }

      // カラムクリック
    case LVN_COLUMNCLICK: {
      this->SortEntries( tool_bar_.select_column_group_box_.GetSelectedIndex(), reinterpret_cast<NMLISTVIEW*>( nmhdr )->iSubItem );
      return {WMResult::Done};
    }

      // ドラッグ & ドロップ周り
    case LVN_BEGINDRAG: {
      NMLISTVIEW* nm = reinterpret_cast<NMLISTVIEW*>( nmhdr );
      MainList::Item list_item( &list_, nm->iItem );
      list_item.SetSelected( true );
      drag_handler_ = list_item.MakeDragImage();
      POINT start_point = drag_handler_.GetStartPoint();
      drag_handler_.Begin( 0, nm->ptAction.x - start_point.x, nm->ptAction.y - start_point.y );
      drag_handler_.Enter( list_, nm->ptAction.x, nm->ptAction.y );
      this->SetCapture();
      return {WMResult::Done};
    }
    }
    return {WMResult::Incomplete};
  } );

  this->RegisterWMMouseMove( [this] ( int, int x, int y ) -> WMResult {
    if ( drag_handler_.IsEntered() ) {
      int current_x = x - this->ConvertToClientPoint( list_.GetPoint() ).x;
      int current_y = y - this->ConvertToClientPoint( list_.GetPoint() ).y;

      MainList::Item hit_item = list_.HitTest( current_x, current_y );
      if ( hit_item.IsValid() && NOT( hit_item.IsDropHilighted() ) ) {
        drag_handler_.ReEnter( current_x, current_y, [&] ( void ) {
          drag_handler_.CancelDropHilighted();
          hit_item.SetDropHilight( true );
          drag_handler_.SetDropHilightedItem( hit_item );
          list_.Update();
        } );
      }
      else if ( hit_item.IsInvalid() ) {
        drag_handler_.ReEnter( current_x, current_y, [&] ( void ) {
          drag_handler_.CancelDropHilighted();
          MainList::Item invalid_item( nullptr, MainList::Item::INVALID_INDEX );
          drag_handler_.SetDropHilightedItem( invalid_item );
          // drag_handler_.SetDropHilightedItem( MainList::Item( nullptr, MainList::Item::INVALID_INDEX ) );
          list_.Update();
        } );
      }
      else {
        drag_handler_.Move( current_x, current_y );
      }
    }
    return {WMResult::Incomplete};
  }, true );

  this->RegisterSingleHandler( WM_LBUTTONUP, [this] ( WPARAM, LPARAM ) -> WMResult {
    if ( drag_handler_.IsEntered() ) {
      drag_handler_.Leave();
      drag_handler_.End();
      TtWindow::ReleaseCapture();
      MainList::Item to_item = list_.GetDropHilight();
      MainList::Item from_item = drag_handler_.GetItem();
      drag_handler_.CancelDropHilighted();
      list_.MoveItem( from_item, to_item );
      list_.Update();
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  }, true );


  // ファイル D&D
  this->DragAcceptFiles( true );
  this->RegisterWMDropFiles( [&] ( HDROP drop ) -> WMResult {
    this->SetForground();

    unsigned int count = ::DragQueryFile( drop, static_cast<UINT>( -1 ), nullptr, 0 );
    for ( unsigned int i = 0; i < count; ++i ) {
      const unsigned int buf_size = ::DragQueryFile( drop, i, nullptr, 0 ) + 1;
      auto buf = std::make_unique<char[]>( buf_size );
      ::DragQueryFile( drop, i, buf.get(), buf_size );
      this->AddEntry( buf.get() );
      this->IfToggleButtonIsPressedDisplayCell();
    }

    ::DragFinish( drop );

    return {WMResult::Done};
  } );

  // -- 終了時
  this->RegisterWMClose( [this] ( void ) -> WMResult {
    IniFileOperation::SavePlacement( this->GetWindowPlacement() );
    IniFileOperation::SaveAutoDisplayCells( main_menu_.view_menu_.GetItemAt( 0 ).GetCheck() );
    common_parameter_.WriteToFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );

    return {WMResult::Incomplete}; // 処理差し込みの為
  }, false );

  // -- テスト -----
  this->AddCommandHandler( CommandID::Test1, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test2, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test3, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );
}


void
Mainstay::MainFrame::LoadPlacementFromIniFile( void )
{
  WINDOWPLACEMENT placement;
  if ( IniFileOperation::LoadPlacement( placement ) ) {
    this->SetWindowPlacement( placement );
  }
}

void
Mainstay::MainFrame::LoadSettingsFromFile( void )
{
  settings_.ReadFromFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );
}

void
Mainstay::MainFrame::SaveSettingsToFile( void )
{
  settings_.WriteToFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );
}


void
Mainstay::MainFrame::SetAccelerator( void )
{
  using Modifier = TtForm::AcceleratorMap::ShortcutKey::Modifier;
  TtForm::AcceleratorMap map;
  map.Register( {Modifier::None, VK_DELETE}, CommandID::DeleteEntry );

  TtForm::RegisterAccelerator( *this, map );
}


void
Mainstay::MainFrame::SetEntryProcessorMenu( void )
{
  auto register_menu_command_handler = [this] ( TtSubMenuCommand& menu ) {
    // ここキャプチャで menu を保持する
    entry_processor_menu_holder_.push_back( TtUtility::SharedDestructorCall( [this, menu] ( void ) mutable -> void {
      this->RemoveMenuCommandHandler( menu );
    } ) );

    this->AddMenuCommandHandler( menu, [this] ( TtMenuItem item ) -> WMResult {
      TtSquirrel::Object object = *item.GetParameterAs<TtSquirrel::Object*>();
      this->SquirrelErrorHandling( [&] ( void ) {
        vm_->CallExecuteOf( object, entry_menu_.last_selected_item_.GetParameter() );
      } );
      return {WMResult::Done};
    } );
  };

  entry_processor_menu_holder_.clear();
  this->SquirrelErrorHandling( [&] ( void ) {
    entry_processor_menu_maker_ = vm_->GetSubMenuCommandMakerOfEntryProcessors();
    entry_processor_menu_maker_.SetAtMakeMenu( register_menu_command_handler );
    entry_menu_.SetEntryProcessorMenu( entry_processor_menu_maker_.MakeMenu() );
  } );
  if ( entry_processor_menu_maker_.GetRoot().empty() ) {
    TtSubMenuCommand menu = TtSubMenuCommand::Create();
    TtMenuItem item = menu.AppendNewItem( 0, StrT::Main::EntryProcessorMenuEmpty.Get() );
    item.SetEnabled( false );
    item.SetParameterAs<void*>( nullptr );
    entry_menu_.SetEntryProcessorMenu( menu );
  }
}


void
Mainstay::MainFrame::SetSortMethodToStatusBar( const std::string& str )
{
  status_bar_.SetTextAt( 0, StrT::Main::StatusBarSortMethodTitle.Get() + str );
}


void
Mainstay::MainFrame::ShowPropertyOfEntry( void )
{
  MainList::Item item = list_.GetSelected();
  if ( item.IsValid() ) {
    EntryDialog dx( item.GetParameter(), *this );
    dx.ShowDialog( *this );
    this->IfToggleButtonIsPressedDisplayCell();
  }
}


void
Mainstay::MainFrame::ClearEntries( void )
{
  list_.ClearItems();
  entry_pool_.clear();
}

void
Mainstay::MainFrame::DisplayEntriesFromEntryPool( void )
{
  list_.StopRedraw( [&] ( void ) {
    list_.ClearItems();
    for ( auto& entry : entry_pool_ ) {
      list_.MakeNewItemFromEntry( entry.get() );
    }
  } );
  list_.SetWidthOfFirstColumnAuto();
}

void
Mainstay::MainFrame::DisplayColumn( void )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    std::string first = vm_->FirstColumnIsNotNull() ? vm_->GetFirstColumnName() : StrT::Main::DefaultFirstColumnName.Get();
    list_.ResetColumnsBy( first, vm_->GetColumnNamesFromColumnGroupIndex( tool_bar_.select_column_group_box_.GetSelectedIndex() ) );
  } );
}

void
Mainstay::MainFrame::DisplayCell( void )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    for ( unsigned int i = 0; i < list_.GetItemCount(); ++i ) {
      MainList::Item item = list_.GetItem( i );
      Entry* entry = item.GetParameter();
      vm_->CallParseAsBmsDataOnce( entry );
      item.SetText( this->GetStringForFirstColumnCellFromEntry( entry ) );
      for ( unsigned int k = 1; k < list_.GetColumnCount(); ++k ) {
        std::string result = vm_->CallColumnDisplayCell( tool_bar_.select_column_group_box_.GetSelectedIndex(), k, entry );
        item.SetSubItemText( k, result );
      }
    }
    list_.SetWidthOfFirstColumnAuto();
  } );
}

std::string
Mainstay::MainFrame::GetStringForFirstColumnCellFromEntry( Entry* entry )
{
  std::string tmp;
  this->SquirrelErrorHandling( [&] ( void ) {
    tmp = vm_->FirstColumnIsNotNull() ? vm_->CallFirstColumnDisplayCell( entry ) : entry->path_;
  } );
  return tmp;
}

void
Mainstay::MainFrame::IfToggleButtonIsPressedDisplayCell( void )
{
  if ( main_menu_.view_menu_.GetItemAt( 0 ).GetCheck() ) {
    this->DisplayCell();
  }
}

void
Mainstay::MainFrame::ParseEachBmsOfEntry( void )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    for ( unsigned int i = 0; i < list_.GetItemCount(); ++i ) {
      MainList::Item item = list_.GetItem( i );
      vm_->CallParseAsBmsDataOnce( item.GetParameter() );
    }
  } );
}


void
Mainstay::MainFrame::AddEntry( const std::string& path )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    entry_pool_.push_back( std::make_shared<Entry>( path ) );
    MainList::Item item = list_.MakeNewItemFromEntry( entry_pool_.back().get() );
    vm_->CallEntryConstructorAndPushIt( entry_pool_.back().get() );
    item.SetText( this->GetStringForFirstColumnCellFromEntry( entry_pool_.back().get() ) );
    list_.SetWidthOfFirstColumnAuto();
  } );
}


void
Mainstay::MainFrame::DeleteEntry( MainList::Item item )
{
  auto it = TtUtility::FindIf( entry_pool_, [&item] ( auto& it ) {
    return it.get() == item.GetParameter();
  } );
  if ( it != entry_pool_.end() ) {
    entry_pool_.erase( it );
  }
  item.Remove();
}


void
Mainstay::MainFrame::SortEntries( unsigned int group_index, unsigned int column_index )
{
  // 同じ sort なら逆順にする
  std::pair<unsigned int, unsigned int> sort_parameter = {group_index, column_index};
  bool ascending = (last_sort_parameter_ != sort_parameter);
  last_sort_parameter_ = ascending ? sort_parameter : MainFrame::NoSorted;

  this->SetSortMethodToStatusBar( list_.GetColumn( column_index ).GetText() + (ascending ? "" : StrT::Main::StatusBarSortMethodDescending.Get()) );

  if ( column_index == 0 ) {
    this->SquirrelErrorHandling( [&] ( void ) {
      if ( vm_->FirstColumnIsNotNull() ) {
        list_.Sort<Entry*>( [&] ( Entry* x, Entry* y ) -> int {
          int tmp = 0;
          this->SquirrelErrorHandling( [&] ( void ) {
            tmp = vm_->CallFirstColumnCompare( x, y );
          } );
          return tmp;
        }, ascending );
      }
      else {
        list_.Sort<Entry*>( [&] ( Entry* x, Entry* y ) -> int {
          return TtString::Compare( x->path_, y->path_ );
        }, ascending );
      }
    } );
    return;
  }

  list_.Sort<Entry*>( [&] ( Entry* x, Entry* y ) -> int {
    int tmp = 0;
    this->SquirrelErrorHandling( [&] ( void ) {
      tmp = vm_->CallColumnCompare( group_index, column_index, x, y );
    } );
    return tmp;
  }, ascending );
}


void
Mainstay::MainFrame::ConvertEntry( Entry* entry )
{
  Core::ConvertParameter parameter = common_parameter_;
  if ( entry->parameter_ ) {
    parameter = entry->parameter_.value();
  }
  parameter.input_file_path_ = entry->path_;
  ConverterDialog dialog( parameter );
  dialog.ShowDialog( *this );
}

void
Mainstay::MainFrame::ConvertEntries( std::vector<Entry*> entries )
{
  if ( entries.empty() ) {
    return;
  }
  multiple_convert_dialog_.emplace( *this, entries );
  multiple_convert_dialog_->ShowDialog( *this );
  this->Hide();
  multiple_convert_dialog_->Show();
  multiple_convert_dialog_->ConvertStart();
}


void
Mainstay::MainFrame::InitializeSquirrelVM( void )
{
  vm_.emplace( this );
  vm_->SetCommonParameter( &common_parameter_ );
  vm_->Initialize();
  vm_->SetPrintFunction( [&dialog = squirrel_standard_output_dialog_] ( const std::string& str ) {
    dialog.AddText( str );
  } );

  this->SquirrelErrorHandling( [&] ( void ) {
    vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + SquirrelVMBase::BASE_FILENAME );
    vm_->InitializeForMainstay();

    vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + SquirrelVM::CLASS_FILENAME );
    if ( auto tmp = TtPath::GetExecutingFilePathCustomExtension( "nut" ); TtPath::FileExists( tmp ) ) {
      vm_->DoFile( tmp );
    }

    // -- Entry Menu
    this->SetEntryProcessorMenu();

    // -- ColumnGroup
    main_menu_.column_group_menu_.Clear();
    tool_bar_.select_column_group_box_.Clear();
    for ( std::string& name : vm_->GetColumnGroupNames() ) {
      main_menu_.column_group_menu_.AppendNewItem( 0, name );
      tool_bar_.select_column_group_box_.Push( name );
    }
    main_menu_.column_group_menu_.CheckRadioItem( 0 );
    tool_bar_.select_column_group_box_.SetSelect( 0 );
    this->DisplayColumn();

    // 登録済 Entry に関して
    for ( unsigned int i = 0; i < list_.GetItemCount(); ++i ) {
      MainList::Item item = list_.GetItem( i );
      Entry* entry = item.GetParameter();
      vm_->CallEntryConstructorAndPushIt( entry );
      item.SetText( this->GetStringForFirstColumnCellFromEntry( entry ) );
    }
    list_.SetWidthOfFirstColumnAuto();

    this->IfToggleButtonIsPressedDisplayCell();
  } );
}

void
Mainstay::MainFrame::SquirrelErrorHandling( std::function<void ( void )> function )
{
  this->SquirrelErrorHandlingReturnErrorNotOccurred( [&] ( void ) {
    function();
    return true;
  } );
}

bool
Mainstay::MainFrame::SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function )
{
  try {
    return function();
  }
  catch ( TtSquirrel::Exception& ex ) {
    std::string tmp = StrT::Main::MBScriptErrorMessage.Get();
    std::string message = ex.GetStandardMessage();
    if ( NOT( message.empty() ) ) {
      tmp.append( "\r\n\r\n" );
      tmp.append( message );
    }
    tmp.append( "\r\n" );
    squirrel_standard_output_dialog_.AddText( tmp );

    TtMessageBoxOk box;
    box.SetMessage( tmp );
    box.SetCaption( StrT::Main::MBScriptErrorCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
}
