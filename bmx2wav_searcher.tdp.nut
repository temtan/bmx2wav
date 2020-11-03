// bmx2wav_searcher.tdp.nut

{
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav_searcher.nut" );
  tmp.included_in_build = true;
  tmp.depends.append( "bmx2wav_searcher.nut" );
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
  local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\bmx2wav_searcher.class.nut" );
  tmp.included_in_build = true;
  tmp.depends.append( "bmx2wav_searcher.class.nut" );
  tmp.commands.append( "copy /Y $? $@" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
