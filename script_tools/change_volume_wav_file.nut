
// WAV ファイルの音量を変更して保存します。

local message = "wav ファイルの音量を変更します。\nwav ファイルを選択して下さい。";
MessageBox.Ok( message, "説明", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "wav ファイル(*.wav)", ["*.wav"] ) );
dialog.filters.append( FileDialog.Filter( "全てのファイル(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output_dir = Path.dirname( input ) + "\\volume_changed";

  mkdir( output_dir );

  local user_dialog = UserInputDialogNumber();
  user_dialog.title = "音量入力";
  user_dialog.explanation = "調整する音量の割合を入力して下さい(%)";
  user_dialog.show_dialog();
  local volume = user_dialog.input.tofloat() / 100.0;
  puts( "volume : " + volume + " %" );

  local output = output_dir + "\\" + Path.basename( input );
  output = Path.get_file_not_exist_path_from( output );

  puts( "input = " + input );
  puts( "output = " + output );
  print( "start ... " );

  local w = Wave( input );
  w.change_volume( volume );
  w.write_to_file( output );

  puts( "end" );

  MessageBox.Ok( "wav ファイルの音量調整が終了しました。出力ファイル : " + output, "調整終了", MessageBox.Icon.Information );
}
