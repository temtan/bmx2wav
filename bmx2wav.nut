
// ----- ColumnGroup 関連 ------------------------------------------------
// null の場合はフルパスを表示する
Main.first_column = null;
// 以下と同じ
// Main.first_column = FullPathColumn();

// カラムグループ設定
{
  local group = ColumnGroup( "簡易" );
  group.columns.append( ParameterColumn() );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  Main.column_groups.append( group );
}

{
  local group = ColumnGroup( "詳細" );
  group.columns.append( ParameterColumn() );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "GENRE", HeaderColumn.compare_as_string ) );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  group.columns.append( HeaderColumn( "BPM", HeaderColumn.compare_as_integer_asc ) );
  group.columns.append( MostSeriousErrorColumn() );
  group.columns.append( PlayingTimeColumn() );
  Main.column_groups.append( group );
}

{
  local group = ColumnGroup( "他色々" );
  group.columns.append( ParameterColumn() );
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
  Main.column_groups.append( group );
}

// -- エントリー詳細ダイアログリスト表記設定 -----------------------------
Main.entry_dialog_list_items.append( HeaderColumn( "TITLE" ) );
Main.entry_dialog_list_items.append( HeaderColumn( "ARTIST" ) );
Main.entry_dialog_list_items.append( HeaderColumn( "GENRE" ) );
Main.entry_dialog_list_items.append( PlayerColumn() );
Main.entry_dialog_list_items.append( HeaderColumn( "PLAYLEVEL" ) );
Main.entry_dialog_list_items.append( HeaderColumn( "BPM" ) );
Main.entry_dialog_list_items.append( MostSeriousErrorColumn() );
Main.entry_dialog_list_items.append( PlayingTimeColumn() );
Main.entry_dialog_list_items.append( ContainsRandomStatementColumn() );


// -- EntryProcessors 関連 -----------------------------------------------
{
  local preview = {
    name = "プレビュー再生",
    function execute( entry ) {
      entry.parse_as_bms_data_once();
      local key = "PREVIEW";
      if ( key in entry.bms_data.headers ) {
        local path = Path.dirname( entry.path ) + "\\" + entry.bms_data.headers[key];
        BMX2WAV.play_wav_or_ogg_file_async( path, 50 );
      }
    }
  };

  local shell_execute = {
    name = "開く",
    function execute( entry ) {
      BMX2WAV.shell_execute( "open", entry.path, null, Path.dirname( entry.path ) );
    }
  };

  local notepad = {
    name = "メモ帳で開く",
    function execute( entry ) {
      BMX2WAV.create_process( "notepad.exe", entry.path, null, BMX2WAV.ShowState.SHOW, false );
    }
  };

  Main.entry_processors.items.append( preview );
  local sub = SubMenu( "ファイル操作", [shell_execute, notepad] );
  Main.entry_processors.items.append( sub );
}
