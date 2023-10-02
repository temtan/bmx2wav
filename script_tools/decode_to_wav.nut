
// ogg �t�@�C�� 1 �� wav �t�@�C���Ƀf�R�[�h���܂��B

local message = "ogg �t�@�C���� wav �t�@�C���ɕϊ����܂��B\nogg �t�@�C����I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "ogg �t�@�C��(*.ogg)", ["*.ogg"] ) );
dialog.filters.append( FileDialog.Filter( "�S�Ẵt�@�C��(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output = Path.get_file_not_exist_path_from( Path.change_extension( input, "wav" ) );

  puts( "input = " + input );
  puts( "output = " + output );
  puts( "�ϊ��J�n" );

  local w = Wave( input );
  w.write_to_file( output );

  puts( "�ϊ��I��" );

  MessageBox.Ok( "ogg �t�@�C������ wav �t�@�C���ւ̕ϊ����I�����܂����B�o�̓t�@�C�� : " + output, "�ϊ��I��", MessageBox.Icon.Information );
}
