// searcher/item_id_list.h

#pragma once

#include <string>
#include <vector>

#include <shlobj.h>

#include "tt_window.h"


namespace BMX2WAV::Searcher {
  // -- ItemIDList ---------------------------------------------------------
  class ItemIDList {
  public:
    static std::shared_ptr<ItemIDList> GetDrivesItemIDList( TtWindow& window );

    explicit ItemIDList( ITEMIDLIST* handle, std::shared_ptr<IShellFolder> desktop_folder );
    ~ItemIDList();

    ITEMIDLIST* GetHandle( void );

    std::string GetPath( void );
    std::string GetPathSafety( void );
    std::string GetDisplayName( void );
    std::string GetTypeName( void );

    int GetIconIndex( void );
    int GetOpenIconIndex( void );

    SFGAOF GetAttributes( void );
    bool HasSubfolder( void );
    bool IsGhosted( void );
    bool IsLink( void );
    bool IsShare( void );
    bool IsFolder( void );
    bool IsStorage( void );
    bool IsFileSystem( void );

    std::vector<std::shared_ptr<ItemIDList>> GetChildrenFolders( TtWindow& window );
    std::vector<std::shared_ptr<ItemIDList>> GetChildrenNonFolders( TtWindow& window );
    std::vector<std::shared_ptr<ItemIDList>> GetChildren( TtWindow& window, SHCONTF flag );

  private:
    ITEMIDLIST*                   handle_;
    std::shared_ptr<IShellFolder> desktop_folder_;
  };
}
