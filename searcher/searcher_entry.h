// searcher_entry.h

#pragma once

#include <atomic>

#include "tt_thread.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_message_queue.h"

#include "tt_squirrel_object.h"

#include "base/bms_data.h"

#include "searcher/item_id_list.h"


namespace BMX2WAV::Searcher {
  class FileEntry;
  class DirectoryEntry;

  // -- Entry ------------------------------------------------------------
  class Entry {
  public:
    explicit Entry( void );

    virtual ~Entry();

    template <class TYPE>
    bool TypeIs( void ) {
      return dynamic_cast<TYPE*>( this ) != nullptr;
    }
    bool IsFile( void );
    bool IsDirectory( void );

    template <class TYPE>
    TYPE* CastTo( void ) {
      return dynamic_cast<TYPE*>( this );
    }
    FileEntry* CastToFile( void );
    DirectoryEntry* CastToDirectory( void );

    virtual std::string GetPath( void ) = 0;
    virtual std::string GetDisplayName( void ) = 0;
    virtual std::string GetTypeName( void ) = 0;
    virtual int GetIconIndex( void ) = 0;

  public:
    int                               entry_number_;
    std::optional<TtSquirrel::Object> squirrel_object_;
    bool                              search_hit_;
  };

  // -- WithItemIDListEntry ----------------------------------------------
  class WithItemIDListEntry : public Entry {
  protected:
    explicit WithItemIDListEntry( std::shared_ptr<ItemIDList> item_id_list );

  public:
    virtual std::string GetPath( void ) override;
    virtual std::string GetDisplayName( void ) override;
    virtual std::string GetTypeName( void ) override;
    virtual int GetIconIndex( void ) override;

  public:
    std::shared_ptr<ItemIDList> item_id_list_;
  };

  // -- FileEntry --------------------------------------------------------
  class FileEntry : public WithItemIDListEntry {
    friend class EntryPool;

  private:
    explicit FileEntry( std::shared_ptr<ItemIDList> item_id_list );

  public:
    void ParseAsBmsData( void );
    void ParseAsBmsDataOnce( void );

  public:
    std::shared_ptr<BL::BmsData> bms_data_;
  };

  // -- DirectoryEntry ---------------------------------------------------
  class DirectoryEntry : public WithItemIDListEntry {
    friend class EntryPool;

  private:
    explicit DirectoryEntry( std::shared_ptr<ItemIDList> item_id_list );
  };

  // -- EntryPool --------------------------------------------------------
  class EntryPool : private std::vector<std::shared_ptr<Entry>> {
  public:
    class WaitThreadDialog;

  public:
    explicit EntryPool( void );

    using base = std::vector<std::shared_ptr<Entry>>;
    using base::begin;
    using base::end;
    using base::clear;
    using base::size;
    using base::at;

    void SetEntryCallback( std::function<void ( Entry* entry )> callback );

    std::shared_ptr<Entry>          MakeNewEntryFromItemIDList( std::shared_ptr<ItemIDList> item_id_list );
    std::shared_ptr<FileEntry>      MakeNewFileEntry( std::shared_ptr<ItemIDList> item_id_list );
    std::shared_ptr<DirectoryEntry> MakeNewDirectoryEntry( std::shared_ptr<ItemIDList> item_id_list );

    void AddEntry( std::shared_ptr<Entry> entry );
    void AddNewFileEntry( std::shared_ptr<ItemIDList> item_id_list );
    void AddNewDirectoryEntry( std::shared_ptr<ItemIDList> item_id_list );

    void StartParseThread( void );
    void EndAndWaitParseThread( void );

  private:
    std::function<void ( Entry* entry )> entry_callback_;

    std::optional<TtFunctionThread> parse_thread_;
    TtMessageQueue<FileEntry*>      parse_queue_;
    std::atomic_bool                parse_abort_;
  };

  // -- EntryPool::WaitThreadDialog ----------------------------------
  class EntryPool::WaitThreadDialog : public TtDialog {
  public:
    explicit WaitThreadDialog( TtFunctionThread& parent_thread );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

  private:
    TtFunctionThread&               parent_thread_;
    std::optional<TtFunctionThread> thread_;

    using Label = TtWindowWithStyle<TtStatic, SS_CENTER>;
    Label wait_label_;
  };
}
