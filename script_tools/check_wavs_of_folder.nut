
// �t�H���_�̒��� WAV �̏󋵂��`�F�b�N���܂��B
local message = "�t�H���_���̉����t�@�C���̈ȉ��̏󋵂��`�F�b�N���܂��B\n�Ewav�t�@�C�������邩�ǂ����B\n�Eogg�t�@�C�������邩�ǂ����B\n�EBMS���Ŏw�肳��Ă��邩�ǂ����B\n\n�t�H���_��I�����ĉ������B";
MessageBox.Ok( message, "����", MessageBox.Icon.Information );

class Entry {
  constructor( name ) {
    this.name = name;
  };

  name = null;
  wav = false;
  ogg = false;
  use = false;
};

/*
local dialog = FolderBrowserDialog();

if ( dialog.show_dialog() ) {
  local input_dir = dialog.selected_path;
 */
if ( true ) {
  local user_dialog = UserInputDialogNumber();
  user_dialog.title = "�t�H���_�w��";
  user_dialog.explanation = "�t�H���_��D&D����̂�";
  user_dialog.show_dialog();
  local input_dir = user_dialog.input;

  local table = {};
  local if_not_in_table_new_slot = function( name ) {
    if ( !(name in table) ) {
      table[name] <- Entry( name );
    }
  };

  foreach ( file in BMX2WAV.get_directory_entries( input_dir ) ) {
    local name = Path.remove_extension( Path.basename( file ) );

    if ( Path.has_extension( file, "wav" ) ) {
      if_not_in_table_new_slot( name );
      table[name].wav = true;
    }
    if ( Path.has_extension( file, "ogg" ) ) {
      if_not_in_table_new_slot( name );
      table[name].ogg = true;
    }
    if ( Path.has_extension( file, "bms" ) || Path.has_extension( file, "bme" ) ) {
      local parser = Parser();
      local bms_data = parser.parse( file );

      foreach ( wav in bms_data.wav_array ) {
        if ( wav ) {
          local wav_name = Path.remove_extension( wav );
          if_not_in_table_new_slot( wav_name );
          table[wav_name].use = true;
        }
      }
    }
  }

  local max_length = 4;
  foreach ( entry in table ) {
    if ( entry.name.len() > max_length ) {
      max_length = entry.name.len();
    }
  }
  if ( true ) {
    local str = "";
    for ( local i = 4; i < max_length; i += 1 ) {
      str += " ";
    }
    str += "name : WAV : OGG : BMS";
    puts( str )
  }
  foreach ( entry in table ) {
    local str = "";
    for ( local i = entry.name.len(); i < max_length; i += 1 ) {
      str += " ";
    }
    str += entry.name;
    puts( str + " :  " + (entry.wav ? "OK" : "--") + " :  " + (entry.ogg ? "OK" : "--") + " :  " + (entry.use ? "OK" : "--" ) );
  }
}
