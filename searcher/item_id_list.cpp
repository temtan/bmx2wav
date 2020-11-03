// searcher/item_id_list.cpp

#include <shlwapi.h>

#include "tt_utility.h"
#include "tt_exception.h"

#include "searcher/item_id_list.h"

#pragma comment (lib, "shlwapi.lib")

#pragma warning(disable : 4090)

using namespace BMX2WAV;


// -- ItemIDList ---------------------------------------------------------
std::shared_ptr<Searcher::ItemIDList>
Searcher::ItemIDList::GetDrivesItemIDList( TtWindow& window )
{
  ITEMIDLIST* handle;
  ::SHGetSpecialFolderLocation( window.GetHandle(), CSIDL_DRIVES, &handle );

  IShellFolder* desktop_folder_raw;
  ::SHGetDesktopFolder( &desktop_folder_raw );
  std::shared_ptr<IShellFolder> desktop_folder( desktop_folder_raw, [] ( IShellFolder* folder ) { folder->Release(); } );

  return std::make_shared<ItemIDList>( handle, desktop_folder );
}


Searcher::ItemIDList::ItemIDList( ITEMIDLIST* handle, std::shared_ptr<IShellFolder> desktop_folder ) :
handle_( handle ),
desktop_folder_( desktop_folder )
{
}

Searcher::ItemIDList::~ItemIDList()
{
  ::CoTaskMemFree( handle_ );
}


ITEMIDLIST*
Searcher::ItemIDList::GetHandle( void )
{
  return handle_;
}


std::string
Searcher::ItemIDList::GetPath( void )
{
  char buf[1024];
  auto ret = ::SHGetPathFromIDList( handle_, buf );
  if ( ret == FALSE ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetPathFromIDList ) );
  }
  return buf;
}

std::string
Searcher::ItemIDList::GetPathSafety( void )
{
  try {
    return this->GetPath();
  }
  catch ( TtSystemCallException ) {
    return "";
  }
}


std::string
Searcher::ItemIDList::GetDisplayName( void )
{
  SHFILEINFO info;
  auto ret = ::SHGetFileInfo( reinterpret_cast<LPTSTR>( handle_ ), 0, &info, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_DISPLAYNAME  );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetFileInfo ) );
  }
  return info.szDisplayName;
}

std::string
Searcher::ItemIDList::GetTypeName( void )
{
  SHFILEINFO info;
  auto ret = ::SHGetFileInfo( reinterpret_cast<LPTSTR>( handle_ ), 0, &info, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_TYPENAME );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetFileInfo ) );
  }
  return info.szTypeName;
}


int
Searcher::ItemIDList::GetIconIndex( void )
{
  SHFILEINFO info;
  ::SHGetFileInfo( reinterpret_cast<LPTSTR>( handle_ ), 0, &info, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_SYSICONINDEX );
  return info.iIcon;
}

int
Searcher::ItemIDList::GetOpenIconIndex( void )
{
  SHFILEINFO info;
  ::SHGetFileInfo( reinterpret_cast<LPTSTR>( handle_ ), 0, &info, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_OPENICON );
  return info.iIcon;
}


SFGAOF
Searcher::ItemIDList::GetAttributes( void )
{
  SHFILEINFO info;
  auto ret = ::SHGetFileInfo( reinterpret_cast<LPTSTR>( handle_ ), 0, &info, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_ATTRIBUTES );
  if ( ret == 0 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetFileInfo ) );
  }
  return info.dwAttributes;
}

bool
Searcher::ItemIDList::HasSubfolder( void )
{
  return this->GetAttributes() & SFGAO_HASSUBFOLDER;
}

bool
Searcher::ItemIDList::IsGhosted( void )
{
  return this->GetAttributes() & SFGAO_GHOSTED;
}

bool
Searcher::ItemIDList::IsLink( void )
{
  return this->GetAttributes() & SFGAO_LINK;
}

bool
Searcher::ItemIDList::IsShare( void )
{
  return this->GetAttributes() & SFGAO_SHARE;
}

bool
Searcher::ItemIDList::IsFolder( void )
{
  return this->GetAttributes() & SFGAO_FOLDER;
}

bool
Searcher::ItemIDList::IsStorage( void )
{
  return this->GetAttributes() & SFGAO_STORAGE;
}

bool
Searcher::ItemIDList::IsFileSystem( void )
{
  return this->GetAttributes() & SFGAO_FILESYSTEM;
}


std::vector<std::shared_ptr<Searcher::ItemIDList>>
Searcher::ItemIDList::GetChildrenFolders( TtWindow& window )
{
  return this->GetChildren( window, SHCONTF_FOLDERS );
}

std::vector<std::shared_ptr<Searcher::ItemIDList>>
Searcher::ItemIDList::GetChildrenNonFolders( TtWindow& window )
{
  return this->GetChildren( window, SHCONTF_NONFOLDERS );
}

std::vector<std::shared_ptr<Searcher::ItemIDList>>
Searcher::ItemIDList::GetChildren( TtWindow& window, SHCONTF flag )
{
  std::vector<std::shared_ptr<ItemIDList>> v;

  IShellFolder* current_folder;
  desktop_folder_->BindToObject( handle_, NULL, IID_PPV_ARGS( &current_folder ) );
  TtUtility::DestructorCall current_folder_releaser( [current_folder] ( void ) {
    current_folder->Release();
  } );

  IEnumIDList* enum_id_list;
  if ( current_folder->EnumObjects( window.GetHandle(), flag, &enum_id_list ) != S_OK ) {
    return v;
  }
  TtUtility::DestructorCall enum_id_list_releaser( [enum_id_list] ( void ) {
    enum_id_list->Release();
  } );

  ITEMIDLIST* child;
  while ( enum_id_list->Next( 1, &child, NULL ) == S_OK ) {
    TtUtility::DestructorCall child_releaser( [child] ( void ) {
      ::CoTaskMemFree( child );
    } );
    v.push_back( std::make_shared<ItemIDList>( ::ILCombine( handle_, child ), desktop_folder_ ) );
  }
  return v;
}
