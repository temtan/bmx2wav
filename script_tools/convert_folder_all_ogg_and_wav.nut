
// �w�肵���t�H���_�z���ɂ���S�Ă� ogg �t�@�C���܂��� wav �t�@�C����
// wav �t�@�C���܂��� ogg �t�@�C���ɕϊ����܂��B
// ���Ƀt�@�C��������ꍇ�͕ϊ����܂���B

local message = "�t�H���_���� wav �t�@�C���� ogg �t�@�C���ɁAogg �t�@�C���� wav �t�@�C���ɕϊ����܂��B\nwav �t�H���_��I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

local dialog = FolderBrowserDialog();

if ( dialog.show_dialog() ) {
  puts( "target = " + dialog.selected_path );
  local files = BMX2WAV.get_directory_entries( dialog.selected_path );
  foreach ( file in files ) {
    if ( Path.has_extension( file, "wav" ) || Path.has_extension( file, "ogg" ) ) {
      local wav_file = Path.change_extension( file, "wav" );
      local ogg_file = Path.change_extension( file "ogg" );
      if ( Path.file_exist( wav_file ) && Path.file_exist( ogg_file ) ) {
        continue;
      }
      puts( "reading " + file );
      local w = Wave( file );
      if ( ! Path.file_exist( wav_file ) ) {
        w.write_to_file( wav_file );
        puts( "output " + wav_file );
      }
      if ( ! Path.file_exist( ogg_file ) ) {
        w.write_to_file_as_ogg( ogg_file, 0.5 );
        puts( "output " + ogg_file );
      }
    }
  }
  puts( "completed." );
}
