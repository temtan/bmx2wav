// bmx2wav.tdp.nut

{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav.nut" );
  if ( Developer.current_structure.name == "Debug_32" ) {
    tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav_32bit.nut" );
  }
  tmp.included_in_build = true;
  tmp.depends.append( "bmx2wav.nut" );
  tmp.commands.append( "copy /Y $? $@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav.base.nut" );
  tmp.included_in_build = true;
  tmp.depends.append( "bmx2wav.base.nut" );
  tmp.commands.append( "copy /Y $? $@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav.class.nut" );
  tmp.included_in_build = true;
  tmp.depends.append( "bmx2wav.class.nut" );
  tmp.commands.append( "copy /Y $? $@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
