
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
