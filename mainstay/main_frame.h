// mainstay/main_frame.h

#pragma once

#include "tt_form.h"

#include "utility_dialogs.h"
#include "mainstay/custom_control.h"
#include "mainstay/entry.h"
#include "mainstay/main_squirrel_vm.h"
#include "mainstay/multiple_convert_dialog.h"


namespace BMX2WAV::Mainstay {
  // -- MainFrame --------------------------------------------------------
  class MainFrame : public TtForm {
    friend class EntryDialog;
    friend class MultipleConvertDialog;

  private:
    static std::pair<unsigned int, unsigned int> NoSorted;

  public:
    explicit MainFrame( void );

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;
    virtual bool  Created( void ) override;

    void RegisterHandlers( void );

    void LoadPlacementFromIniFile( void );

    void SetAccelerator( void );

    void SetEntryProcessorMenu( void );

    void SetSortMethodToStatusBar( const std::string& str );

    void ShowPropertyOfEntry( void );

    void ClearEntries( void );
    void DisplayEntriesFromEntryPool( void );
    void DisplayColumn( void );
    void DisplayCell( void );
    std::string GetStringForFirstColumnCellFromEntry( Entry* entry );
    void IfToggleButtonIsPressedDisplayCell( void );
    void ParseEachBmsOfEntry( void );

    void AddEntry( const std::string& path );
    void DeleteEntry( MainList::Item item );

    void SortEntries( unsigned int group_index, unsigned int column_index );

    void ConvertEntry( Entry* entry );
    void ConvertEntries( std::vector<Entry*> entries );

    void InitializeSquirrelVM( void );
    void SquirrelErrorHandling( std::function<void ( void )> function );
    bool SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function );

  private:
    std::optional<SquirrelVM> vm_;

    std::vector<std::shared_ptr<Entry>> entry_pool_;

    Core::ConvertParameter common_parameter_;

    TtSubMenuCommandMaker                        entry_processor_menu_maker_;
    std::vector<TtUtility::SharedDestructorCall> entry_processor_menu_holder_;

    std::pair<unsigned int, unsigned int>        last_sort_parameter_;

    std::optional<MultipleConvertDialog>         multiple_convert_dialog_;

    // -- control member -------------------------------------------------
    // -- 全体 -----
    MainMenu    main_menu_;
    MainToolBar tool_bar_;
    MainList    list_;
    TtStatusBar status_bar_;

    // -- ポップアップメニュー
    EntryMenu entry_menu_;

    // -- 他 -----
    OutputDialog squirrel_standard_output_dialog_;

    // リストビュー操作絡み
    MainList::Item::DragHandler drag_handler_;
  };
}
