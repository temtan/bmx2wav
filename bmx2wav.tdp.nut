// bmx2wav.tdp.nut

{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\japanese.res" );
  tmp.included_in_build = true;
  tmp.depends.append( "lang\\japanese.rc" );
  tmp.commands.append( "$(RC) /nologo -Fo$@ $?" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\english.res" );
  tmp.included_in_build = true;
  tmp.depends.append( "lang\\english.rc" );
  tmp.commands.append( "$(RC) /nologo -Fo$@ $?" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\lang\\japanese.dll" );
  tmp.included_in_build = true;
  tmp.depends.append( Developer.current_structure.output_directory_name + "\\japanese.res" );
  tmp.commands.append( "if not exist " + Developer.current_structure.output_directory_name + "\\lang mkdir " + Developer.current_structure.output_directory_name + "\\lang" );
  tmp.commands.append( "$(LINK) /DLL /NOENTRY /MACHINE:X64 $? /OUT:$@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\lang\\english.dll" );
  tmp.included_in_build = true;
  tmp.depends.append( Developer.current_structure.output_directory_name + "\\english.res" );
  tmp.commands.append( "if not exist " + Developer.current_structure.output_directory_name + "\\lang mkdir " + Developer.current_structure.output_directory_name + "\\lang" );
  tmp.commands.append( "$(LINK) /DLL /NOENTRY /MACHINE:X64 $? /OUT:$@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
{
  local tmp = MakeCommandEntry( "lang_clean" );
  tmp.included_in_clean = true;
  tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\japanese.res" );
  tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\lang\\japanese.dll" );
  tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\english.res" );
  tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\lang\\english.dll" );
  Developer.current_structure.additional_make_commands.append( tmp );
}

/*
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
*/
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

