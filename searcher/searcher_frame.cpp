// searcher/searcher_frame.cpp

#include <compare>

#include "tt_path.h"
#include "tt_message_box.h"
#include "tt_file_dialog.h"
#include "tt_clipboard.h"

#include "exception.h"
#include "utility.h"
#include "string_table.h"

#include "core/wave_maker.h"

#include "searcher/searcher_common.h"
#include "searcher/searcher_settings_property_sheet.h"

#include "searcher/searcher_frame.h"

using namespace BMX2WAV;


// -- Searcher::MainFrame ------------------------------------------------
std::pair<unsigned int, unsigned int>
Searcher::MainFrame::NoSorted = {std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()};

Searcher::MainFrame::MainFrame( void ) :
vm_( std::nullopt ),

entry_pool_(),
entry_pool_thread_( std::nullopt ),

entry_processor_menu_maker_(),
entry_processor_menu_holder_(),

last_sort_parameter_(),

settings_(),
searched_flag_( false ),

main_menu_(),
tool_bar_(),
splitter_panel_( 4, 24, 24 ),
tree_( *this ),
list_(),
status_bar_(),

drag_handler_(),
list_data_object_( nullptr ),
list_drop_source_( nullptr )
{
  ::OleInitialize( NULL );
  list_data_object_ = new ListDataObject();
  list_drop_source_ = new ListDropSource();
  list_drop_target_ = new ListDropTarget( list_, drag_handler_ );
}


Searcher::MainFrame::~MainFrame()
{
  list_data_object_->Release();
  list_drop_source_->Release();
  list_drop_target_->Release();
  ::OleUninitialize();
}


DWORD
Searcher::MainFrame::GetStyle( void )
{
  return WS_OVERLAPPEDWINDOW;
}

DWORD
Searcher::MainFrame::GetExtendedStyle( void )
{
  return 0;
}


bool
Searcher::MainFrame::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetIconAsSmall( Image::ICONS[Image::Index::MainSmall] );
  this->SetText( APPLICATION_NAME + std::string( " " ) + VERSION );
  this->SetMenu( main_menu_ );

  // -- コントロール作成 -----
  tool_bar_.Create( {this, CommandID::Control::MainToolBar} );
  splitter_panel_.Create( {this} ); {
    tree_.Create( {&splitter_panel_, CommandID::Control::MainTree} );
    list_.Create( {&splitter_panel_, CommandID::Control::MainList} );
    splitter_panel_.SetNorthWindow( &tree_ );
    splitter_panel_.SetSouthWindow( &list_ );
  }
  splitter_panel_.SetSplitterPosition( Const::DefaultSplitterPosition );

#ifdef _WIN64
  // x64 だと自動でツールチップが表示されない為
  this->AddNotifyHandler( CommandID::Control::MainToolBar, tool_bar_.MakeNotifyHandlerForToolTipByStringTable() );
#endif

  status_bar_.Create( {this} );
  status_bar_.DivideInto( 1, {-1} );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int f, int width, int height ) -> WMResult {
    const int tool_bar_height = 28;
    tool_bar_.SetPositionSize(        0,               0,     width,              tool_bar_height );
    splitter_panel_.SetPositionSize(  1, tool_bar_height, width - 2, height - tool_bar_height - 20 );

    status_bar_.SendMessage( WM_SIZE, f, MAKELPARAM( width, height ) );
    return {WMResult::Done};
  } );

  tree_.SetRoot();

  this->RegisterHandlers();

  tree_.Show();
  list_.Show();
  tool_bar_.Show();
  splitter_panel_.Show();

  // Show しないと表示されないので注意
  squirrel_standard_output_dialog_.ShowDialog( *this );
  squirrel_standard_output_dialog_.SetText( StrT::Searcher::Main::SquirrelStandardOutputDialogTitle.Get() );

  this->SetAccelerator();

  // 色々と初期化
  {
    last_sort_parameter_ = MainFrame::NoSorted;
    this->SetSortMethodToStatusBar( StrT::Searcher::Main::DefaultSortMethodName.Get() );
  }

  this->InitializeSquirrelVM();

  return true;
}


void
Searcher::MainFrame::RegisterHandlers( void )
{
  // -- コマンド ID 対応 -----
  // ファイル
  this->AddCommandHandler( CommandID::Close, [this] ( int, HWND ) -> WMResult {
    this->Close();
    return {WMResult::Done};
  } );

  // CSV 出力
  this->AddCommandHandler( CommandID::ExportCSVToFile, [this] ( int, HWND ) -> WMResult {
    TtSaveFileDialog dialog;
    dialog.GetFilters().push_back( {"CSVファイル(*.csv)", "*.csv"} );
    dialog.GetFilters().push_back( {"すべてのファイル(*.*)", "*.*"} );
    if( dialog.ShowDialog( *this ) ) {
      FILE* file;
      errno_t error_number = ::fopen_s( &file, dialog.GetFileName().c_str(), "wb" );
      if ( error_number != 0 ) {
        TtMessageBoxOk box;
        box.SetMessage( "ファイルを開くのに失敗しました。\r\nファイル名 ： " + dialog.GetFileName() );
        box.SetCaption( "ファイルオープンエラー" );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
        return {WMResult::Done};
      }
      int ret = ::fputs( list_.GetDataAsCSV( "," ).c_str(), file );
      ::fclose( file );
      if ( ret == EOF ) {
        TtMessageBoxOk box;
        box.SetMessage( "ファイルの書き込みに失敗しました。\r\nファイル名 ： " + dialog.GetFileName() );
        box.SetCaption( "ファイル書き込みエラー" );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExportTSVToClipboard, [this] ( int, HWND ) -> WMResult {
    TtClipboard::SetString( list_.GetDataAsCSV( "\t" ) );
    // エラー処理はよくわからん
    return {WMResult::Done};
  } );

  // ツール
  this->AddCommandHandler( CommandID::Settings, [this] ( int, HWND ) -> WMResult {
    SettingsPropertySheet sheet( settings_ );
    sheet.ShowDialog( *this );
    this->SetAutoDisplayCellButtonFromSettings();
    this->SaveSettingsToFile();
    return {WMResult::Done};
  } );

  // -- ツールバー -----
  this->AddNotifyHandler( CommandID::Control::MainToolBar, [this] ( NMHDR* nmhdr ) -> WMResult {
    if ( nmhdr->code == TBN_DROPDOWN ) {
      NMTOOLBAR* nm = reinterpret_cast<NMTOOLBAR*>( nmhdr );
      RECT rect = tool_bar_.GetButton( nm->iItem ).GetRectangle();
      POINT point = tool_bar_.ConvertToScreenPoint( {rect.left, rect.bottom} );
      switch ( nm->iItem ) {
        // エクスポート
      case CommandID::Export:
        main_menu_.export_menu_.PopupAbsolute( *this, point.x, point.y );
        break;

      default:
        ;
      }
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  } );

  this->AddCommandHandler( CommandID::ReloadSquirrelScript, [this] ( int, HWND ) -> WMResult {
    this->WaitEntryPoolThread();
    this->InitializeSquirrelVM();
    this->DisplayColumn( tool_bar_.select_column_group_box_.GetSelectedIndex() );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ShowSquirrelOutputDialog, [this] ( int, HWND ) -> WMResult {
    squirrel_standard_output_dialog_.Show();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::SelectFilter, [this] ( int code, HWND ) -> WMResult {
    if ( code == CBN_SELCHANGE ) {
      this->WaitEntryPoolThread();
      auto filter_index = tool_bar_.select_filter_box_.GetSelectedIndex();
      auto column_group_index = tool_bar_.select_column_group_box_.GetSelectedIndex();
      this->ManupilateEntryPool( settings_.asynchronous_display_, [&, filter_index, column_group_index] ( void ) {
        this->DisplayEntriesFromEntryPool( filter_index );
        this->IfToggleButtonIsPressedDisplayCell( column_group_index );
      } );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AutoDisplayCells, [this] ( int, HWND ) -> WMResult {
    settings_.auto_display_cells_ = tool_bar_.GetButton( CommandID::MainToolBar::ID::AutoDisplayCells ).IsChecked();
    this->SaveSettingsToFile();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::SelectColumnGroup, [this] ( int code, HWND ) -> WMResult {
    if ( code == CBN_SELCHANGE ) {
      this->WaitEntryPoolThread();
      auto filter_index = tool_bar_.select_filter_box_.GetSelectedIndex();
      auto column_group_index = tool_bar_.select_column_group_box_.GetSelectedIndex();
      this->ManupilateEntryPool( settings_.asynchronous_display_, [&, filter_index, column_group_index] ( void ) {
        this->DisplayColumn( column_group_index );
        this->DisplayEntriesFromEntryPool( filter_index );
        this->IfToggleButtonIsPressedDisplayCell( column_group_index );
      } );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DisplayCells, [this] ( int, HWND ) -> WMResult {
    this->WaitEntryPoolThread();
    auto column_group_index = tool_bar_.select_column_group_box_.GetSelectedIndex();
    this->ManupilateEntryPool( settings_.asynchronous_display_, [&, column_group_index] ( void ) {
      this->DisplayCell( column_group_index );
    } );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ExecuteSearch, [this] ( int, HWND ) -> WMResult {
    MainTree::Item item = tree_.GetSelected();
    if ( NOT( item.IsValid() ) || item.GetParameter()->GetPathSafety().empty() ) {
      TtMessageBoxOk box;
      box.SetCaption( StrT::Searcher::Main::MBExecuteSearchImpossibleDirectoryCaption.Get() );
      box.SetMessage( StrT::Searcher::Main::MBExecuteSearchImpossibleDirectoryMessage.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    else {
      this->WaitEntryPoolThread();

      TtMessageBoxYesNo box;
      box.SetMessage( Utility::Format( StrT::Searcher::Main::MBExecuteSearchExecuteConfirmMessage.Get(),
                                      item.GetParameter()->GetPathSafety().c_str(), tool_bar_.select_search_box_.GetSelectedString().c_str() ) );
      box.SetCaption( StrT::Searcher::Main::MBExecuteSearchExecuteConfirmCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::QUESTION );

      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        auto search_index = tool_bar_.select_search_box_.GetSelectedIndex();
        auto filter_index = tool_bar_.select_filter_box_.GetSelectedIndex();
        auto column_group_index = tool_bar_.select_column_group_box_.GetSelectedIndex();
        this->ManupilateEntryPool( settings_.asynchronous_display_, [&, item, search_index, filter_index, column_group_index] ( void ) {
          entry_pool_.StartParseThread();
          this->GetEntriesOfSearchMethodToEntryPoolAndDisplayEntries( item, search_index, filter_index );
          entry_pool_.EndAndWaitParseThread();
          this->IfToggleButtonIsPressedDisplayCell( column_group_index );
        } );
      }
    }
    return {WMResult::Done};
  } );

  // -- ポップアップメニュー等 -----
  this->AddCommandHandler( CommandID::OpenTreeDirectory, [this] ( int, HWND ) -> WMResult {
    MainTree::Item selected = tree_.GetSelected();
    if ( selected.IsInvalid() || selected == tree_.GetOrigin().GetFirstChild() ) {
      return {WMResult::Done};
    }
    auto path = selected.GetParameter()->GetPathSafety();
    if ( NOT( path.empty() ) ) {
      ::ShellExecute( handle_, "open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::HideEntry, [this] ( int, HWND ) -> WMResult {
    auto tmp = list_.GetSelectedIndices();
    int last_focused_index_ = -1;
    for ( auto it = tmp.rbegin(); it != tmp.rend(); ++it ) {
      MainList::Item item = list_.GetItem( *it );
      if ( item.IsValid() ) {
        if ( item.IsFocused() ) {
          last_focused_index_ = item.GetIndex();
        }
        item.Remove();
      }
    }
    if ( last_focused_index_ >= 0 && list_.GetItemCount() > 0 ) {
      unsigned int index = std::min( static_cast<unsigned int>( last_focused_index_ ), list_.GetItemCount() - 1 );
      list_.GetItem( index ).SetFocused( true );
      list_.GetItem( index ).SetSelected( true );
    }
    return {WMResult::Done};
  } );

  // -- ツリー操作 -----
  this->AddNotifyHandler( CommandID::Control::MainTree, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
    case TVN_ITEMEXPANDING: {
      NMTREEVIEW* nm = reinterpret_cast<NMTREEVIEW*>( nmhdr );
      MainTree::Item item = MainTree::Item( &tree_, nm->itemNew.hItem );
      if ( NOT( item.IsExpandedOnce() ) ) {
        tree_.StopRedraw( [&] ( void ) {
          tree_.AddChildrenOf( item );
        } );
      }
      return {WMResult::Done};
    }

    case TVN_SELCHANGED: {
      this->WaitEntryPoolThread();
      if ( searched_flag_ ) {
        TtMessageBoxYesNo box;
        box.SetMessage( StrT::Searcher::Main::MBSearchResultDestructionConfirmMessage.Get() );
        box.SetCaption( StrT::Searcher::Main::MBSearchResultDestructionConfirmCaption.Get() );
        box.SetIcon( TtMessageBox::Icon::QUESTION );
        if ( box.ShowDialog( *this ) == TtMessageBox::Result::NO ) {
          return {WMResult::Done};
        }
        searched_flag_ = false;
      }
      NMTREEVIEW* nm = reinterpret_cast<NMTREEVIEW*>( nmhdr );
      MainTree::Item item = MainTree::Item( &tree_, nm->itemNew.hItem );
      auto filter_index = tool_bar_.select_filter_box_.GetSelectedIndex();
      auto column_group_index = tool_bar_.select_column_group_box_.GetSelectedIndex();
      this->ManupilateEntryPool( settings_.asynchronous_display_, [&, item, filter_index, column_group_index] ( void ) {
        this->GetEntriesFromTreeItemToEntryPool( item );
        this->DisplayEntriesFromEntryPool( filter_index );
        this->IfToggleButtonIsPressedDisplayCell( column_group_index );
      } );
      return {WMResult::Done};
    }

      // 右クリック時
    case NM_RCLICK: {
      POINT cursor_point = TtWindow::GetCursorPosition();
      MainTree::Item hit_item = tree_.HitTest( cursor_point.x - tree_.GetPoint().x, cursor_point.y - tree_.GetPoint().y );
      if ( hit_item.IsValid() ) {
        hit_item.SetSelect();
      }
      MainTree::Item item = tree_.GetSelected();
      if ( item.IsValid() ) {
        tree_menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
      return {WMResult::Done};
    }

    }
    return {WMResult::Incomplete};
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
      MainList::Item item = list_.GetItem( nm->iItem );

      list_data_object_->GetSelectedIndices().clear();
      std::vector<std::string> paths;
      for ( auto index : list_.GetSelectedIndices() ) {
        MainList::Item selected_item = list_.GetItem( index );
        if ( selected_item.GetParameter()->IsFile() ) {
          paths.push_back( selected_item.GetParameter()->GetPath() );
        }
        list_data_object_->GetSelectedIndices().push_back( index );
      }
      list_data_object_->SetDataAsPathVector( paths );

      drag_handler_ = item.MakeDragImage();
      POINT start_point = drag_handler_.GetStartPoint();
      drag_handler_.Begin( 0, nm->ptAction.x - start_point.x, nm->ptAction.y - start_point.y );

      DWORD dwEffect;
      ::DoDragDrop( list_data_object_, list_drop_source_, DROPEFFECT_MOVE | DROPEFFECT_COPY, &dwEffect );

      if ( drag_handler_.IsEntered() ) {
        drag_handler_.Leave();
      }
      if ( drag_handler_.IsBegun() ) {
        drag_handler_.End();
      }
      return {WMResult::Done};
    }
    }
    return {WMResult::Incomplete};
  } );

  ::RegisterDragDrop( list_.GetHandle(), list_drop_target_ );
  list_drop_target_->SetDropHandler( [this] ( void ) {
    if ( drag_handler_.IsEntered() ) {
      drag_handler_.Leave();
      drag_handler_.End();

      MainList::Item to_item = list_.GetDropHilight();
      unsigned int to_index = to_item.GetIndex();
      std::vector<unsigned int> froms = list_data_object_->GetSelectedIndices();
      drag_handler_.CancelDropHilighted();

      while ( NOT( froms.empty() ) ) {
        unsigned int from_index = froms.back();
        list_.MoveItem( from_index, to_index );
        froms.pop_back();
        for ( auto& one : froms ) {
          if ( one >= to_index ) {
            one += 1;
          }
        }
        if ( to_index > from_index ) {
          to_index -= 1;
        }
      }

      list_.Update();
    }
  } );

  // -- 終了時
  this->RegisterWMClose( [this] ( void ) -> WMResult {
    IniFileOperation::SavePlacement( this->GetWindowPlacement() );
    IniFileOperation::SaveSplitterPosition( splitter_panel_.GetSplitterPosition() );
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
Searcher::MainFrame::SetAccelerator( void )
{
  using Modifier = TtForm::AcceleratorMap::ShortcutKey::Modifier;
  TtForm::AcceleratorMap map;
  map.Register( {Modifier::None, VK_DELETE}, CommandID::HideEntry );

  TtForm::RegisterAccelerator( *this, map );
}


void
Searcher::MainFrame::LoadPlacementFromIniFile( void )
{
  splitter_panel_.SetSplitterPosition( IniFileOperation::LoadSplitterPosition() );

  WINDOWPLACEMENT placement;
  if ( IniFileOperation::LoadPlacement( placement ) ) {
    this->SetWindowPlacement( placement );
  }
}

void
Searcher::MainFrame::LoadSettingsFromFile( void )
{
  settings_.ReadFromFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );
}

void
Searcher::MainFrame::SaveSettingsToFile( void )
{
  settings_.WriteToFile( TtPath::GetExecutingFilePathCustomExtension( "ini" ) );
}


void
Searcher::MainFrame::SetSortMethodToStatusBar( const std::string& str )
{
  status_bar_.SetTextAt( 0, StrT::Searcher::Main::StatusBarSortMethodTitle.Get() + str );
}


void
Searcher::MainFrame::SetAutoDisplayCellButtonFromSettings( void )
{
  tool_bar_.GetButton( CommandID::MainToolBar::ID::AutoDisplayCells ).SetCheck( settings_.auto_display_cells_ );;
}


void
Searcher::MainFrame::SetSelectTargetFolder( const std::string& target )
{
  auto root = tree_.GetOrigin().GetFirstChild();
  root.Expand();
  MainTree::Item item = root.GetFirstChild();

  while ( item.IsValid() ) {
    if ( item.GetParameter()->GetPath() == target ) {
      item.SetSelect();
      break;
    }

    bool is_root = TtPath::IsRoot( item.GetParameter()->GetPath() );
    if ( TtString::StartWith( target, item.GetParameter()->GetPath() + (is_root ? "" : "\\" ) ) ) {
      item.Expand();
      item = item.GetFirstChild();
    }
    else {
      item = item.GetNextSibling();
    }
  }
}

void
Searcher::MainFrame::SetSelectTargetFolderFromSettings( void )
{
  this->SetSelectTargetFolder( settings_.home_folder_ );
}


void
Searcher::MainFrame::SetEntryProcessorMenu( void )
{
  auto register_menu_command_handler = [this] ( TtSubMenuCommand& menu ) {
    // ここキャプチャで menu を保持する
    entry_processor_menu_holder_.push_back( TtUtility::SharedDestructorCall( [this, menu] ( void ) mutable -> void {
      this->RemoveMenuCommandHandler( menu );
    } ) );

    this->AddMenuCommandHandler( menu, [this] ( TtMenuItem item ) -> WMResult {
      TtSquirrel::Object object = *item.GetParameterAs<TtSquirrel::Object*>();
      std::vector<Entry*> v;
      for ( MainList::Item list_item : list_.GetSelectedItems() ) {
        v.push_back( list_item.GetParameter() );
      }
      this->SquirrelErrorHandling( [&] ( void ) {
        vm_->CallExecuteOf( object, entry_menu_.last_selected_item_.GetParameter(), v );
      } );
      return {WMResult::Done};
    } );
  };

  entry_processor_menu_holder_.clear();
  entry_processor_menu_maker_ = vm_->GetSubMenuCommandMakerOfEntryProcessors();
  entry_processor_menu_maker_.SetAtMakeMenu( register_menu_command_handler );
  entry_menu_.SetEntryProcessorMenu( entry_processor_menu_maker_.MakeMenu() );
  if ( entry_processor_menu_maker_.GetRoot().empty() ) {
    TtSubMenuCommand menu = TtSubMenuCommand::Create();
    TtMenuItem item = menu.AppendNewItem( 0, StrT::Searcher::Main::EntryProcessorMenuEmpty.Get() );
    item.SetEnabled( false );
    item.SetParameterAs<void*>( nullptr );
    entry_menu_.SetEntryProcessorMenu( menu );
  }
}


void
Searcher::MainFrame::WaitEntryPoolThread( void )
{
  if ( entry_pool_thread_ && NOT( entry_pool_thread_->HasExited() ) ) {
    EntryPool::WaitThreadDialog dialog( entry_pool_thread_.value() );
    dialog.ShowDialog( *this );
  }
}

void
Searcher::MainFrame::ManupilateEntryPool( bool asynchronous, std::function<void ( void )> manupilation )
{
  if ( asynchronous ) {
    if ( entry_pool_thread_ && NOT( entry_pool_thread_->HasExited() ) ) {
      throw BMX2WAV_INTERNAL_EXCEPTION;
    }
    entry_pool_thread_.emplace( manupilation );
    entry_pool_thread_->Start();
  }
  else {
    manupilation();
  }
}


void
Searcher::MainFrame::ClearEntries( void )
{
  entry_pool_.clear();
  list_.ClearItems();
}

void
Searcher::MainFrame::GetEntriesFromTreeItemToEntryPool( MainTree::Item item )
{
  entry_pool_.clear();
  if ( NOT( item.GetParameter()->IsFileSystem() ) ) {
    return;
  }
  for ( std::shared_ptr<ItemIDList>& child_id : item.GetParameter()->GetChildrenNonFolders( *this ) ) {
    entry_pool_.AddNewFileEntry( child_id );
  }
}

void
Searcher::MainFrame::GetEntriesOfSearchMethodToEntryPoolAndDisplayEntries( MainTree::Item item, unsigned int search_index, unsigned int filter_index )
{
  entry_pool_.clear();
  if ( NOT( item.GetParameter()->IsFileSystem() ) ) {
    return;
  }

  list_.ClearItems();

  std::function<void ( std::shared_ptr<ItemIDList> )> f = [&, last_index = static_cast<unsigned int>( 0 )] ( std::shared_ptr<ItemIDList> parent ) mutable {
    if ( TtPath::IsDirectory( parent->GetPathSafety() ) ) {
      std::vector<std::shared_ptr<Entry>> tmp_pool;
      for ( std::shared_ptr<ItemIDList>& child_id : parent->GetChildrenNonFolders( *this ) ) {
        auto tmp = entry_pool_.MakeNewFileEntry( child_id );
        if ( vm_->CallSearchMethodSearch( search_index, tmp.get() ) ) {
          tmp_pool.push_back( tmp );
        }
      }
      auto directory_entry = entry_pool_.MakeNewDirectoryEntry( parent );
      std::vector<Entry*> v;
      for ( auto& entry : tmp_pool ) {
        v.push_back( entry.get() );
      }
      vm_->CallSearchMethodByEachDirectory( search_index, directory_entry, v );

      while ( last_index != entry_pool_.size() ) {
        auto& tmp = entry_pool_.at( last_index );
        if ( tmp->IsDirectory() || vm_->CallDisplayFilter( filter_index, tmp.get() ) ) {
          list_.MakeNewItemFromEntry( tmp.get() );
        }
        ++last_index;
      }

      for ( auto& entry : tmp_pool ) {
        if ( entry->search_hit_ ) {
          entry_pool_.AddEntry( entry );

          if ( vm_->CallDisplayFilter( filter_index, entry.get() ) ) {
            list_.MakeNewItemFromEntry( entry.get() );
          }
          ++last_index;
        }
      }
      list_.SetWidthOfFirstColumnAuto();
    }
    for ( std::shared_ptr<ItemIDList>& child_id : parent->GetChildrenFolders( *this ) ) {
      f( child_id );
    }
  };

  this->SquirrelErrorHandling( [&] ( void ) {
    f( tree_.GetSharedPointerFrom( item.GetParameter() ) );
    searched_flag_ = true;
  } );
}

void
Searcher::MainFrame::DisplayEntriesFromEntryPool( unsigned int filter_index )
{
  list_.StopRedraw( [&] ( void ) {
    list_.ClearItems();
    this->SquirrelErrorHandling( [&] ( void ) {
      for ( auto& entry : entry_pool_ ) {
        if ( entry->IsDirectory() || vm_->CallDisplayFilter( filter_index, entry.get() ) ) {
          list_.MakeNewItemFromEntry( entry.get() );
        }
      }
    } );
    list_.SetWidthOfFirstColumnAuto();
  } );
}


void
Searcher::MainFrame::DisplayColumn( unsigned int column_group_index )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    list_.ResetColumnsBy( vm_->GetColumnNamesFromColumnGroupIndex( column_group_index ) );
  } );
}

void
Searcher::MainFrame::DisplayCell( unsigned int column_group_index )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    for ( unsigned int i = 0; i < list_.GetItemCount(); ++i ) {
      MainList::Item item = list_.GetItem( i );
      if ( item.GetParameter()->IsDirectory() ) {
        for ( unsigned int k = 1; k < list_.GetColumnCount(); ++k ) {
          item.SetSubItemText( k, "--------" );
        }
      }
      else {
        vm_->CallParseAsBmsDataOnce( item.GetParameter() );
        for ( unsigned int k = 1; k < list_.GetColumnCount(); ++k ) {
          std::string result = vm_->CallColumnDisplayCell( column_group_index, k, item.GetParameter() );
          item.SetSubItemText( k, result );
        }
      }
    }
  } );
}


void
Searcher::MainFrame::IfToggleButtonIsPressedDisplayCell( unsigned int column_group_index )
{
  if ( tool_bar_.GetButton( CommandID::MainToolBar::ID::AutoDisplayCells ).IsChecked() ) {
    this->DisplayCell( column_group_index );
  }
}


void
Searcher::MainFrame::SortEntries( unsigned int group_index, unsigned int column_index )
{
  // 同じ sort なら逆順にする
  std::pair<unsigned int, unsigned int> sort_parameter = {group_index, column_index};
  bool ascending = (last_sort_parameter_ != sort_parameter);
  last_sort_parameter_ = ascending ? sort_parameter : MainFrame::NoSorted;

  this->SetSortMethodToStatusBar( list_.GetColumn( column_index ).GetText() + (ascending ? "" : StrT::Searcher::Main::StatusBarSortMethodDescending.Get()) );

  if ( column_index == 0 ) {
    list_.Sort<Entry*>( [&] ( Entry* x, Entry* y ) -> int {
      return TtUtility::OrderingToInt( x->entry_number_ <=> y->entry_number_ );
    }, ascending );
    return;
  }

  list_.Sort<Entry*>( [&] ( Entry* x, Entry* y ) -> int {
    int tmp = 0;
    if ( x->IsDirectory() ) {
      if ( y->IsDirectory() ) {
        return TtUtility::OrderingToInt( y->entry_number_ <=> x->entry_number_ );
      }
      return 1;
    }
    else if ( y->IsDirectory() ) {
      return -1;
    }
    this->SquirrelErrorHandling( [&] ( void ) {
      tmp = vm_->CallColumnCompare( group_index, column_index, x, y );
    } );
    return tmp;
  }, ascending );
}


void
Searcher::MainFrame::InitializeSquirrelVM( void )
{
  this->ClearEntries();

  // -- Squirrel 関連
  vm_.emplace( this );
  vm_->Initialize();
  vm_->SetPrintFunction( [&dialog = squirrel_standard_output_dialog_] ( const std::string& str ) {
    dialog.AddText( str );
  } );

  this->SquirrelErrorHandling( [&] ( void ) {
    vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + SquirrelVMBase::BASE_FILENAME );

    vm_->InitializeForSearcher( &entry_pool_ );
    entry_pool_.SetEntryCallback( [&] ( Entry* entry ) { vm_->CallEntryConstructor( entry ); } );

    vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + SquirrelVM::CLASS_FILENAME );
    vm_->DoFile( TtPath::GetExecutingFilePathCustomExtension( "nut" ) );

    // -- Entry Menu
    this->SetEntryProcessorMenu();

    // -- DisplayFilter
    tool_bar_.select_filter_box_.Clear();
    for ( std::string& name : vm_->GetDisplayFilterNames() ) {
      tool_bar_.select_filter_box_.Push( name );
    }
    tool_bar_.select_filter_box_.SetSelect( 0 );

    // -- ColumnGroup
    tool_bar_.select_column_group_box_.Clear();
    for ( std::string& name : vm_->GetColumnGroupNames() ) {
      tool_bar_.select_column_group_box_.Push( name );
    }
    tool_bar_.select_column_group_box_.SetSelect( 0 );
    this->DisplayColumn( tool_bar_.select_column_group_box_.GetSelectedIndex() );

    // -- SearchMethod
    tool_bar_.select_search_box_.Clear();
    for ( std::string& name : vm_->GetSearchMethodNames() ) {
      tool_bar_.select_search_box_.Push( name );
    }
    tool_bar_.select_search_box_.SetSelect( 0 );
  } );
}

void
Searcher::MainFrame::SquirrelErrorHandling( std::function<void ( void )> function )
{
  this->SquirrelErrorHandlingReturnErrorNotOccurred( [&] ( void ) {
    function();
    return true;
  } );
}

bool
Searcher::MainFrame::SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function )
{
  try {
    return function();
  }
  catch ( TtSquirrel::Exception& ex ) {
    std::string tmp = StrT::Searcher::Main::MBScriptErrorMessage.Get();
    std::string message = ex.GetStandardMessage();
    if ( NOT( message.empty() ) ) {
      tmp.append( "\r\n\r\n" );
      tmp.append( message );
    }
    tmp.append( "\r\n" );
    squirrel_standard_output_dialog_.AddText( tmp );

    TtMessageBoxOk box;
    box.SetMessage( tmp );
    box.SetCaption( StrT::Searcher::Main::MBScriptErrorCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
}
