// searcher/searcher_control.h

#pragma once

#include "tt_window.h"
#include "tt_menu.h"
#include "tt_combo_box.h"
#include "tt_tool_bar.h"
#include "tt_tree_view.h"
#include "tt_list_view.h"

#include "searcher/item_id_list.h"
#include "searcher/searcher_entry.h"


namespace BMX2WAV::Searcher {
  // -- MainMenu ---------------------------------------------------------
  class MainMenu : public TtMenuBar {
  public:
    explicit MainMenu( void );

  public:
    TtSubMenu file_menu_;
    TtSubMenu export_menu_;
    TtSubMenu tool_menu_;
    TtSubMenu prototype_menu_;
  };

  // -- TreeMenu ---------------------------------------------------------
  class TreeMenu : public TtSubMenu {
  public:
    explicit TreeMenu( void );
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
    DropDownBox select_filter_box_;
    DropDownBox select_column_group_box_;
    DropDownBox select_search_box_;

    using Label = TtWindowWithStyle<TtStatic, SS_CENTER | SS_CENTERIMAGE>;
    Label filter_label_;
    Label select_column_label_;
    Label execute_search_label_;
  };

  // -- MainTree ---------------------------------------------------------
  class MainTree : public TtWindowWithStyle<TtTreeView, TtTreeView::Style::HASBUTTONS | TtTreeView::Style::SHOWSELALWAYS | TtTreeView::Style::DISABLEDRAGDROP | TtTreeView::Style::HASLINES | TtTreeView::Style::LINESATROOT> {
  public:
    using Item = TtTreeItemWith<ItemIDList*>;

    explicit MainTree( TtWindow& parent_window );
    virtual bool CreatedInternal( void ) override;

    void SetRoot( void );
    void AddItemIDList( Item parent, ItemIDList* child_id );

    void AddChildrenOf( Item parent );

    std::shared_ptr<ItemIDList> GetSharedPointerFrom( ItemIDList* item_id_list );

  private:
    TtWindow&                                parent_window_;
    std::vector<std::shared_ptr<ItemIDList>> item_id_list_pool_;
  };

  // -- MainList ---------------------------------------------------------
  class MainList : public TtWindowWithStyle<TtListViewReport, LVS_SHOWSELALWAYS> {
  public:
    static const unsigned int FIRST_COLUMN_DEFAULT_WIDTH = 80;

  public:
    using Item = TtListViewItemWith<Entry*>;

    explicit MainList( void );
    virtual bool CreatedInternal( void ) override;

    void ResetColumnsBy( const std::vector<std::string>& columns );

    void MoveItem( unsigned int from, unsigned int to );

    std::string GetDataAsCSV( const std::string& delimiter );

    void MakeNewItemFromEntry( Entry* entry );

    void SetWidthOfFirstColumnAuto( void );
  };
}
