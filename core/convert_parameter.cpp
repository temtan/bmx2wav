// core/convert_parameter.cpp

#include "tt_ini_file.h"
#include "tt_clipboard.h"
#include "tt_text_template.h"

#include "core/convert_parameter.h"

using namespace BMX2WAV;


// -- Normalize ----------------------------------------------------------
template <>
TtEnumTable<Core::Parameter::Normalize>::TtEnumTable( void ) {
#define REGIST( NAME ) this->Register( Core::Parameter::Normalize::NAME, #NAME )
  REGIST( Default );
  REGIST( None );
  REGIST( Peak );
  REGIST( Average );
  REGIST( Over );
#undef  REGIST
};

template <>
TtEnumTable<Core::Parameter::Normalize>
TtEnumBase<Core::Parameter::Normalize>::TABLE;

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( ConvertParameter );

  DEFINE_PARAMETER_NAME_STRING( OutputAsOgg );
  DEFINE_PARAMETER_NAME_STRING( IgnoreBgaChannel );
  DEFINE_PARAMETER_NAME_STRING( NotNestingIfStatement );
  DEFINE_PARAMETER_NAME_STRING( CancelAtResounding );
  DEFINE_PARAMETER_NAME_STRING( ConvertStartBarNumber );
  DEFINE_PARAMETER_NAME_STRING( ConvertEndBarNumber );
  DEFINE_PARAMETER_NAME_STRING( DoTriming );
  DEFINE_PARAMETER_NAME_STRING( TrimingStartBarNumber );
  DEFINE_PARAMETER_NAME_STRING( TrimingEndBarNumber );
  DEFINE_PARAMETER_NAME_STRING( NormalizeKind );
  DEFINE_PARAMETER_NAME_STRING( NormalizeOverPPM );
  DEFINE_PARAMETER_NAME_STRING( Volume );
  DEFINE_PARAMETER_NAME_STRING( OggBaseQuality );
  DEFINE_PARAMETER_NAME_STRING( RemoveFrontSilence );
  DEFINE_PARAMETER_NAME_STRING( InsertFrontSilence );
  DEFINE_PARAMETER_NAME_STRING( InsertFrontSilenceSecond );
  DEFINE_PARAMETER_NAME_STRING( ImmediatelyAbortErrorLevel );
  DEFINE_PARAMETER_NAME_STRING( DelayAbortErrorLevel );
  DEFINE_PARAMETER_NAME_STRING( BarResolutionMax );
  DEFINE_PARAMETER_NAME_STRING( UsableMemoryMegaByteSize );
  DEFINE_PARAMETER_NAME_STRING( ExecuteAfterProcess );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessExecuteFile );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessExecuteArguments );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessStartOnBackground );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessWaitForProcessExit );
  DEFINE_PARAMETER_NAME_STRING( AfterProcessDeleteOutputFile );
  DEFINE_PARAMETER_NAME_STRING( Name );
  DEFINE_PARAMETER_NAME_STRING( UseScriptFile );
  DEFINE_PARAMETER_NAME_STRING( ScriptFilePath );
  DEFINE_PARAMETER_NAME_STRING( OutputLog );
  DEFINE_PARAMETER_NAME_STRING( LogFilePath );
  DEFINE_PARAMETER_NAME_STRING( NeverOverwriteOutputFile );
  DEFINE_PARAMETER_NAME_STRING( OutputFileTemplate );
  DEFINE_PARAMETER_NAME_STRING( RemoveCanNotUseCharacterAsFilePath );
}


// -- ConvertParameter ---------------------------------------------------
Core::ConvertParameter::ConvertParameter( void ) :
// 特殊
input_file_path_( "" ),

// 全般
name_( "" ),
use_script_file_( false ),
script_file_path_( "" ),
output_log_( false ),
log_file_path_( "" ),

// 出力
output_as_ogg_( false ),
ogg_base_quality_( 0.5 ),
never_overwrite_output_file_( true ),
remove_can_not_use_character_as_file_path_( true ),
output_file_template_( "" ),

// BMS
ignore_bga_channel_( true ),
not_nesting_if_statement_( true ),

// 変換
cancel_at_resounding_( true ),
convert_start_bar_number_( 0 ),
convert_end_bar_number_( 999 ),
do_triming_( false ),
triming_start_bar_number_( 0 ),
triming_end_bar_number_( 999 ),
remove_front_silence_( true ),
insert_front_silence_( true ),
insert_front_silence_second_( 0.7 ),

// 音量
normalize_kind_( Core::Parameter::Normalize::Default ),
normalize_over_ppm_( 100 ),
volume_( 100 ),

// 後処理
execute_after_process_( false ),
after_process_execute_file_( "" ),
after_process_execute_arguments_( "" ),
after_process_start_on_background_( false ),
after_process_wait_for_process_exit_( false ),
after_process_delete_output_file_( false ),

// 制御
immediately_abort_error_level_( ErrorLevel::ImmediatelyAbort ),
delay_abort_error_level_( ErrorLevel::Fatal ),
bar_resolution_max_( 0 ),
usable_memory_mega_byte_size_( 0 )
{
}


void
Core::ConvertParameter::ReadFromFile( const std::string& path )
{
  using namespace Parameter;

  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::ConvertParameter];

  // 全般
  name_             = section.GetString(  Tag::Name,           ""    );
  use_script_file_  = section.GetBoolean( Tag::UseScriptFile,  false );
  script_file_path_ = section.GetString(  Tag::ScriptFilePath, ""    );
  output_log_       = section.GetBoolean( Tag::OutputLog,      false );
  log_file_path_    = section.GetString(  Tag::LogFilePath,    ""    );

  // 出力
  output_as_ogg_                             = section.GetBoolean( Tag::OutputAsOgg,                        false );
  ogg_base_quality_                          = section.GetDouble(  Tag::OggBaseQuality,                     0.5   );
  never_overwrite_output_file_               = section.GetBoolean( Tag::NeverOverwriteOutputFile,           true  );
  remove_can_not_use_character_as_file_path_ = section.GetBoolean( Tag::RemoveCanNotUseCharacterAsFilePath, true  );
  output_file_template_                      = section.GetString(  Tag::OutputFileTemplate,                 ""    );

  // BMS
  ignore_bga_channel_       = section.GetBoolean( Tag::IgnoreBgaChannel,      true );
  not_nesting_if_statement_ = section.GetBoolean( Tag::NotNestingIfStatement, true );

  // 変換
  cancel_at_resounding_        = section.GetBoolean( Tag::CancelAtResounding,       true );
  convert_start_bar_number_    = section.GetInteger( Tag::ConvertStartBarNumber,    0 );
  convert_end_bar_number_      = section.GetInteger( Tag::ConvertEndBarNumber,      999 );
  do_triming_                  = section.GetBoolean( Tag::DoTriming,                false );
  triming_start_bar_number_    = section.GetInteger( Tag::TrimingStartBarNumber,    0 );
  triming_end_bar_number_      = section.GetInteger( Tag::TrimingEndBarNumber,      999 );
  remove_front_silence_        = section.GetBoolean( Tag::RemoveFrontSilence,       true );
  insert_front_silence_        = section.GetBoolean( Tag::InsertFrontSilence,       true );
  insert_front_silence_second_ = section.GetDouble(  Tag::InsertFrontSilenceSecond, 0.7 );

  // 音量
  normalize_kind_     = section.GetEnum<Parameter::Normalize>( Tag::NormalizeKind, Parameter::Normalize::Default );
  normalize_over_ppm_ = section.GetInteger( Tag::NormalizeOverPPM, 100 );
  volume_             = section.GetInteger( Tag::Volume,           100 );

  // 後処理
  execute_after_process_               = section.GetBoolean( Tag::ExecuteAfterProcess,            false );
  after_process_execute_file_          = section.GetString(  Tag::AfterProcessExecuteFile,        "" );
  after_process_execute_arguments_     = section.GetString(  Tag::AfterProcessExecuteArguments,   "" );
  after_process_start_on_background_   = section.GetBoolean( Tag::AfterProcessStartOnBackground,  false );
  after_process_wait_for_process_exit_ = section.GetBoolean( Tag::AfterProcessWaitForProcessExit, false );
  after_process_delete_output_file_    = section.GetBoolean( Tag::AfterProcessDeleteOutputFile,   false );

  // 制御
  immediately_abort_error_level_ = section.GetEnum<ErrorLevel>( Tag::ImmediatelyAbortErrorLevel, ErrorLevel::ImmediatelyAbort );
  delay_abort_error_level_       = section.GetEnum<ErrorLevel>( Tag::DelayAbortErrorLevel,       ErrorLevel::Fatal );
  {
    // -1 の場合は最大値に
    int tmp = section.GetInteger( Tag::BarResolutionMax, 0 );
    bar_resolution_max_ = ( tmp == -1 ) ? std::numeric_limits<decltype( bar_resolution_max_ )>::max() : tmp;
  }
  usable_memory_mega_byte_size_  = section.GetInteger( Tag::UsableMemoryMegaByteSize, 0 );

  ini_file.Flush();
}

void
Core::ConvertParameter::WriteToFile( const std::string& path ) const
{
  using namespace Parameter;

  TtIniFile ini_file( path );
  TtIniSection section = ini_file[Tag::ConvertParameter];

  // 全般
  section.SetString(  Tag::Name,           name_ );
  section.SetBoolean( Tag::UseScriptFile,  use_script_file_ );
  section.SetString(  Tag::ScriptFilePath, script_file_path_ );
  section.SetBoolean( Tag::OutputLog,      output_log_ );
  section.SetString(  Tag::LogFilePath,    log_file_path_ );

  // 出力
  section.SetBoolean( Tag::OutputAsOgg,                        output_as_ogg_ );
  section.SetDouble(  Tag::OggBaseQuality,                     ogg_base_quality_ );
  section.SetBoolean( Tag::NeverOverwriteOutputFile,           never_overwrite_output_file_ );
  section.SetBoolean( Tag::RemoveCanNotUseCharacterAsFilePath, remove_can_not_use_character_as_file_path_ );
  section.SetString(  Tag::OutputFileTemplate,                 output_file_template_ );

  // BMS
  section.SetBoolean( Tag::IgnoreBgaChannel,      ignore_bga_channel_ );
  section.SetBoolean( Tag::NotNestingIfStatement, not_nesting_if_statement_ );

  // 変換
  section.SetBoolean( Tag::CancelAtResounding,       cancel_at_resounding_ );
  section.SetInteger( Tag::ConvertStartBarNumber,    convert_start_bar_number_ );
  section.SetInteger( Tag::ConvertEndBarNumber,      convert_end_bar_number_ );
  section.SetBoolean( Tag::DoTriming,                do_triming_ );
  section.SetInteger( Tag::TrimingStartBarNumber,    triming_start_bar_number_ );
  section.SetInteger( Tag::TrimingEndBarNumber,      triming_end_bar_number_ );
  section.SetBoolean( Tag::RemoveFrontSilence,       remove_front_silence_ );
  section.SetBoolean( Tag::InsertFrontSilence,       insert_front_silence_ );
  section.SetDouble(  Tag::InsertFrontSilenceSecond, insert_front_silence_second_ );

  // 音量
  section.SetEnum<Parameter::Normalize>( Tag::NormalizeKind,    normalize_kind_ );
  section.SetInteger(                    Tag::NormalizeOverPPM, normalize_over_ppm_ );
  section.SetInteger(                    Tag::Volume,           volume_ );

  // 後処理
  section.SetBoolean( Tag::ExecuteAfterProcess,            execute_after_process_ );
  section.SetString(  Tag::AfterProcessExecuteFile,        after_process_execute_file_ );
  section.SetString(  Tag::AfterProcessExecuteArguments,   after_process_execute_arguments_ );
  section.SetBoolean( Tag::AfterProcessStartOnBackground,  after_process_start_on_background_ );
  section.SetBoolean( Tag::AfterProcessWaitForProcessExit, after_process_wait_for_process_exit_ );
  section.SetBoolean( Tag::AfterProcessDeleteOutputFile,   after_process_delete_output_file_ );

  // 制御
  section.SetEnum<ErrorLevel>( Tag::ImmediatelyAbortErrorLevel, immediately_abort_error_level_ );
  section.SetEnum<ErrorLevel>( Tag::DelayAbortErrorLevel,       delay_abort_error_level_ );
  section.SetInteger(          Tag::BarResolutionMax,           bar_resolution_max_ );
  section.SetInteger(          Tag::UsableMemoryMegaByteSize,   usable_memory_mega_byte_size_ );

  ini_file.Flush();
}


// -- 出力テンプレート変換関数 -----
std::string
Core::TranslateTemplatePath( const ConvertParameter& parameter, BL::BmsData* bms_data )
{
  return Core::TranslateTemplatePath( parameter.output_file_template_, parameter.input_file_path_, parameter.output_as_ogg_, bms_data );
}

std::string
Core::TranslateTemplatePath( const std::string& template_path,
                             const std::string& input_path,
                             bool               output_as_ogg,
                             BL::BmsData*       bms_data )
{
  TtTextTemplate::Document document;
  document.ParseText( template_path );
  auto if_documente_has_kay_register_string = [&] ( const std::string& key, const std::string& value ) {
    if ( document.HasKey( key ) ) {
      document[key] = value;
    }
  };
  auto& Q = if_documente_has_kay_register_string;

  const std::string auto_ext = output_as_ogg ? "ogg" : "wav";
  Q( "input_bms_path",                           input_path );
  Q( "input_bms_path_without_extension",         TtPath::RemoveExtension( input_path ) );
  Q( "input_bms_directory",                      TtPath::DirName( input_path ) );
  Q( "input_bms_parent_directory",               TtPath::DirName( TtPath::DirName( input_path ) ) );
  Q( "input_bms_basename",                       TtPath::BaseName( input_path ) );
  Q( "input_bms_basename_without_extension",     TtPath::RemoveExtension( TtPath::BaseName( input_path ) ) );
  Q( "input_bms_auto_extension_change",          TtPath::ChangeExtension( input_path, auto_ext ) );
  Q( "input_bms_basename_auto_extension_change", TtPath::BaseName( TtPath::ChangeExtension( input_path, auto_ext ) ) );
  Q( "auto_extension",                           auto_ext );
  Q( "bmx2wav_directory",                        TtPath::GetExecutingDirectoryPath() );

  for ( auto& key : document.GetKeys() ) {
    if ( TtString::StartWith( key, "header_" ) ) {
      std::string header = TtString::ToUpper( key.substr( 7 ) );
      if ( bms_data->headers_.contains( header ) ) {
        document[key] = bms_data->headers_[header];
      }
      else {
        document[key] = "";
      }
    }
  }

  return document.MakeText();
}


// -- TemplatePathTranslateHelpDialog ------------------------------------
Core::TemplatePathTranslateHelpDialog::TemplatePathTranslateHelpDialog( void ) :
menu_( TtSubMenu::Create() )
{
}


DWORD
Core::TemplatePathTranslateHelpDialog::GetStyle( void )
{
  return WS_SYSMENU | WS_CAPTION | WS_SIZEBOX;
}

DWORD
Core::TemplatePathTranslateHelpDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
Core::TemplatePathTranslateHelpDialog::Created( void )
{
  struct CommandID {
    enum ID : int {
      List = 15001,
      MenuCopy,
    };
  };

  this->SetText( "使用可能変数一覧" );

  list_.Create( {this, CommandID::List} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    list_.SetPositionSize( 2, 2, w - 4, h - 4 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 670, 200, false );

  menu_.AppendNewItem( CommandID::MenuCopy, "変数名をコピー(&C)" );
  this->AddNotifyHandler( CommandID::List, [this] ( NMHDR* nmhdr ) -> WMResult {
    // アイテム右クリック
    if ( nmhdr->code == NM_RCLICK ) {
      POINT cursor_point = TtWindow::GetCursorPosition();
      TtListViewItem hit_item = list_.HitTest( cursor_point.x - list_.GetPoint().x, cursor_point.y - list_.GetPoint().y );
      if ( hit_item.IsValid() ) {
        hit_item.SetSelected( true );
      }
      TtListViewItem item = list_.GetSelected();
      if ( item.IsValid() ) {
        menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::MenuCopy, [this] ( int, HWND ) -> WMResult {
    TtListViewItem item = list_.GetSelected();
    if ( item.IsValid() ) {
      TtClipboard::SetString( item.GetSubItemText( 0 ) );
    }
    return {WMResult::Done};
  } );

  this->RegisterWMClose( [this] ( void ) -> WMResult {
    this->Hide();
    // 閉じない
    return {WMResult::Done};
  }, false );

  // -- 文字列変換ヘルプリスト
  {
    list_.SetFullRowSelect( true );
    TtListViewColumn column_name = list_.MakeNewColumn();
    column_name.SetText( "変数名" );
    TtListViewColumn column_description = list_.MakeNewColumn();
    column_description.SetText( "説明" );

    auto add = [this] ( const std::string& var, const std::string& description ) {
      auto item = list_.MakeNewItem();
      item.SetSubItemText( 0, "@@" + var + "@@" );
      item.SetSubItemText( 1, description );
    };

    add( "input_bms_path", "入力 BMS ファイルのパス" );
    add( "input_bms_path_without_extension", "入力 BMS ファイルのパスから拡張子部分を除いたもの" );
    add( "input_bms_directory", "入力 BMS ファイルがあるフォルダのパス" );
    add( "input_bms_parent_directory", "入力 BMS ファイルがあるフォルダの親フォルダのパス" );
    add( "input_bms_basename", "入力 BMS ファイルのパスのファイル名部分" );
    add( "input_bms_basename_without_extension", "入力 BMS ファイルのパスのファイル名部分から拡張子部分を除いたもの" );
    add( "input_bms_auto_extension_change", "入力 BMS ファイルのパスから拡張子を出力設定に合わせて wav または ogg に変えたもの" );
    add( "input_bms_basename_auto_extension_change", "入力 BMS ファイルのパスのファイル名部分からその拡張子を出力設定に合わせて wav または ogg に変えたもの" );
    add( "auto_extension", "出力設定に合わせて wav または ogg" );
    add( "bmx2wav_directory", "BMX2WAV 実行ファイルの置かれているフォルダのパス" );
    add( "header_XXXXX", "入力 BMS ファイル内データのヘッダ XXXXX の値" );

    column_name.SetWidthAuto();
    column_description.SetWidthAuto();
  }

  list_.Show();
  return {true};
}
