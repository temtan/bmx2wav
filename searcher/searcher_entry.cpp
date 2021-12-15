// searcher_entry.cpp

#include "tt_path.h"
#include "tt_message_box.h"
#include "tt_form.h"

#include "base/parser.h"

#include "exception.h"
#include "string_table.h"

#include "searcher/searcher_common.h"

#include "searcher/searcher_entry.h"

using namespace BMX2WAV;


static int kazu = 0;

// -- Searcher::Entry ----------------------------------------------------
Searcher::Entry::Entry( void ) :
entry_number_( -1 ),
squirrel_object_( std::nullopt )
{
}

Searcher::Entry::~Entry()
{
}


bool
Searcher::Entry::IsFile( void )
{
  return this->TypeIs<FileEntry>();
}

bool
Searcher::Entry::IsDirectory( void )
{
  return this->TypeIs<DirectoryEntry>();
}


Searcher::FileEntry*
Searcher::Entry::CastToFile( void )
{
  return this->CastTo<FileEntry>();
}

Searcher::DirectoryEntry*
Searcher::Entry::CastToDirectory( void )
{
  return this->CastTo<DirectoryEntry>();
}


// -- WithItemIDListEntry ------------------------------------------------
Searcher::WithItemIDListEntry::WithItemIDListEntry( std::shared_ptr<ItemIDList> item_id_list ) :
Entry(),
item_id_list_( item_id_list )
{
}

std::string
Searcher::WithItemIDListEntry::GetPath( void )
{
 return item_id_list_->GetPathSafety();
}

std::string
Searcher::WithItemIDListEntry::GetDisplayName( void )
{
  return item_id_list_->GetDisplayName();
}

std::string
Searcher::WithItemIDListEntry::GetTypeName( void )
{
  return item_id_list_->GetTypeName();
}

int
Searcher::WithItemIDListEntry::GetIconIndex( void )
{
  return item_id_list_->GetIconIndex();
}

// -- FileEntry ----------------------------------------------------------
Searcher::FileEntry::FileEntry( std::shared_ptr<ItemIDList> item_id_list ) :
WithItemIDListEntry( item_id_list )
{
  if ( item_id_list->IsFolder() ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
}

void
Searcher::FileEntry::ParseAsBmsData( void )
{
  BL::Parser::Parser parser;
  parser.not_nesting_if_statement_ = true;
  try {
    bms_data_ = parser.Parse( this->GetPath() );
  }
  catch ( TtException& ) {
    bms_data_ = std::make_shared<BL::BmsData>();
    bms_data_->path_ = this->GetPath();
    bms_data_->most_serious_error_level_ = ErrorLevel::Internal;
  }
  catch ( ... ) {
    bms_data_ = std::make_shared<BL::BmsData>();
    bms_data_->path_ = this->GetPath();
    bms_data_->most_serious_error_level_ = ErrorLevel::Internal;
  }
}

void
Searcher::FileEntry::ParseAsBmsDataOnce( void )
{
  if ( NOT( bms_data_ ) ) {
    this->ParseAsBmsData();
  }
}

// -- DirectoryEntry -----------------------------------------------------
Searcher::DirectoryEntry::DirectoryEntry( std::shared_ptr<ItemIDList> item_id_list ) :
WithItemIDListEntry( item_id_list )
{
  if ( NOT( item_id_list->IsFolder() ) ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
}


// -- EntryPool ----------------------------------------------------------
Searcher::EntryPool::EntryPool( void ) :
entry_callback_( nullptr )
{
}

void
Searcher::EntryPool::SetEntryCallback( std::function<void ( Entry* entry )> callback )
{
  entry_callback_ = callback;
}


std::shared_ptr<Searcher::Entry>
Searcher::EntryPool::MakeNewEntryFromItemIDList( std::shared_ptr<ItemIDList> item_id_list )
{
  std::shared_ptr<Entry> entry;
  if ( item_id_list->IsFolder() ) {
    return this->MakeNewDirectoryEntry( item_id_list );
  }
  return this->MakeNewFileEntry( item_id_list );
}


std::shared_ptr<Searcher::FileEntry>
Searcher::EntryPool::MakeNewFileEntry( std::shared_ptr<ItemIDList> item_id_list )
{
  if ( item_id_list->IsFolder() ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  auto entry = std::shared_ptr<FileEntry>( new FileEntry( item_id_list ) );
  if ( entry_callback_ ) {
    entry_callback_( entry.get() );
  }
  return entry;
}

std::shared_ptr<Searcher::DirectoryEntry>
Searcher::EntryPool::MakeNewDirectoryEntry( std::shared_ptr<ItemIDList> item_id_list )
{
  if ( NOT( item_id_list->IsFolder() ) ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  auto entry = std::shared_ptr<DirectoryEntry>( new DirectoryEntry( item_id_list ) );
  if ( entry_callback_ ) {
    entry_callback_( entry.get() );
  }
  return entry;
}


void
Searcher::EntryPool::AddEntry( std::shared_ptr<Entry> entry )
{
  entry->entry_number_ = static_cast<int>( this->size() );
  this->push_back( entry );
  if ( parse_thread_ && entry->IsFile() ) {
    parse_queue_.PostMessage( entry->CastToFile() );
  }
}

void
Searcher::EntryPool::AddNewFileEntry( std::shared_ptr<ItemIDList> item_id_list )
{
  this->AddEntry( this->MakeNewFileEntry( item_id_list ) );
}

void
Searcher::EntryPool::AddNewDirectoryEntry( std::shared_ptr<ItemIDList> item_id_list )
{
  this->AddEntry( this->MakeNewDirectoryEntry( item_id_list ) );
}


void
Searcher::EntryPool::StartParseThread( void )
{
  if ( parse_thread_ && NOT( parse_thread_->HasExited() ) ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  parse_queue_.ClearMessage();
  parse_abort_.store( false );
  parse_thread_.emplace( [&] ( void ) {
    for ( FileEntry* entry = parse_queue_.GetMessage(); entry; entry = parse_queue_.GetMessage() ) {
      if ( parse_abort_.load() ) {
        break;
      }
      entry->ParseAsBmsDataOnce();
      if ( parse_abort_.load() ) {
        break;
      }
    }
  } );
  parse_thread_->Start();
}

void
Searcher::EntryPool::EndAndWaitParseThread( void )
{
  parse_abort_.store( true );
  parse_queue_.PostMessage( nullptr );
  if ( parse_thread_ ) {
    parse_thread_->Join();
    parse_thread_.reset();
  }
}


// -- EntryPool::WaitThreadDialog ----------------------------------------
Searcher::EntryPool::WaitThreadDialog::WaitThreadDialog( TtFunctionThread& parent_thread ) :
parent_thread_( parent_thread ),
thread_( std::nullopt )
{
}


DWORD
Searcher::EntryPool::WaitThreadDialog::GetStyle( void )
{
  return 0;
}

DWORD
Searcher::EntryPool::WaitThreadDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
Searcher::EntryPool::WaitThreadDialog::Created( void )
{
  this->SetText( StrT::Searcher::EntryWaitDialog::Title.Get() );

  wait_label_.Create( {this} );

  this->SetSize( 160, 90 );
  this->SetCenterRelativeToParent();
  wait_label_.SetPositionSize( 4, 12,  160 - 16, 60 );

  wait_label_.SetText( StrT::Searcher::EntryWaitDialog::Text.Get() );

  thread_.emplace( [&] ( void ) {
    parent_thread_.Join();
    this->Close();
  } );
  thread_->Start();

  wait_label_.Show();

  return true;
}
