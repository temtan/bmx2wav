// libogg.tdp.nut

Developer.current_structure.build_functions.after_build = function ( exit_code )
{
  if ( exit_code == 0 ) {
    local str = "copy /Y \"";
    str += Developer.output_directory_path + "\\" + Developer.current_structure.target_name;
    str += "\" \"";
    local lib_folder = Developer.current_structure.name == "Release_x64" ? "lib\\x64" : "lib\\x86";
    str += Developer.project_directory_path + "\\" + lib_folder + "\\" + Developer.current_structure.target_name;
    str += "\"\n";

    print( "\n" );
    print( str );
    Developer.system( str );
  }
}
