// mainstay/custom_control.h

#pragma once

#include "tt_menu.h"
#include "tt_tool_bar.h"
#include "tt_list_view.h"
#include "tt_combo_box.h"

#include "mainstay/entry.h"


namespace BMX2WAV::Mainstay {
  // -- MainMenu ---------------------------------------------------------
  class MainMenu : public TtMenuBar {
  public:
    explicit MainMenu( void );

  public:
    TtSubMenu file_menu_;
    TtSubMenu view_menu_;
    TtSubMenu edit_menu_;
    TtSubMenu convert_menu_;
    TtSubMenu tool_menu_;
    TtSubMenu help_menu_;
    TtSubMenu prototype_menu_;

    TtSubMenuCommand column_group_menu_;
  };

  // -- EntryMenu --------------------------------------------------------
  class EntryMenu : public TtSubMenu {
  public:
    explicit EntryMenu( void );

    void SetEntryProcessorMenu( TtSubMenuCommand& menu );

  public:
    TtSubMenu                  entry_processor_menu_;
    TtListViewItemWith<Entry*> last_selected_item_;
  };

  // -- MainToolBar ------------------------------------------------------
  class MainToolBar : public TtWindowWithStyle<TtToolBar, TtToolBar::Style::Flat | TtToolBar::Style::List | TtToolBar::Style::WithToolTips> {
  public:
    explicit MainToolBar( void );

    virtual bool CreatedInternal( void ) override;

  public:
    using DropDownBox = TtWindowWithStyle<TtComboBox, CBS_DROPDOWN | CBS_DISABLENOSCROLL | WS_VSCROLL>;
    DropDownBox select_column_group_box_;

    using Label = TtWindowWithStyle<TtStatic, SS_CENTER | SS_CENTERIMAGE>;
    Label select_column_label_;
  };

  // -- MainList ---------------------------------------------------------
  class MainList : public TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_SHOWSELALWAYS> {
  private:
    static const unsigned int COLUMN_DEFAULT_WIDTH = 80;

  public:
    using Item = TtListViewItemWith<Entry*>;
    // using Column = TtListViewColumnWith<ColumnInfo*>;

    explicit MainList( void );
    virtual bool CreatedInternal( void ) override;

    void ResetColumnsBy( const std::string& first, const std::vector<std::string>& columns );

    void MoveItem( Item& from, Item& to );

    Item MakeNewItemFromEntry( Entry* entry );

    void SetWidthOfFirstColumnAuto( void );
  };
}
