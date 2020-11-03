// mainstay/custom_control.cpp

#include "common.h"

#include "mainstay/custom_control.h"

using namespace BMX2WAV;


// -- MainMenu -----------------------------------------------------------
Mainstay::MainMenu::MainMenu( void ) :
TtMenuBar( false ),

file_menu_( TtSubMenu::Create() ),
view_menu_( TtSubMenu::Create() ),
edit_menu_( TtSubMenu::Create() ),
convert_menu_( TtSubMenu::Create() ),
tool_menu_( TtSubMenu::Create() ),
help_menu_( TtSubMenu::Create() ),
prototype_menu_( TtSubMenu::Create() ),

column_group_menu_( TtSubMenuCommand::Create() )
{
  auto add_menu = [] ( TtMenu& menu, int command_id, Image::Index::Value image_index, const std::string& str ) {
    menu.AppendNewItem( command_id, str );
    if ( image_index != Image::Index::None ) {
      menu.GetLastItem().SetBmpImage( Image::BMPS[image_index] );
    }
  };
  using Index = Image::Index;

  this->AppendMenu( file_menu_, "ファイル(&F)" );
  add_menu( file_menu_, CommandID::Close, Index::None, "終了(&X)" );

  this->AppendMenu( view_menu_, "表示(&V)" );
  add_menu( view_menu_, CommandID::AutoDisplayCells, Index::AutoDisplayCells, "自動でセルの内容を表示する(&T)" );
  view_menu_.GetLastItem().SetCheck( true );
  view_menu_.AppendMenu( column_group_menu_, "表示するカラムグループ(&G)" );
  add_menu( view_menu_, CommandID::DisplayCells,     Index::DisplayCells,     "セルの内容を表示する(&D)" );
  add_menu( view_menu_, CommandID::Reload,           Index::Reload,           "BMSの再読込(&R)" );

  this->AppendMenu( edit_menu_, "編集(&E)" );
  add_menu( edit_menu_, CommandID::EditCommonParameter,     Index::EditCommonParameter,     "共通設定の編集(&E)..." );
  edit_menu_.AppendSeparator();
  add_menu( edit_menu_, CommandID::AddEntry,                Index::AddEntry,                "BMSの追加(&A)" );
  edit_menu_.AppendSeparator();
  add_menu( edit_menu_, CommandID::DeleteEntry,             Index::DeleteEntry,             "BMSの削除(&D)" );
  add_menu( edit_menu_, CommandID::DeleteAllEntry,          Index::DeleteAllEntry,          "全BMSの削除(&E)" );
  edit_menu_.AppendSeparator();
  add_menu( edit_menu_, CommandID::ShowPropertyOfEntry,     Index::ShowPropertyOfEntry,     "BMSのプロパティ(&P)..." );
  add_menu( edit_menu_, CommandID::EditIndividualParameter, Index::EditIndividualParameter, "BMSの個別設定(&I)..." );

  this->AppendMenu( convert_menu_, "変換(&C)" );
  add_menu( convert_menu_, CommandID::ConvertOneBms, Index::ConvertOneBms, "個別に変換する(&C)..." );
  add_menu( convert_menu_, CommandID::ConvertAllBms, Index::ConvertAllBms, "全て変換する(&A)..." );

  this->AppendMenu( tool_menu_, "ツール(&T)" );
  /*
  // TODO いる？
  add_menu( tool_menu_, CommandID::Settings,              Index::None, "環境設定(&S)..." );
  tool_menu_.AppendSeparator();
   */
  add_menu( tool_menu_, CommandID::ReloadSquirrelScript,     Index::ReloadSquirrelScript,     "スクリプトファイルを再読み込みする(&R)" );
  add_menu( tool_menu_, CommandID::ShowSquirrelOutputDialog, Index::ShowSquirrelOutputDialog, "スクリプト出力ダイアログを表示する(&O)..." );
  tool_menu_.AppendSeparator();
  add_menu( tool_menu_, CommandID::StartSearcher,            Index::SearcherMainSmall,        "BMX2WAV Searcherを起動する(&T)..." );

  this->AppendMenu( help_menu_, "ヘルプ(&H)" );
  add_menu( help_menu_, CommandID::VersionInformation, Index::None, "バージョン情報(&A)..." );

  if ( IniFileOperation::LoadTestMode() ) {
    this->AppendMenu( prototype_menu_, "開発中" );
    add_menu( prototype_menu_, CommandID::Test1, Index::None, "Test1" );
    add_menu( prototype_menu_, CommandID::Test2, Index::None, "Test2" );
    add_menu( prototype_menu_, CommandID::Test3, Index::None, "Test3" );
  }
}


// -- EntryMenu ----------------------------------------------------------
Mainstay::EntryMenu::EntryMenu( void ) :
TtSubMenu( TtSubMenuCommand::Create() ),
entry_processor_menu_( TtSubMenuCommand::Create() ),
last_selected_item_( nullptr, TtListViewItem::INVALID_INDEX )
{
  auto add_menu = [this] ( int command_id, Image::Index::Value image_index, const std::string& str ) {
    this->AppendNewItem( command_id, str );
    if ( image_index != Image::Index::None ) {
      this->GetLastItem().SetBmpImage( Image::BMPS[image_index] );
    }
  };
  using Index = Image::Index;

  add_menu( CommandID::ShowPropertyOfEntry,     Index::ShowPropertyOfEntry,     "プロパティ(&P)" );
  add_menu( CommandID::EditIndividualParameter, Index::EditIndividualParameter, "個別設定編集(&E)" );
  this->AppendSeparator();
  add_menu( CommandID::ConvertOneBms,           Index::ConvertOneBms,           "変換(&C)" );
  this->AppendSeparator();
  add_menu( CommandID::DeleteEntry,             Index::DeleteEntry,             "削除(&D)" );
  this->AppendSeparator();
  this->AppendMenu( entry_processor_menu_, "スクリプト関数を実行する(&F)" );
  // この辺の構成を変えたら↓の EntryProcessorElement の数値を変える事
}


void
Mainstay::EntryMenu::SetEntryProcessorMenu( TtSubMenuCommand& menu )
{
  const int EntryProcessorElement = 7;
  this->InsertMenu( EntryProcessorElement, menu, this->GetItemAt( EntryProcessorElement ).GetText() );
  this->DeleteAt( EntryProcessorElement + 1 );
  entry_processor_menu_ = menu;
}


// -- MainToolBar --------------------------------------------------------
Mainstay::MainToolBar::MainToolBar( void )
{
}

bool
Mainstay::MainToolBar::CreatedInternal( void )
{
  this->TtToolBar::CreatedInternal();

  this->SetExtendedStyle( TtToolBar::ExtendedStyle::DrawDropDwonArrows | TtToolBar::ExtendedStyle::MixedButtons );
  this->SetImageList( *Image::LIST );

  namespace ID = CommandID::MainToolBar;
  using Index = Image::Index;

  auto add_button = [this] ( int command_id, int image_index, const std::string& str, int style ) {
    this->AddButtonWithString( command_id, Image::LIST->GetOffsetIndex( image_index ), str, style );
  };
  auto add_standard_button = [this, add_button] ( int command_id, int image_index, const std::string& str ) {
    add_button( command_id, image_index, str, TtToolBar::Button::Style::Standard );
  };
  auto add_drop_down_button = [this, add_button] ( int command_id, int image_index, const std::string& str, bool is_whole ) {
    add_button( command_id, image_index, str,
                TtToolBar::Button::Style::Standard | (is_whole ? TtToolBar::Button::Style::WholeDropDown : TtToolBar::Button::Style::DropDown ) );
  };
  auto add_drop_down_box = [this] ( int command_id, auto& box, int width, int height ) {
    // For ComboBox
    this->AddSeparator( command_id );
    // セパレータに対しては幅変更になる
    this->GetButton( command_id ).SetBmpImageIndex( width + 8 );

    box.Create( {this, command_id} );
    box.SetPositionSize( this->GetButton( command_id ).GetRectangle().left + 4, 2, width, height );
    box.Show();
  };
  auto add_label = [this] ( int command_id, TtStatic& label, const std::string& str, int width ) {
    // For Label
    this->AddSeparator( command_id );
    // セパレータに対しては幅変更になる
    this->GetButton( command_id ).SetBmpImageIndex( width + 4 );

    label.Create( {this} );
    label.SetPositionSize( this->GetButton( command_id ).GetRectangle().left + 4, 0, width, 22 );
    label.SetText( str );
    label.Show();
  };

  add_button( ID::AutoDisplayCells, Index::AutoDisplayCells, "セルの自動表示", TtToolBar::Button::Style::Standard | TtToolBar::Button::Style::Check );
  add_label( ID::SelectColumnLabel, select_column_label_, "カラム表示", 60 );
  add_drop_down_box( ID::SelectColumnGroup, select_column_group_box_, 120, 120 );
  add_standard_button( ID::DisplayCells, Index::DisplayCells, "セルの内容を表示する" );
  add_standard_button( ID::Reload,       Index::Reload,       "BMSの再読込" );
  this->AddSeparator();
  add_standard_button( ID::EditCommonParameter, Index::EditCommonParameter, "共通設定の編集" );
  this->AddSeparator();
  add_standard_button( ID::AddEntry,       Index::AddEntry,       "BMSの追加" );
  add_standard_button( ID::DeleteEntry,    Index::DeleteEntry,    "BMSの削除" );
  add_standard_button( ID::DeleteAllEntry, Index::DeleteAllEntry, "全BMSの削除" );
  this->AddSeparator();
  add_standard_button( ID::ShowPropertyOfEntry,     Index::ShowPropertyOfEntry,     "BMSのプロパティ" );
  add_standard_button( ID::EditIndividualParameter, Index::EditIndividualParameter, "BMSの個別設定" );
  this->AddSeparator();
  add_standard_button( ID::ConvertOneBms, Index::ConvertOneBms, "個別に変換する" );
  add_standard_button( ID::ConvertAllBms, Index::ConvertAllBms, "全て変換する");
  this->AddSeparator();
  add_standard_button( ID::ReloadSquirrelScript,     Index::ReloadSquirrelScript,     "スクリプトファイルを再読み込みする");
  add_standard_button( ID::ShowSquirrelOutputDialog, Index::ShowSquirrelOutputDialog, "スクリプト出力ダイアログを表示する");
  this->AddSeparator();
  add_standard_button( ID::StartSearcher, Index::SearcherMainSmall, "BMX2WAV Searcherを起動する");

  return true;
}


// -- MainList -----------------------------------------------------------
Mainstay::MainList::MainList( void )
{
}

bool
Mainstay::MainList::CreatedInternal( void )
{
  this->SetFullRowSelect( true );
  this->SetHasGridLines( true );
  this->SetHeaderDragAndDrop( true );
  this->SetSmallImageList( *Image::LIST );
  return true;
}

void
Mainstay::MainList::ResetColumnsBy( const std::string& first, const std::vector<std::string>& columns )
{
  this->ClearColumns();
  {
    auto column = this->MakeNewColumn();
    column.SetText( first );
    column.SetWidth( MainList::COLUMN_DEFAULT_WIDTH );
  }
  for ( auto& name : columns ) {
    auto column = this->MakeNewColumn();
    column.SetText( name );
    column.SetWidth( MainList::COLUMN_DEFAULT_WIDTH );
  }
}


void
Mainstay::MainList::MoveItem( Item& from, Item& to )
{
  if ( from.GetIndex() >= this->GetItemCount() || to.GetIndex() >= this->GetItemCount() ) {
    return;
  }
  if ( from.GetIndex() == to.GetIndex() ) {
    return;
  }

  std::vector<std::string> tmp;
  Entry* p = from.GetParameter();
  for ( unsigned int i = 0; i < this->GetColumnCount(); ++i ) {
    tmp.push_back( from.GetSubItemText( i ) );
  }
  // これ以降 item の Index がずれるので注意
  Item item = this->InsertNewItem( to.GetIndex() );
  item.SetParameter( p );
  item.SetImageIndex( Image::LIST->GetOffsetIndex( Image::Index::BmsFile ) );
  for ( unsigned int i = 0; i < tmp.size(); ++i ) {
    item.SetSubItemText( i, tmp[i] );
  }
  this->GetItem( from.GetIndex() + (from.GetIndex() > to.GetIndex() ? 1 : 0) ).Remove();
}

Mainstay::MainList::Item
Mainstay::MainList::MakeNewItemFromEntry( Entry* entry )
{
  Item item = this->MakeNewItem();
  item.SetParameter( entry );
  item.SetImageIndex( Image::LIST->GetOffsetIndex( Image::Index::BmsFile ) );
  return item;
}

void
Mainstay::MainList::SetWidthOfFirstColumnAuto( void )
{
  if ( this->GetItemCount() == 0 ) {
    this->GetColumn( 0 ).SetWidth( MainList::COLUMN_DEFAULT_WIDTH );
  }
  else {
    this->GetColumn( 0 ).SetWidthAuto();
  }
}
