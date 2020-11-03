
// -- Path テーブル ------------------------------------------------------
Path.has_extension <- function( str, extension ) {
  if ( ::endswith( str.tolower(), "." + extension.tolower() ) ) {
    return true;
  }
  return false;
}

// ----- BMX2WAV テーブル ------------------------------------------------
BMX2WAV.replace_string <- function ( str, pattern, replacement )
{
  local result = "";
  local start = 0;
  local ex = regexp( pattern );
  while ( true ) {
    local res = ex.search( str, start );
    if ( res ) {
      result += str.slice( start, res.begin ) + replacement;
      start = res.end;
    }
    else {
      result += str.slice( start );
      break;
    }
  }
  return result;
}


BMX2WAV.play_wav_or_ogg_file_async <- function( path, volume ) {
  if ( Path.has_extension( path, "wav" ) ) {
    BMX2WAV.play_wav_file_async( path, volume );
  }
  if ( Path.has_extension( path, "ogg" ) ) {
    BMX2WAV.play_ogg_file_async( path, volume );
  }
}


// ----- ColumnGroup 関連 ------------------------------------------------
function safety_get( table, key, default_value ) {
  return ( key in table ) ? table[key] : default_value;
}

// -- Column
class Column {
  constructor( name ) {
    this.name = name;
  }

  function display_cell( entry ) {
    return "-";
  }

  function compare( entry_x, entry_y ) {
    return entry.getclass().compare( entry_x, entry_y );
  }

  function compare_bms_data_must_exist( entry_x, entry_y ) {
    if ( entry_x.bms_data == null ) {
      return entry_y.bms_data == null ? 0 : 1;
    }
    if ( entry_y.bms_data == null ) {
      return -1
    }
    return this.compare_bms_data_checked( entry_x, entry_y );
  }

  function compare_bms_data_checked( entry_x, entry_y ) {
    // 継承クラスでのオーバーライド用
    return 0;
  }

  function string_compare( str_x, str_y ) {
    if ( str_x == "" ) {
      return str_y == "" ? 0 : 1;
    }
    if ( str_y == "" ) {
      return -1;
    }
    return str_x <=> str_y;
  }

  name = null;
};

// -- HeaderColumn
class HeaderColumn extends Column {
  constructor( header_name, ... ) {
    base.constructor( "#" + header_name );
    this.header_name = header_name;
    this.compare_function = HeaderColumn.compare_as_string;
    if ( vargv.len() > 0 ) {
      this.compare_function = vargv[0];
    }
  }

  function display_cell( entry ) {
    return ::safety_get( entry.bms_data.headers, this.header_name, "-" );
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return this.compare_function( x, y );
  }

  function compare_as_integer_asc( x, y ) {
    local x_value = ::safety_get( x.bms_data.headers, this.header_name, "" );
    local y_value = ::safety_get( y.bms_data.headers, this.header_name, "" );
    if ( x_value == "" ) {
      return y_value == "" ? 0 : 1;
    }
    if ( y_value == "" ) {
      return -1;
    }
    return x_value.tointeger() <=> y_value.tointeger();
  }

  function compare_as_integer_desc( x, y ) {
    local x_value = ::safety_get( x.bms_data.headers, this.header_name, "" );
    local y_value = ::safety_get( y.bms_data.headers, this.header_name, "" );
    if ( x_value == "" ) {
      return y_value == "" ? 0 : 1;
    }
    if ( y_value == "" ) {
      return -1;
    }
    return y_value.tointeger() <=> x_value.tointeger();
  }

  function compare_as_string( x, y ) {
    local x_value = ::safety_get( x.bms_data.headers, this.header_name, "" );
    local y_value = ::safety_get( y.bms_data.headers, this.header_name, "" );
    return this.string_compare( x_value, y_value );
  }

  header_name = null;
  compare_function = null;
}

// -- PlayerColumn
class PlayerColumn extends HeaderColumn {
  constructor() {
    base.constructor( "PLAYER", HeaderColumn.compare_as_integer_asc );
  }

  function display_cell( entry ) {
    local tmp = base.display_cell( entry );
    switch ( tmp ) {
    case "1": return "1P";
    case "2": return "2P";
    case "3": return "DP";
    case "4": return "Battle Play";
    default:  return tmp;
    }
  }
}

// -- ObjectCountColumn
class ObjectCountColumn extends Column {
  constructor() {
    base.constructor( "オブジェクト数" );
  }

  function display_cell( entry ) {
    return entry.bms_data.object_count;
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return y.bms_data.object_count <=> x.bms_data.object_count;
  }
}

// -- ObjectOf1PCountColumn
class ObjectOf1PCountColumn extends Column {
  constructor() {
    base.constructor( "1Pオブジェクト数" );
  }

  function display_cell( entry ) {
    return entry.bms_data.object_count_of_1P;
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return y.bms_data.object_count_of_1P <=> x.bms_data.object_count_of_1P;
  }
}

// -- ObjectOf2PCountColumn
class ObjectOf2PCountColumn extends Column {
  constructor() {
    base.constructor( "2Pオブジェクト数" );
  }

  function display_cell( entry ) {
    return entry.bms_data.object_count_of_2P;
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return y.bms_data.object_count_of_2P <=> x.bms_data.object_count_of_2P;
  }
}

// -- MostSeriousErrorColumn
class MostSeriousErrorColumn extends Column {
  constructor() {
    base.constructor( "最重エラー" )
  }

  function display_cell( entry ) {
    return ErrorLevel.error_level_to_string( entry.bms_data.most_serious_error_level );
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return y.bms_data.most_serious_error_level <=> x.bms_data.most_serious_error_level;
  }
}

// -- ContainsRandomStatementColumn
class ContainsRandomStatementColumn extends Column {
  constructor() {
    base.constructor( "ランダム構文" );
  }

  function display_cell( entry ) {
    return entry.bms_data.has_random_statement ? "有" : "無";
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return (y.bms_data.has_random_statement ? 1 : 0) <=> (x.bms_data.has_random_statement ? 1 : 0);
  }
}

// -- FullPathColumn
class FullPathColumn extends Column {
  constructor() {
    base.constructor( "ファイルパス" );
  }

  function display_cell( entry ) {
    return entry.path;
  }

  function compare( x, y ) {
    return this.string_compare( x.path, y.path );
  }
}

// -- BasenameColumn
class BasenameColumn extends Column {
  constructor() {
    base.constructor( "ファイル名" );
  }

  function display_cell( entry ) {
    return Path.basename( entry.path );
  }

  function compare( x, y ) {
    return this.string_compare( Path.basename( x.path ), Path.basename( y.path ) );
  }
}

// -- ParentDirectoryColumn
class ParentDirectoryColumn extends Column {
  constructor() {
    base.constructor( "フォルダ" );
  }

  function get_parent_directory( entry ) {
    return Path.basename( Path.dirname( entry.path ) );
  }

  function display_cell( entry ) {
    return this.get_parent_directory( entry );
  }

  function compare( x, y ) {
    return this.string_compare( this.get_parent_directory( x ), this.get_parent_directory( y ) );
  }
}

// -- ParentsParentDirectoryColumn
class ParentsParentDirectoryColumn extends Column {
  constructor() {
    base.constructor( "親フォルダ" );
  }

  function get_parents_parent_directory( entry ) {
    return Path.basename( Path.dirname( Path.dirname( entry.path ) ) );
  }

  function display_cell( entry ) {
    return this.get_parents_parent_directory( entry );
  }

  function compare( x, y ) {
    return this.string_compare( this.get_parents_parent_directory( x ), this.get_parents_parent_directory( y ) );
  }
}

// -- PlayingTimeColumn
class PlayingTimeColumn extends Column {
  constructor() {
    base.constructor( "演奏時間" );
  }

  function display_cell( entry ) {
    local tmp = entry.bms_data.calculate_playing_time();
    if ( tmp == null ) {
      return "Error";
    }
    local minute = (tmp / 60.0).tointeger();
    local second = (tmp - 60.0 * minute.tofloat()).tointeger();
    return format( "%d:%02d", minute, second );
  }

  function compare( x, y ) {
    return this.compare_bms_data_must_exist( x, y );
  }

  function compare_bms_data_checked( x, y ) {
    return x.bms_data.calculate_playing_time() <=> y.bms_data.calculate_playing_time();
  }
}


// -- ColumnGroup
class ColumnGroup {
  constructor( name ) {
    this.name = name;
    this.columns = [];
  }

  name = null;
  columns = null;
};
