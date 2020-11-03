// searcher_squirrel_vm.h

#pragma once

#include "tt_menu.h"

#include "squirrel_vm_base.h"

#include "searcher/searcher_entry.h"


namespace BMX2WAV::Searcher {
  // -- SquirrelVM -------------------------------------------------------
  class SquirrelVM : public BMX2WAV::SquirrelVMBase {
  public:
    static const char* const CLASS_FILENAME;

  private:
    static TtSquirrel::VirtualMachine::Closure ConvertClosure( std::function<int ( SquirrelVM& )> closure );

  public:
    using SquirrelVMBase::SquirrelVMBase;

    void InitializeForSearcher( EntryPool* entry_pool );

    std::vector<std::string> GetDisplayFilterNames( void );
    std::vector<std::string> GetColumnGroupNames( void );
    std::vector<std::string> GetSearchMethodNames( void );

    void CallEntryConstructor( Entry* entry );
    bool CallDisplayFilter( unsigned int index, Entry* entry );
    bool CallSearchMethodSearch( unsigned int index, Entry* entry );
    void CallSearchMethodByEachDirectory( unsigned int index, std::shared_ptr<DirectoryEntry> directory_entry, std::vector<Entry*>& entries );

    void CallParseAsBmsDataOnce( Entry* entry );

    std::vector<std::string> GetColumnNamesFromColumnGroupIndex( unsigned int index );
    std::string CallColumnDisplayCell( unsigned int group_index, unsigned int column_index, Entry* entry );
    int CallColumnCompare( unsigned int group_index, unsigned int column_index, Entry* x, Entry* y );

    TtSubMenuCommandMaker GetSubMenuCommandMakerOfEntryProcessors( void );
    void CallExecuteOf( TtSquirrel::Object object, Entry* target_entry, std::vector<Entry*> entries );

  private:
    EntryPool* entry_pool_;
  };
}
