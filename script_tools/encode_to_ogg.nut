
// wav ファイル 1 つを ogg ファイルにエンコードします。

local message = "wav ファイルを ogg ファイルに変換します。\nwav ファイルを選択して下さい。";
MessageBox.Ok( message, "説明", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "wav ファイル(*.wav)", ["*.wav"] ) );
dialog.filters.append( FileDialog.Filter( "全てのファイル(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output = Path.get_file_not_exist_path_from( Path.change_extension( input, "ogg" ) );

  puts( "input = " + input );
  puts( "output = " + output );
  puts( "変換開始" );

  local w = Wave( input );
  w.write_to_file_as_ogg( output, 0.5 );

  puts( "変換終了" );

  MessageBox.Ok( "wav ファイルから ogg ファイルへの変換が終了しました。出力ファイル : " + output, "変換終了", MessageBox.Icon.Information );
}
