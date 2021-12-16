// searcher/searcher_frame.h

#pragma once

#include <optional>

#include "tt_form.h"

#include "squirrel_vm_base.h"
#include "searcher_squirrel_vm.h"
#include "utility_dialogs.h"

#include "searcher/searcher_control.h"
#include "searcher/searcher_dragdrop.h"
#include "searcher/searcher_entry.h"
#include "searcher/searcher_settings.h"


namespace BMX2WAV::Searcher {
  class MainFrame : public TtForm {
  private:
    static std::pair<unsigned int, unsigned int> NoSorted;

  public:
    explicit MainFrame( void );
    ~MainFrame();

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;
    virtual bool  Created( void ) override;

    void RegisterHandlers( void );
    void SetAccelerator( void );

    void LoadPlacementFromIniFile( void );
    void LoadSettingsFromFile( void );
    void SaveSettingsToFile( void );

    void SetSortMethodToStatusBar( const std::string& str );

    void SetAutoDisplayCellButtonFromSettings( void );

    void SetSelectTargetFolder( const std::string& target );
    void SetSelectTargetFolderFromSettingsHomeFolder( void );

    void SetEntryProcessorMenu( void );

    void WaitEntryPoolThread( void );
    void ManupilateEntryPool( bool asynchronous, std::function<void ( void )> manupilation );

    void ClearEntries( void );
    void GetEntriesFromTreeItemToEntryPool( MainTree::Item item );
    void GetEntriesOfSearchMethodToEntryPoolAndDisplayEntries( MainTree::Item item, unsigned int search_index, unsigned int filter_index );
    void DisplayEntriesFromEntryPool( unsigned int filter_index );
    void DisplayColumn( unsigned int column_group_index );
    void DisplayCell( unsigned int column_group_index );
    void IfToggleButtonIsPressedDisplayCell( unsigned int column_group_index );
    void SortEntries( unsigned int group_index, unsigned int column_index );
    bool IfSearchedFlagIsTrueThenShowConfirmMessageBox( void );

    void InitializeSquirrelVM( void );
    void SquirrelErrorHandling( std::function<void ( void )> function );
    bool SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function );

  private:
    std::optional<SquirrelVM> vm_;

    EntryPool                       entry_pool_;
    std::optional<TtFunctionThread> entry_pool_thread_;

    TtSubMenuCommandMaker                        entry_processor_menu_maker_;
    std::vector<TtUtility::SharedDestructorCall> entry_processor_menu_holder_;

    std::pair<unsigned int, unsigned int>        last_sort_parameter_;

    Settings settings_;
    bool     searched_flag_;

    // -- control member -------------------------------------------------
    // -- 全体 -----
    MainMenu                main_menu_;
    MainToolBar             tool_bar_;
    TtVirticalSplitterPanel splitter_panel_;
    MainTree                tree_;
    MainList                list_;
    TtStatusBar             status_bar_;

    // -- ポップアップメニュー
    TreeMenu  tree_menu_;
    EntryMenu entry_menu_;

    // -- 他 -----
    OutputDialog squirrel_standard_output_dialog_;

    // -- ツリー D&D 絡み
    MainList::Item::DragHandler drag_handler_;
    ListDataObject*             list_data_object_;
    ListDropSource*             list_drop_source_;
    ListDropTarget*             list_drop_target_;
  };
}
