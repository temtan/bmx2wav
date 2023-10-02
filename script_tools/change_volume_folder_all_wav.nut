
// フォルダの中の全ての WAV ファイルの音量を変更して保存します。

local message = "フォルダ内の全ての wav ファイルの音量を変更します。\nフォルダを選択して下さい。";
MessageBox.Ok( message, "説明", MessageBox.Icon.Information );

local dialog = FolderBrowserDialog();

if ( dialog.show_dialog() ) {
  local input_dir = dialog.selected_path;
  local output_dir = input_dir + "\\volume_changed";

  mkdir( output_dir );

  local user_dialog = UserInputDialogNumber();
  user_dialog.title = "音量入力";
  user_dialog.explanation = "調整する音量の割合を入力して下さい(%)";
  user_dialog.show_dialog();
  local volume = user_dialog.input.tofloat() / 100.0;
  puts( "volume : " + volume * 100.0 + " %" );

  local one_file_process = function ( input ) {
    local output = output_dir + "\\" + Path.basename( input );
    output = Path.get_file_not_exist_path_from( output );

    puts( "input = " + input );
    puts( "output = " + output );
    print( "start ... " );

    local w = Wave( input );
    w.change_volume( volume );
    w.write_to_file( output );

    puts( "end" );
  };

  foreach ( file in BMX2WAV.get_directory_entries( input_dir ) ) {
    if ( Path.has_extension( file, "wav" ) ) {
      one_file_process( file );
    }
  }

  MessageBox.Ok( "wav ファイルの音量調整が終了しました。出力フォルダ : " + output_dir, "調整終了", MessageBox.Icon.Information );
}
