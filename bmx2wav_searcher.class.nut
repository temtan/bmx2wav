
// ----- Display Filter 関連 ---------------------------------------------
// -- DisplayFilter
class DisplayFilter {
  constructor( name ) {
    this.name = name;
  }

  function filtering( entry ) {
    return true;
  }

  name = null;
};

// -- AllPassFilter
class AllPassFilter extends DisplayFilter {
  constructor() {
    base.constructor( StrT.Searcher.Main.Toolbar.DisplayFilterAllPass.get() );
  }

  function filtering( entry ) {
    return true;
  }
};

// -- ExtensionsFilter
class ExtensionsFilter extends DisplayFilter {
  constructor( name, ... ) {
    base.constructor( name );
    this.extensions = [];
    for ( local i = 0; i < vargv.len(); i++ ) {
      this.extensions.append( vargv[i].tolower() );
    }
  }

  function filtering( entry ) {
    local low_path = entry.path.tolower();
    foreach ( extension in this.extensions ) {
      if ( ::endswith( low_path, "." + extension ) ) {
        return true;
      }
    }
    return false;
  }

  extensions = null;
};

// -- 登録
Searcher.display_filters.append( ExtensionsFilter(
  StrT.Searcher.Main.Toolbar.DisplayFilterBmsGeneral.get(), "bms", "bme", "bml", "pms" ) );

Searcher.display_filters.append( ExtensionsFilter(
  StrT.Searcher.Main.Toolbar.DisplayFilterOnlyBms.get(), "bms" ) );

Searcher.display_filters.append( ExtensionsFilter(
  StrT.Searcher.Main.Toolbar.DisplayFilterOnlyBme.get(), "bme" ) );

// 処理が重いので非推奨
// Searcher.display_filters.append( ExtensionsFilter( "wav, oggのみ", "wav", "ogg" ) );
// Searcher.display_filters.append( AllPassFilter() );


// ----- ColumnGroup 関連 ------------------------------------------------
{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Simple.get() );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  Searcher.column_groups.append( group );
}

{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Details.get() );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "GENRE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "BPM", HeaderColumn.compare_as_integer_asc ) );
  group.columns.append( PlayingTimeColumn() );
  Searcher.column_groups.append( group );
}

{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Various.get() );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( ObjectCountColumn() );
  group.columns.append( ObjectOf1PCountColumn() );
  group.columns.append( ObjectOf2PCountColumn() );
  group.columns.append( ContainsRandomStatementColumn() );
  group.columns.append( ParentDirectoryColumn() );
  group.columns.append( ParentsParentDirectoryColumn() );
  group.columns.append( FullPathColumn() );
  Searcher.column_groups.append( group );
}

{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.ForSearch.get() );
  group.columns.append( ParentDirectoryColumn() );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( ParentsParentDirectoryColumn() );
  group.columns.append( FullPathColumn() );
  Searcher.column_groups.append( group );
}


// ----- SearchMethod 関連 -----------------------------------------------
class SearchMethod {
  constructor( name ) {
    this.name = name;
  }

  function search( entry ) {
    return true;
  }

  function by_each_directory( directory_entry, entry_array ) {
    // フォルダ毎に呼び出される関数
    // search で true になった entry の配列が entry_array
    // entry.search_hit（デフォルトはtrue）を false にするとリストに入らない
  }

  name = null;
};

// -- BmsAllSearchMethod
class BmsAllSearchMethod extends SearchMethod {
  constructor() {
    base.constructor( StrT.Searcher.Main.Toolbar.SearchMethodBmsAll.get() );
  }

  function search( entry ) {
    return this.filter.filtering( entry );
  }

  function by_each_directory( directory_entry, entry_array ) {
    if ( entry_array.len() > 0 ) {
      Searcher.add_directory_entry_to_list( directory_entry );
    }
  }

  filter = ExtensionsFilter( StrT.Searcher.Main.Toolbar.SearchMethodFilterBmsGeneral.get(), "bms", "bme", "bml", "pms" );
}

// -- MostPlaylevelSearchMethod
class MostPlaylevelSearchMethod extends SearchMethod {
  constructor() {
    base.constructor( StrT.Searcher.Main.Toolbar.SearchMethodMostPlaylevel.get() );
  }

  function search( entry ) {
    return this.filter.filtering( entry );
  }

  function by_each_directory( directory_entry, entry_array ) {
    if ( entry_array.len() > 0 ) {
      Searcher.add_directory_entry_to_list( directory_entry );
    }

    local key = "PLAYLEVEL";
    local current = null;
    foreach ( entry in entry_array ) {
      entry.parse_as_bms_data_once();

      if ( key in entry.bms_data.headers ) {
        try {
          if ( current == null ) {
            current = entry;
          }
          else if ( current.bms_data.headers[key].tointeger() > entry.bms_data.headers[key].tointeger() ) {
            entry.search_hit = false;
          }
          else {
            current.search_hit = false;
            current = entry;
          }
        }
        catch ( e ) {
          // continue;
        }
      }
    }
  }

  filter = ExtensionsFilter( StrT.Searcher.Main.Toolbar.SearchMethodFilterBmsGeneral.get(), "bms", "bme", "bml", "pms" );
}

// -- 登録
Searcher.search_methods.append( BmsAllSearchMethod() );
Searcher.search_methods.append( MostPlaylevelSearchMethod() );


// -- EntryProcessors 関連 -----------------------------------------------
{
  local preview = {
    name = StrT.Searcher.Main.PopupEntry.ProcessorMenuPlayPreview.get(),
    function execute( target_entry, entries ) {
      if ( !(target_entry.is_file) ) {
        return;
      }
      target_entry.parse_as_bms_data_once();
      local key = "PREVIEW";
      if ( key in target_entry.bms_data.headers ) {
        local path = Path.dirname( target_entry.path ) + "\\" + target_entry.bms_data.headers[key];
        BMX2WAV.play_wav_or_ogg_file_async( path, 50 );
      }
    }
  };

  local shell_execute = {
    name = StrT.Searcher.Main.PopupEntry.ProcessorMenuOpen.get(),
    function execute( target_entry, entries ) {
      BMX2WAV.shell_execute( "open", target_entry.path, null, Path.dirname( target_entry.path ) );
    }
  };

  local notepad = {
    name = StrT.Searcher.Main.PopupEntry.ProcessorMenuOpenByNotepad.get(),
    function execute( target_entry, entries ) {
      if ( !(target_entry.is_file) ) {
        return;
      }
      BMX2WAV.create_process( "notepad.exe", target_entry.path, null, BMX2WAV.ShowState.SHOW, false );
    }
  };

  Searcher.entry_processors.items.append( preview );
  local sub = ::SubMenu( StrT.Searcher.Main.PopupEntry.ProcessorMenuFileOperations.get(), [shell_execute, notepad] );
  Searcher.entry_processors.items.append( sub );
}
