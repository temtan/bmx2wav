
// ogg ファイル 1 つを wav ファイルにデコードします。

local message = "ogg ファイルを wav ファイルに変換します。\nogg ファイルを選択して下さい。";
MessageBox.Ok( message, "説明", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "ogg ファイル(*.ogg)", ["*.ogg"] ) );
dialog.filters.append( FileDialog.Filter( "全てのファイル(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output = Path.get_file_not_exist_path_from( Path.change_extension( input, "wav" ) );

  puts( "input = " + input );
  puts( "output = " + output );
  puts( "変換開始" );

  local w = Wave( input );
  w.write_to_file( output );

  puts( "変換終了" );

  MessageBox.Ok( "ogg ファイルから wav ファイルへの変換が終了しました。出力ファイル : " + output, "変換終了", MessageBox.Icon.Information );
}
