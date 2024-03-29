// bmx2wav.class.nut

// -- 出力テンプレート関係 -----------------------------------------------
Main.translate_template_path_by_parameter <- function ( parameter, bms_data ) {
  return Main.translate_template_path( parameter.output_file_template,
                                       parameter.input_file_path,
                                       parameter.output_as_ogg,
                                       bms_data );
}

Main.Converter.get_translated_template_path <- function () {
  return Main.translate_template_path_by_parameter( this.parameter, this.bms_data );
}

// ----- 変換中コールバック関数群 ----------------------------------------
// -- tools
Main.callback_current_end_string <- null;
Main.puts_with_end_string <- function ( str ) {
  if ( Main.callback_current_end_string ) {
    str += ": " + Main.callback_current_end_string;
  }
  ::puts( str );
}


// abort
Main.callback_table.aborted = function ( converter, user_abort ) {
  if ( user_abort ) {
    ::puts( StrT.Message.Converter.RequestToAbortByUser.get() );
  }
  ::puts( StrT.Message.Converter.Aborted.get() );
}

// exception
Main.callback_table.exception_occurred = function ( converter, exception ) {
  local tmp = ErrorLevel.error_level_to_string( exception.get_error_level() );
  ::puts( tmp + " : " + exception.message );

  if ( exception.get_error_level() <= ErrorLevel.Tiny ) {
    Main.callback_current_end_string = StrT.Message.Converter.ExceptionOccurredErrorExist.get();
  }
}

Main.callback_table.parser_exception_occurred = function ( converter, exception ) {
  local tmp = converter.parameter.input_file_path;
  tmp = tmp + "(" + exception.line + "): ";
  tmp = tmp + ErrorLevel.error_level_to_string( exception.get_error_level() );
  ::puts( tmp + " : " + exception.message );

  if ( exception.get_error_level() <= ErrorLevel.Tiny ) {
    Main.callback_current_end_string = StrT.Message.Converter.ExceptionOccurredErrorExist.get();
  }
}

// progress
Main.callback_table.before_initialize = function ( converter ) {
  ::puts( StrT.Message.Converter.ConvertStart.get() );
  ::puts( "BMS File Path : " + converter.parameter.input_file_path );
  ::puts( StrT.Message.Converter.InitializeStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.after_initialize = function ( converter ) {
  Main.puts_with_end_string( StrT.Message.Converter.InitializeEnd.get() );
}

Main.callback_table.before_parse = function ( converter ) {
  ::puts( StrT.Message.Converter.ParseStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.decide_output_file_path = function ( converter ) {
  local tmp = converter.get_translated_template_path();
  if ( converter.parameter.remove_can_not_use_character_as_file_path ) {
    tmp = Path.remove_can_not_use_character_as_file_path_from( tmp );
  }
  if ( converter.parameter.never_overwrite_output_file ) {
    tmp = Path.get_file_not_exist_path_from( tmp );
  }
  return tmp;
}

Main.callback_table.after_parse = function ( converter ) {
  Main.puts_with_end_string( StrT.Message.Converter.ParseEnd.get() );
  ::puts( ::format( StrT.Message.Converter.MaxResolutionAndBarNumber.get(),
                    converter.bms_data.max_resolution,
                    converter.bms_data.bar_number_of_max_resolution ) );
}

Main.callback_table.before_read_audio_files = function ( converter ) {
  ::puts( StrT.Message.Converter.ReadAudioFilesStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.decide_audio_file_path = function ( converter, filename ) {
  local base_path = Path.dirname( converter.parameter.input_file_path ) + "\\" + filename;
  local wav_path = Path.change_extension( base_path, "wav" );
  local ogg_path = Path.change_extension( base_path, "ogg" );

  if ( Path.file_exist( wav_path ) ) {
    return wav_path;
  }
  else if ( Path.file_exist( ogg_path ) ) {
    return ogg_path;
  }
  return base_path;
}

Main.callback_table.audio_file_read_start = function( converter, word, path ) {
  ::print( "WAV" + BMX2WAV.integer_to_word_string( word ) + " read : " + path + "  start ... " );
}

Main.callback_table.audio_file_read_end = function( converter, word, path ) {
  ::puts( "end" );
}

Main.callback_table.after_read_audio_files = function ( converter ) {
  Main.puts_with_end_string( StrT.Message.Converter.ReadAudioFilesEnd.get() );
}

Main.callback_table.before_mixin_waves = function ( converter ) {
  ::puts( StrT.Message.Converter.MixinWavesStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.after_mixin_waves = function ( converter ) {
  Main.puts_with_end_string( StrT.Message.Converter.MixinWavesEnd.get() );
}

Main.callback_table.before_affect_wave = function ( converter ) {
  ::puts( StrT.Message.Converter.AffectWaveStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.after_affect_wave = function ( converter ) {
  Main.puts_with_end_string( StrT.Message.Converter.AffectWaveEnd.get() );
}

Main.callback_table.complete_normalize = function ( converter, ratio ) {
  local tmp = NormalizeKind.normalize_kind_to_string( converter.parameter.normalize_kind );
  if ( converter.parameter.normalize_kind == NormalizeKind.Over || converter.parameter.normalize_kind == NormalizeKind.Default ) {
    tmp += " : OverPPM : " + converter.parameter.normalize_over_ppm;
  }
  ::puts( StrT.Message.Converter.NormalizeKind.get() + tmp );
  ::puts( StrT.Message.Converter.NormalizeRatio.get() + ratio );
}

Main.callback_table.before_output_to_file = function ( converter ) {
  ::puts( StrT.Message.Converter.OutputToFileStart.get() );
  Main.callback_current_end_string = null;
}

Main.callback_table.after_output_to_file = function ( converter ) {
  if ( converter.output_file_path == null || converter.output_file_path == "" ) {
    ::puts( StrT.Message.Converter.OutputToFileNoOutputNotice.get() );
    Main.puts_with_end_string( StrT.Message.Converter.OutputToFileNoOutputEnd.get() );
  }
  else {
    ::puts( StrT.Message.Converter.OutputFile.get() + converter.output_file_path );
    Main.puts_with_end_string( StrT.Message.Converter.OutputToFileEnd.get() );
  }

  ::puts( ::format( StrT.Message.Converter.ProcessingTime.get(), converter.get_processing_time() ) );
}

Main.callback_table.after_process = function ( converter ) {
  if ( converter.parameter.execute_after_process ) {
    BMX2WAV.create_process(
      converter.parameter.after_process_execute_file,
      BMX2WAV.replace_string( converter.parameter.after_process_execute_arguments, "@@output_file_path@@", converter.output_file_path ),
      Path.dirname( converter.parameter.after_process_execute_file ),
      converter.parameter.after_process_start_on_background ? BMX2WAV.ShowState.SHOW_MINIMIZED_NO_ACTIVE : BMX2WAV.ShowState.SHOW,
      converter.parameter.after_process_wait_for_process_exit );
  }
  if ( converter.parameter.after_process_delete_output_file ) {
    // remove() is squirrel system function
    ::remove( converter.output_file_path );
  }
}

Main.callback_table.before_finalize = function ( converter ) {}
Main.callback_table.after_finalize = function ( converter ) {}


// ----- ColumnGroup 関連 ------------------------------------------------
// -- ParameterColumn
class ParameterColumn extends Column {
  constructor() {
    base.constructor( StrT.Main.ColumnName.Parameter.get() );
  }

  function display_cell( entry ) {
    local parameter = entry.get_individual_parameter();

    if ( parameter ) {
      return parameter.name + StrT.Main.ColumnName.ParameterIndividual.get();
    }
    else {
      return Main.get_common_parameter().name + StrT.Main.ColumnName.ParameterCommon.get();
    }
  }

  function compare( x, y ) {
    return ( x.get_individual_parameter() == null ) <=> ( y.get_individual_parameter() == null );
  }
}

// null の場合はフルパスを表示する
Main.first_column = null;
// 以下と同じ
// Main.first_column = FullPathColumn();

// カラムグループ設定
{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Simple.get() );
  group.columns.append( ParameterColumn() );
  group.columns.append( HeaderColumn( "TITLE", HeaderColumn.compare_as_string ) );
  group.columns.append( HeaderColumn( "ARTIST", HeaderColumn.compare_as_string ) );
  group.columns.append( PlayerColumn() );
  group.columns.append( HeaderColumn( "PLAYLEVEL", HeaderColumn.compare_as_integer_desc ) );
  Main.column_groups.append( group );
}

{
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Details.get() );
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
  local group = ColumnGroup( StrT.Main.ColumnGroupName.Various.get() );
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
    name = StrT.Main.PopupEntry.ProcessorMenuPlayPreview.get(),
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
    name = StrT.Main.PopupEntry.ProcessorMenuOpen.get(),
    function execute( entry ) {
      BMX2WAV.shell_execute( "open", entry.path, null, Path.dirname( entry.path ) );
    }
  };

  local notepad = {
    name = StrT.Main.PopupEntry.ProcessorMenuOpenByNotepad.get(),
    function execute( entry ) {
      BMX2WAV.create_process( "notepad.exe", entry.path, null, BMX2WAV.ShowState.SHOW, false );
    }
  };

  Main.entry_processors.items.append( preview );
  local sub = SubMenu( StrT.Main.PopupEntry.ProcessorMenuFileOperations.get(), [shell_execute, notepad] );
  Main.entry_processors.items.append( sub );
}
