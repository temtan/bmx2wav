
// �t�H���_�̒��̑S�Ă� WAV �t�@�C���̉��ʂ�ύX���ĕۑ����܂��B

local message = "�t�H���_���̑S�Ă� wav �t�@�C���̉��ʂ�ύX���܂��B\n�t�H���_��I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

local dialog = FolderBrowserDialog();

if ( dialog.show_dialog() ) {
  local input_dir = dialog.selected_path;
  local output_dir = input_dir + "\\volume_changed";

  mkdir( output_dir );

  local user_dialog = UserInputDialogNumber();
  user_dialog.title = "���ʓ���";
  user_dialog.explanation = "�������鉹�ʂ̊�������͂��ĉ�����(%)";
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

  MessageBox.Ok( "wav �t�@�C���̉��ʒ������I�����܂����B�o�̓t�H���_ : " + output_dir, "�����I��", MessageBox.Icon.Information );
}
