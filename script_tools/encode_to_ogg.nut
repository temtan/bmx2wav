
// wav �t�@�C�� 1 �� ogg �t�@�C���ɃG���R�[�h���܂��B

local message = "wav �t�@�C���� ogg �t�@�C���ɕϊ����܂��B\nwav �t�@�C����I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "wav �t�@�C��(*.wav)", ["*.wav"] ) );
dialog.filters.append( FileDialog.Filter( "�S�Ẵt�@�C��(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output = Path.get_file_not_exist_path_from( Path.change_extension( input, "ogg" ) );

  puts( "input = " + input );
  puts( "output = " + output );
  puts( "�ϊ��J�n" );

  local w = Wave( input );
  w.write_to_file_as_ogg( output, 0.5 );

  puts( "�ϊ��I��" );

  MessageBox.Ok( "wav �t�@�C������ ogg �t�@�C���ւ̕ϊ����I�����܂����B�o�̓t�@�C�� : " + output, "�ϊ��I��", MessageBox.Icon.Information );
}
