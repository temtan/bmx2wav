
// WAV �t�@�C���̉��ʂ�ύX���ĕۑ����܂��B

local message = "wav �t�@�C���̉��ʂ�ύX���܂��B\nwav �t�@�C����I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

local dialog = OpenFileDialog();
dialog.filters.append( FileDialog.Filter( "wav �t�@�C��(*.wav)", ["*.wav"] ) );
dialog.filters.append( FileDialog.Filter( "�S�Ẵt�@�C��(*.*)", "*.*" ) );

if ( dialog.show_dialog() ) {
  local input = dialog.file_name;
  local output_dir = Path.dirname( input ) + "\\volume_changed";

  mkdir( output_dir );

  local user_dialog = UserInputDialogNumber();
  user_dialog.title = "���ʓ���";
  user_dialog.explanation = "�������鉹�ʂ̊�������͂��ĉ�����(%)";
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

  MessageBox.Ok( "wav �t�@�C���̉��ʒ������I�����܂����B�o�̓t�@�C�� : " + output, "�����I��", MessageBox.Icon.Information );
}
