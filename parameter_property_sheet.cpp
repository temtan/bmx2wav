// parameter_property_sheet.cpp

#include "tt_message_box.h"
#include "tt_window_utility.h"
#include "tt_file_dialog.h"

#include "base/parser.h"
#include "string_table.h"

#include "parameter_property_sheet.h"

using namespace BMX2WAV;


// -- GeneralPage --------------------------------------------------------
ParameterPropertySheet::GeneralPage::GeneralPage( Core::ConvertParameter& parameter, ParameterPropertySheet& parent ) :
Page( StrT::PPS::General.Get() ),
parameter_( parameter ),
parent_( parent )
{
  this->PresetSizeAtPageCreate( 268, 220 );
}

bool
ParameterPropertySheet::GeneralPage::Created( void )
{
  load_save_group_.Create( {this} );
  load_button_.Create( {this, CommandID::LoadButton} );
  save_button_.Create( {this, CommandID::SaveButton} );
  name_label_.Create( {this} );
  name_edit_.Create( {this} );
  use_script_check_.Create( {this, CommandID::UseScriptCheck} );
  script_label_.Create( {this} );
  script_edit_.Create( {this} );
  script_ref_button_.Create( {this, CommandID::ScriptReferenceButton} );
  output_log_check_.Create( {this, CommandID::OutputLogCheck} );
  log_label_.Create( {this} );
  log_edit_.Create( {this} );
  log_ref_button_.Create( {this, CommandID::LogReferenceButton} );
  control_created_ = true;

  load_save_group_.SetPositionSize(     4,   4, 316, 60 );
  load_button_.SetPositionSize(        24,  20, 120, 32 );
  save_button_.SetPositionSize(       174,  20, 120, 32 );
  name_label_.SetPositionSize(          4,  76, 100, 16 );
  name_edit_.SetPositionSize(         120,  72, 160, 20 );
  use_script_check_.SetPositionSize(    4, 106, 200, 16 );
  script_label_.SetPositionSize(       12, 130, 200, 16 );
  script_edit_.SetPositionSize(        12, 150, 300, 20 );
  script_ref_button_.SetPositionSize( 316, 150,  52, 20 );
  output_log_check_.SetPositionSize(    4, 192, 200, 16 );
  log_label_.SetPositionSize(          12, 216, 200, 16 );
  log_edit_.SetPositionSize(           12, 236, 300, 20 );
  log_ref_button_.SetPositionSize(    316, 236,  52, 20 );

  load_save_group_.SetText(   StrT::PPS::GeneralLoadSaveGroup.Get() );
  load_button_.SetText(       StrT::PPS::GeneralLoadButton.Get() );
  save_button_.SetText(       StrT::PPS::GeneralSaveButton.Get() );
  name_label_.SetText(        StrT::PPS::GeneralNameLabel.Get() );
  use_script_check_.SetText(  StrT::PPS::GeneralUseScriptCheck.Get() );
  script_label_.SetText(      StrT::PPS::GeneralScriptLabel.Get() );
  script_ref_button_.SetText( StrT::PPS::GeneralScriptRefButton.Get() );
  output_log_check_.SetText(  StrT::PPS::GeneralOutputLogCheck.Get() );
  log_label_.SetText(         StrT::PPS::GeneralLogLabel.Get() );
  log_ref_button_.SetText(    StrT::PPS::GeneralLogRefButton.Get() );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    parameter_.name_             = name_edit_.GetText();
    parameter_.use_script_file_  = use_script_check_.GetCheck();
    parameter_.script_file_path_ = script_edit_.GetText();
    parameter_.output_log_       = output_log_check_.GetCheck();
    parameter_.log_file_path_    = log_edit_.GetText();
    return true;
  };

  this->AddCommandHandler( CommandID::LoadButton, [&] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {StrT::PPS::GeneralFileDialogAllFile.Get(), "*.*"} );
    if ( dialog.ShowDialog( *this ) ) {
      Core::ConvertParameter tmp;
      tmp.ReadFromFile( dialog.GetFileName() );
      parameter_ = tmp;
      parent_.SetParameterToPagesControl();
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::SaveButton, [&] ( int, HWND ) -> WMResult {
    TtSaveFileDialog dialog;
    dialog.GetFilters().push_back( {StrT::PPS::GeneralFileDialogAllFile.Get(), "*.*"} );
    if ( dialog.ShowDialog( *this ) ) {
      auto tmp = parameter_;
      parent_.CallAtApplyOfPages();
      parameter_.WriteToFile( dialog.GetFileName() );
      parameter_ = tmp;
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::UseScriptCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      script_label_.SetEnabled( use_script_check_.GetCheck() );
      script_edit_.SetEnabled( use_script_check_.GetCheck() );
      script_ref_button_.SetEnabled( use_script_check_.GetCheck() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ScriptReferenceButton, [&] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {StrT::PPS::GeneralFileDialogAllFile.Get(), "*.*"} );
    if ( dialog.ShowDialog( *this ) ) {
      script_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OutputLogCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      log_label_.SetEnabled( output_log_check_.GetCheck() );
      log_edit_.SetEnabled( output_log_check_.GetCheck() );
      log_ref_button_.SetEnabled( output_log_check_.GetCheck() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::LogReferenceButton, [&] ( int, HWND ) -> WMResult {
    TtSaveFileDialog dialog;
    dialog.SetOverwritePrompt( false );
    dialog.GetFilters().push_back( {StrT::PPS::GeneralFileDialogAllFile.Get(), "*.*"} );
    if ( dialog.ShowDialog( *this ) ) {
      log_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  this->SetParameterToControls();

  load_save_group_.Show();
  load_button_.Show();
  save_button_.Show();
  name_label_.Show();
  name_edit_.Show();
  use_script_check_.Show();
  script_label_.Show();
  script_edit_.Show();
  script_ref_button_.Show();
  output_log_check_.Show();
  log_label_.Show();
  log_edit_.Show();
  log_ref_button_.Show();

  return true;
}

void
ParameterPropertySheet::GeneralPage::SetParameterToControlsBody( void )
{
  name_edit_.SetText( parameter_.name_ );
  use_script_check_.SetCheck( parameter_.use_script_file_ );
  script_edit_.SetText( parameter_.script_file_path_ );
  output_log_check_.SetCheck( parameter_.output_log_ );
  log_edit_.SetText( parameter_.log_file_path_ );

  this->CallCommandHandler( CommandID::UseScriptCheck, BN_CLICKED, use_script_check_.GetHandle() );
  this->CallCommandHandler( CommandID::OutputLogCheck, BN_CLICKED, output_log_check_.GetHandle() );
}

// -- OutputPage ---------------------------------------------------------
ParameterPropertySheet::OutputPage::OutputPage( Core::ConvertParameter& parameter, ParameterPropertySheet& parent ) :
Page( StrT::PPS::Output.Get() ),
parameter_( parameter ),
parent_( parent ),

font_for_output_template_edit_( ::CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                              SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                                              CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                              DEFAULT_PITCH | FF_MODERN, nullptr ), true )
{
}

bool
ParameterPropertySheet::OutputPage::Created( void )
{
  output_as_ogg_check_.Create( {this, CommandID::OutputAsOggCheck} );
  ogg_base_quality_label_.Create( {this} );
  ogg_base_quality_edit_.Create( {this} );
  never_overwrite_check_.Create( {this} );
  remove_char_check_.Create( {this} );
  output_file_template_label_.Create( {this} );
  output_file_help_button_.Create( {this, CommandID::OutputTemplateHelpButton} );
  output_file_template_edit_.Create( {this, CommandID::OutputTemplateEdit} );
  output_file_template_input_bms_label_.Create( {this} );
  output_file_template_input_bms_edit_.Create( {this} );
  output_file_template_result_label_.Create( {this} );
  output_file_template_result_edit_.Create( {this} );
  control_created_ = true;

  output_as_ogg_check_.SetPositionSize(                    4,  12, 300,  16 );
  ogg_base_quality_label_.SetPositionSize(                12,  32, 220,  16 );
  ogg_base_quality_edit_.SetPositionSize(                240,  28,  50,  20 );
  never_overwrite_check_.SetPositionSize(                  4,  60, 360,  16 );
  remove_char_check_.SetPositionSize(                      4,  88, 360,  16 );
  output_file_template_label_.SetPositionSize(             4, 130, 300,  16 );
  output_file_help_button_.SetPositionSize(              200, 124, 230,  24 );
  output_file_template_edit_.SetPositionSize(             12, 156, 440,  20 );
  output_file_template_input_bms_label_.SetPositionSize(  12, 196, 200,  16 );
  output_file_template_input_bms_edit_.SetPositionSize(  152, 192, 300,  20 );
  output_file_template_result_label_.SetPositionSize(     12, 220, 300,  16 );
  output_file_template_result_edit_.SetPositionSize(      12, 240, 440,  20 );

  output_file_template_edit_.SetFont( font_for_output_template_edit_ );
  output_file_template_input_bms_edit_.SetFont( font_for_output_template_edit_ );
  output_file_template_result_edit_.SetFont( font_for_output_template_edit_ );

  output_as_ogg_check_.SetText(                  StrT::PPS::OutputOutputAsOggCheck.Get() );
  ogg_base_quality_label_.SetText(               StrT::PPS::OutputOggBaseQualityLabel.Get());
  never_overwrite_check_.SetText(                StrT::PPS::OutputNeverOverwriteCheck.Get() );
  remove_char_check_.SetText(                    StrT::PPS::OutputRemoveCharCheck.Get() );
  output_file_template_label_.SetText(           StrT::PPS::OutputOutputFileTemplateLabel.Get() );
  output_file_help_button_.SetText(              StrT::PPS::OutputOutputFileHelpButton.Get() );
  output_file_template_input_bms_label_.SetText( StrT::PPS::OutputOutputFileExampleBms.Get() );
  output_file_template_result_label_.SetText(    StrT::PPS::OutputOutputFileExampleResult.Get() );

  this->GetHandlers().at_kill_active = [this] ( void ) -> bool {
    TtMessageBoxOk box;
    box.SetCaption( StrT::PPS::OutputMBOggBaseQualityCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );

    if ( double tmp; NOT( TtUtility::StringToDouble( ogg_base_quality_edit_.GetText(), &tmp ) ) ) {
      box.SetMessage( StrT::PPS::OutputMBOggBaseQualityMessage.Get() );
      box.ShowDialog( *this );
      return false;
    }
    return true;
  };

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    if ( NOT( this->GetHandlers().at_kill_active() ) ) {
      return false;
    }

    parameter_.output_as_ogg_ = output_as_ogg_check_.GetCheck();
    TtUtility::StringToDouble( ogg_base_quality_edit_.GetText(), &parameter_.ogg_base_quality_ );
    parameter_.never_overwrite_output_file_ = never_overwrite_check_.GetCheck();
    parameter_.remove_can_not_use_character_as_file_path_ = remove_char_check_.GetCheck();
    parameter_.output_file_template_ = output_file_template_edit_.GetText();
    return true;
  };

  this->AddCommandHandler( CommandID::OutputAsOggCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      ogg_base_quality_label_.SetEnabled( output_as_ogg_check_.GetCheck() );
      ogg_base_quality_edit_.SetEnabled( output_as_ogg_check_.GetCheck() );
      this->CallCommandHandler( CommandID::OutputTemplateEdit, EN_CHANGE, output_file_template_edit_.GetHandle() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OutputTemplateHelpButton, [this] ( int, HWND ) -> WMResult {
    output_file_help_dialog_.Show();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OutputTemplateEdit, [this] ( int code, HWND ) -> WMResult {
    if ( code == EN_CHANGE ) {
      std::string path = output_file_template_input_bms_edit_.GetText();
      BL::BmsData* bms_data = nullptr;
      if ( parent_.individual_bms_path_ ) {
        path     = parent_.individual_bms_path_.value();
        bms_data = parent_.bms_data_.get();
      }
      auto result = Core::TranslateTemplatePath( output_file_template_edit_.GetText(), path, output_as_ogg_check_.GetCheck(), bms_data );
      output_file_template_result_edit_.SetText( result );
    }
    return {WMResult::Done};
  } );

  this->SetParameterToControls();

  output_as_ogg_check_.Show();
  ogg_base_quality_label_.Show();
  ogg_base_quality_edit_.Show();
  never_overwrite_check_.Show();
  remove_char_check_.Show();
  output_file_template_label_.Show();
  output_file_template_edit_.Show();
  output_file_help_button_.Show();
  output_file_template_input_bms_label_.Show();
  output_file_template_input_bms_edit_.Show();
  output_file_template_result_label_.Show();
  output_file_template_result_edit_.Show();

  // Show ‚µ‚È‚¢‚Æ•\Ž¦‚³‚ê‚È‚¢‚Ì‚Å’ˆÓ
  output_file_help_dialog_.ShowDialog( this->GetParentSheet() );

  return true;
}

void
ParameterPropertySheet::OutputPage::SetParameterToControlsBody( void )
{
  output_file_template_input_bms_edit_.SetText( parent_.individual_bms_path_ ? parent_.individual_bms_path_.value() : "C:\\hoge\\fuga\\piyo.bms" );

  output_as_ogg_check_.SetCheck( parameter_.output_as_ogg_ );
  ogg_base_quality_edit_.SetText( TtUtility::ToStringFrom( parameter_.ogg_base_quality_ ) );
  never_overwrite_check_.SetCheck( parameter_.never_overwrite_output_file_ );
  remove_char_check_.SetCheck( parameter_.remove_can_not_use_character_as_file_path_ );
  output_file_template_edit_.SetText( parameter_.output_file_template_ );

  this->CallCommandHandler( CommandID::OutputAsOggCheck, BN_CLICKED, output_as_ogg_check_.GetHandle() );
}


// -- ParserPage ---------------------------------------------------------
ParameterPropertySheet::ParserPage::ParserPage( Core::ConvertParameter& parameter ) :
Page( StrT::PPS::Parser.Get() ),
parameter_( parameter )
{
}

bool
ParameterPropertySheet::ParserPage::Created( void )
{
  read_as_utf8_check_.Create( {this} );
  ignore_bga_channel_check_.Create( {this} );
  not_nesting_if_statement_check_.Create( {this} );
  control_created_ = true;

  read_as_utf8_check_.SetPositionSize(              4, 12, 400, 16 );
  ignore_bga_channel_check_.SetPositionSize(        4, 40, 400, 16 );
  not_nesting_if_statement_check_.SetPositionSize(  4, 60, 400, 16 );

  read_as_utf8_check_.SetText(             StrT::PPS::ParserReadAsUTF8Check.Get() );
  ignore_bga_channel_check_.SetText(       StrT::PPS::ParserIgnoreBGAChannelCheck.Get() );
  not_nesting_if_statement_check_.SetText( StrT::PPS::ParserNotNestingIfStatementCheck.Get() );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    parameter_.read_as_utf8_             = read_as_utf8_check_.GetCheck();
    parameter_.ignore_bga_channel_       = ignore_bga_channel_check_.GetCheck();
    parameter_.not_nesting_if_statement_ = not_nesting_if_statement_check_.GetCheck();
    return true;
  };

  this->SetParameterToControls();

  read_as_utf8_check_.Show();
  ignore_bga_channel_check_.Show();
  not_nesting_if_statement_check_.Show();

  return true;
}

void
ParameterPropertySheet::ParserPage::SetParameterToControlsBody( void )
{
  read_as_utf8_check_.SetCheck( parameter_.read_as_utf8_ );
  ignore_bga_channel_check_.SetCheck( parameter_.ignore_bga_channel_ );
  not_nesting_if_statement_check_.SetCheck( parameter_.not_nesting_if_statement_ );
}


// -- MixinPage ----------------------------------------------------------
ParameterPropertySheet::MixinPage::MixinPage( Core::ConvertParameter& parameter ) :
Page( StrT::PPS::Mixin.Get() ),
parameter_( parameter )
{
}

bool
ParameterPropertySheet::MixinPage::Created( void )
{
  cancel_at_resounding_check_.Create( {this} );
  start_label_.Create( {this} );
  end_label_.Create( {this} );
  convert_start_end_bar_number_label_.Create( {this} );
  convert_start_bar_number_edit_.Create( {this} );
  convert_end_bar_number_edit_.Create( {this} );
  do_triming_check_.Create( {this, CommandID::DoTrimingCheck} );
  triming_start_bar_number_edit_.Create( {this} );
  triming_end_bar_number_edit_.Create( {this} );
  remove_front_silence_check_.Create( {this} );
  insert_front_silence_check_.Create( {this, CommandID::InsertFrontSilenceCheck} );
  insert_front_silence_second_label_.Create( {this} );
  insert_front_silence_second_edit_.Create( {this} );
  control_created_ = true;

  cancel_at_resounding_check_.SetPositionSize(           4,  12, 450, 16 );
  start_label_.SetPositionSize(                        224,  44, 100, 16 );
  end_label_.SetPositionSize(                          328,  44, 100, 16 );
  convert_start_end_bar_number_label_.SetPositionSize(  88,  74, 150, 16 );
  convert_start_bar_number_edit_.SetPositionSize(      240,  70,  40, 20 );
  convert_end_bar_number_edit_.SetPositionSize(        340,  70,  40, 20 );
  do_triming_check_.SetPositionSize(                     8, 114, 220, 16 );
  triming_start_bar_number_edit_.SetPositionSize(      240, 110,  40, 20 );
  triming_end_bar_number_edit_.SetPositionSize(        340, 110,  40, 20 );
  remove_front_silence_check_.SetPositionSize(           4, 160, 240, 16 );
  insert_front_silence_check_.SetPositionSize(           4, 180, 240, 16 );
  insert_front_silence_second_label_.SetPositionSize(    4, 204, 120, 16 );
  insert_front_silence_second_edit_.SetPositionSize(   130, 200,  60, 20 );

  cancel_at_resounding_check_.SetText(         StrT::PPS::MixinCancelAtResoundingCheck.Get() );
  start_label_.SetText(                        StrT::PPS::MixinStartLabel.Get() );
  end_label_.SetText(                          StrT::PPS::MixinEndLabel.Get() );
  convert_start_end_bar_number_label_.SetText( StrT::PPS::MixinConvertStartEndBarNumberLabel.Get() );
  do_triming_check_.SetText(                   StrT::PPS::MixinDoTrimingCheck.Get() );
  remove_front_silence_check_.SetText(         StrT::PPS::MixinRemoveFrontSilenceCheck.Get() );
  insert_front_silence_check_.SetText(         StrT::PPS::MixinInsertFrontSilenceCheck.Get() );
  insert_front_silence_second_label_.SetText(  StrT::PPS::MixinInsertFrontSilenceSecondLabel.Get() );

  this->GetHandlers().at_kill_active = [this] ( void ) -> bool {
    TtMessageBoxOk box;
    box.SetCaption( StrT::PPS::MixinMBKillActiveCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    unsigned int start;
    unsigned int end;
    if ( NOT( TtUtility::StringToInteger( convert_start_bar_number_edit_.GetText(), &start ) ) ||
         ( start < 0 || start > 999 ) ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageStart.Get() );
      box.ShowDialog( *this );
      return false;
    };
    if ( NOT( TtUtility::StringToInteger( convert_end_bar_number_edit_.GetText(), &end ) ) ||
         ( end < 0 || end > 999 ) ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageEnd.Get() );
      box.ShowDialog( *this );
      return false;
    };
    if ( start > end ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageOrder.Get() );
      box.ShowDialog( *this );
      return false;
    }
    if ( NOT( TtUtility::StringToInteger( triming_start_bar_number_edit_.GetText(), &start ) ) ||
         ( start < 0 || start > 999 ) ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageTrimStart.Get() );
      box.ShowDialog( *this );
      return false;
    };
    if ( NOT( TtUtility::StringToInteger( triming_end_bar_number_edit_.GetText(), &end ) ) ||
         ( end < 0 || end > 999 ) ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageTrimEnd.Get() );
      box.ShowDialog( *this );
      return false;
    };
    if ( start > end ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageTrimOrder.Get() );
      box.ShowDialog( *this );
      return false;
    }

    if ( double tmp; NOT( TtUtility::StringToDouble( insert_front_silence_second_edit_.GetText(), &tmp ) ) ) {
      box.SetMessage( StrT::PPS::MixinMBKillActiveMessageFrontSilence.Get() );
      box.ShowDialog( *this );
      return false;
    }
    return true;
  };

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    if ( NOT( this->GetHandlers().at_kill_active() ) ) {
      return false;
    }

    parameter_.cancel_at_resounding_ = cancel_at_resounding_check_.GetCheck();
    TtUtility::StringToInteger( convert_start_bar_number_edit_.GetText(), &parameter_.convert_start_bar_number_ );
    TtUtility::StringToInteger( convert_end_bar_number_edit_.GetText(), &parameter_.convert_end_bar_number_ );
    parameter_.do_triming_ = do_triming_check_.GetCheck();
    TtUtility::StringToInteger( triming_start_bar_number_edit_.GetText(), &parameter_.triming_start_bar_number_ );
    TtUtility::StringToInteger( triming_end_bar_number_edit_.GetText(), &parameter_.triming_end_bar_number_ );
    parameter_.remove_front_silence_ = remove_front_silence_check_.GetCheck();
    parameter_.insert_front_silence_ = insert_front_silence_check_.GetCheck();
    TtUtility::StringToDouble( insert_front_silence_second_edit_.GetText(), &parameter_.insert_front_silence_second_ );

    return true;
  };

  this->AddCommandHandler( CommandID::DoTrimingCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      triming_start_bar_number_edit_.SetEnabled( do_triming_check_.GetCheck() );
      triming_end_bar_number_edit_.SetEnabled( do_triming_check_.GetCheck() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::InsertFrontSilenceCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      insert_front_silence_second_label_.SetEnabled( insert_front_silence_check_.GetCheck() );
      insert_front_silence_second_edit_.SetEnabled( insert_front_silence_check_.GetCheck() );
    }
    return {WMResult::Done};
  } );

  this->SetParameterToControls();

  cancel_at_resounding_check_.Show();
  start_label_.Show();
  end_label_.Show();
  convert_start_end_bar_number_label_.Show();
  convert_start_bar_number_edit_.Show();
  convert_end_bar_number_edit_.Show();
  do_triming_check_.Show();
  triming_start_bar_number_edit_.Show();
  triming_end_bar_number_edit_.Show();
  remove_front_silence_check_.Show();
  insert_front_silence_check_.Show();
  insert_front_silence_second_label_.Show();
  insert_front_silence_second_edit_.Show();

  return true;
}

void
ParameterPropertySheet::MixinPage::SetParameterToControlsBody( void )
{
  cancel_at_resounding_check_.SetCheck( parameter_.cancel_at_resounding_ );
  convert_start_bar_number_edit_.SetText( TtUtility::ToStringFrom( parameter_.convert_start_bar_number_ ) );
  convert_end_bar_number_edit_.SetText( TtUtility::ToStringFrom( parameter_.convert_end_bar_number_ ) );;
  do_triming_check_.SetCheck( parameter_.do_triming_ );
  triming_start_bar_number_edit_.SetText( TtUtility::ToStringFrom( parameter_.triming_start_bar_number_ ) );
  triming_end_bar_number_edit_.SetText( TtUtility::ToStringFrom( parameter_.triming_end_bar_number_ ) );
  remove_front_silence_check_.SetCheck( parameter_.remove_front_silence_ );
  insert_front_silence_check_.SetCheck( parameter_.insert_front_silence_ );
  insert_front_silence_second_edit_.SetText( TtUtility::ToStringFrom( parameter_.insert_front_silence_second_ ) );

  this->CallCommandHandler( CommandID::DoTrimingCheck,          BN_CLICKED, do_triming_check_.GetHandle() );
  this->CallCommandHandler( CommandID::InsertFrontSilenceCheck, BN_CLICKED, insert_front_silence_check_.GetHandle() );
}


// -- AudioPage ----------------------------------------------------------
ParameterPropertySheet::AudioPage::AudioPage( Core::ConvertParameter& parameter ) :
Page( StrT::PPS::Audio.Get() ),
parameter_( parameter )
{
}


bool
ParameterPropertySheet::AudioPage::Created( void )
{
  {
    TtWindow::CreateParameter tmp = {this};
    tmp.group_start_ = true;
    normalize_kind_none_radio_.Create( tmp.AlterId( CommandID::NormalizeKindNoneRadio ) );
    normalize_kind_peak_radio_.Create( {this, CommandID::NormalizeKindPeakRadio} );
    normalize_kind_average_radio_.Create( {this, CommandID::NormalizeKindAverageRadio} );
    normalize_kind_over_radio_.Create( {this, CommandID::NormalizeKindOverRadio} );
    normalize_kind_group_.Create( tmp );
  }
  normalize_over_ppm_label_.Create( {this} );
  normalize_over_ppm_edit_.Create( {this} );
  volume_label_.Create( {this} );
  volume_edit_.Create( {this} );
  control_created_ = true;

  normalize_kind_none_radio_.SetPositionSize(     16,  30, 200,  16 );
  normalize_kind_peak_radio_.SetPositionSize(     16,  54, 200,  16 );
  normalize_kind_average_radio_.SetPositionSize(  16,  78, 200,  16 );
  normalize_kind_over_radio_.SetPositionSize(     16, 102, 200,  16 );
  normalize_kind_group_.SetPositionSize(           8,  10, 300, 140 );
  normalize_over_ppm_label_.SetPositionSize(      50, 126, 150,  16 );
  normalize_over_ppm_edit_.SetPositionSize(      208, 122,  60,  20 );
  volume_label_.SetPositionSize(                   8, 164, 100,  16 );
  volume_edit_.SetPositionSize(                  100, 160,  60,  20 );

  normalize_kind_none_radio_.SetText(    StrT::PPS::AudioNormalizeKindNoneRadio.Get() );
  normalize_kind_peak_radio_.SetText(    StrT::PPS::AudioNormalizeKindPeakRadio.Get() );
  normalize_kind_average_radio_.SetText( StrT::PPS::AudioNormalizeKindAverageRadio.Get() );
  normalize_kind_over_radio_.SetText(    StrT::PPS::AudioNormalizeKindOverRadio.Get() );
  normalize_kind_group_.SetText(         StrT::PPS::AudioNormalizeKindGroup.Get() );
  normalize_over_ppm_label_.SetText(     StrT::PPS::AudioNormalizeOverPPMLabel.Get() );
  volume_label_.SetText(                 StrT::PPS::AudioVolumeLabel.Get() );

       

 
  this->GetHandlers().at_kill_active = [this] ( void ) -> bool {
    TtMessageBoxOk box;
    box.SetCaption( StrT::PPS::AudioMBKillActiveCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );

    if ( unsigned int tmp; NOT( TtUtility::StringToInteger( normalize_over_ppm_edit_.GetText(), &tmp ) ) ) {
      box.SetMessage( StrT::PPS::AudioMBKillActiveMessageOverPPM.Get() );
      box.ShowDialog( *this );
      return false;
    }
    if ( int tmp; NOT( TtUtility::StringToInteger( volume_edit_.GetText(), &tmp ) ) ) {
      box.SetMessage( StrT::PPS::AudioMBKillActiveMessageVolume.Get() );
      box.ShowDialog( *this );
      return false;
    }
    return true;
  };
  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    if ( NOT( this->GetHandlers().at_kill_active() ) ) {
      return false;
    }

    {
      using Core::Parameter::Normalize;
      TtRadioCheckToValue<Normalize> table( Normalize::Default );
      table.Register( normalize_kind_none_radio_,    Normalize::None );
      table.Register( normalize_kind_peak_radio_,    Normalize::Peak );
      table.Register( normalize_kind_average_radio_, Normalize::Average );
      table.Register( normalize_kind_over_radio_,    Normalize::Over );
      parameter_.normalize_kind_ = table.GetValue();
    }
    TtUtility::StringToInteger( normalize_over_ppm_edit_.GetText(), &parameter_.normalize_over_ppm_ );
    TtUtility::StringToInteger( volume_edit_.GetText(), &parameter_.volume_ );
    return true;
  };

  {
    auto handler = [this] ( int code, HWND ) -> WMResult {
      if ( code == BN_CLICKED ) {
        normalize_over_ppm_label_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
        normalize_over_ppm_edit_.SetEnabled( normalize_kind_over_radio_.GetCheck() );
      }
      return {WMResult::Done};
    };
    this->AddCommandHandler( CommandID::NormalizeKindNoneRadio, handler );
    this->AddCommandHandler( CommandID::NormalizeKindPeakRadio, handler );
    this->AddCommandHandler( CommandID::NormalizeKindAverageRadio, handler );
    this->AddCommandHandler( CommandID::NormalizeKindOverRadio, handler );
  }

  this->SetParameterToControls();

  normalize_kind_none_radio_.Show();
  normalize_kind_peak_radio_.Show();
  normalize_kind_average_radio_.Show();
  normalize_kind_over_radio_.Show();
  normalize_kind_group_.Show();
  normalize_over_ppm_label_.Show();
  normalize_over_ppm_edit_.Show();
  volume_label_.Show();
  volume_edit_.Show();

  return true;
}

void
ParameterPropertySheet::AudioPage::SetParameterToControlsBody( void )
{
  {
    using Core::Parameter::Normalize;
    TtValueToRadioCheck<Normalize> table( normalize_kind_over_radio_ );
    table.Register( Normalize::None,    normalize_kind_none_radio_ );
    table.Register( Normalize::Peak,    normalize_kind_peak_radio_ );
    table.Register( Normalize::Average, normalize_kind_average_radio_ );
    table.Register( Normalize::Over,    normalize_kind_over_radio_ );
    table.SetCheck( parameter_.normalize_kind_ );
  }
  normalize_over_ppm_edit_.SetText( TtUtility::ToStringFrom( parameter_.normalize_over_ppm_ ) );
  volume_edit_.SetText( TtUtility::ToStringFrom( parameter_.volume_ ) );

  this->CallCommandHandler( CommandID::NormalizeKindNoneRadio, BN_CLICKED, normalize_kind_none_radio_.GetHandle() );
}


// -- AfterProcessPage ---------------------------------------------------
ParameterPropertySheet::AfterProcessPage::AfterProcessPage( Core::ConvertParameter& parameter ) :
Page( StrT::PPS::AfterProcess.Get() ),
parameter_( parameter )
{
}

bool
ParameterPropertySheet::AfterProcessPage::Created( void )
{
  execute_after_process_check_.Create( {this, CommandID::ExecuteAfterProcessCheck} );
  execute_file_label_.Create( {this} );
  execute_file_edit_.Create( {this} );
  execute_file_ref_button_.Create( {this, CommandID::ExecuteFileReferenceButton} );
  execute_arguments_label_.Create( {this} );
  execute_arguments_edit_.Create( {this} );
  execute_arguments_help_.Create( {this} );
  start_on_background_check_.Create( {this} );
  wait_for_process_exit_check_.Create( {this} );
  delete_output_file_check_.Create( {this} );
  control_created_ = true;

  execute_after_process_check_.SetPositionSize(   4,  12, 384,  16 );
  execute_file_label_.SetPositionSize(           12,  36, 384,  16 );
  execute_file_edit_.SetPositionSize(            12,  56, 300,  20 );
  execute_file_ref_button_.SetPositionSize(     316,  56,  52,  20 );
  execute_arguments_label_.SetPositionSize(      12,  84, 320,  20 );
  execute_arguments_edit_.SetPositionSize(       12, 104, 360,  20 );
  execute_arguments_help_.SetPositionSize(       24, 132, 384,  16 );
  start_on_background_check_.SetPositionSize(    12, 156, 384,  16 );
  wait_for_process_exit_check_.SetPositionSize(  12, 176, 384,  16 );
  delete_output_file_check_.SetPositionSize(     12, 196, 384,  16 );

  execute_after_process_check_.SetText( StrT::PPS::AfterProcessExecuteAfterProcessCheck.Get() );
  execute_file_label_.SetText(          StrT::PPS::AfterProcessExecuteFileLabel.Get() );
  execute_file_ref_button_.SetText(     StrT::PPS::AfterProcessExecuteFileRefButton.Get() );
  execute_arguments_label_.SetText(     StrT::PPS::AfterProcessExecuteArgumentsLabel.Get() );
  execute_arguments_help_.SetText(      StrT::PPS::AfterProcessExecuteArgumentsHelp.Get() );
  start_on_background_check_.SetText(   StrT::PPS::AfterProcessStartOnBackgroundCheck.Get() );
  wait_for_process_exit_check_.SetText( StrT::PPS::AfterProcessWaitForProcessExitCheck.Get() );
  delete_output_file_check_.SetText(    StrT::PPS::AfterProcessDeleteOutputFileCheck.Get() );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    parameter_.execute_after_process_               = execute_after_process_check_.GetCheck();
    parameter_.after_process_execute_file_          = execute_file_edit_.GetText();
    parameter_.after_process_execute_arguments_     = execute_arguments_edit_.GetText();
    parameter_.after_process_start_on_background_   = start_on_background_check_.GetCheck();
    parameter_.after_process_wait_for_process_exit_ = wait_for_process_exit_check_.GetCheck();
    parameter_.after_process_delete_output_file_    = delete_output_file_check_.GetCheck();
    return true;
  };

  this->AddCommandHandler( CommandID::ExecuteAfterProcessCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      const bool flag = execute_after_process_check_.GetCheck();
      execute_file_label_.SetEnabled( flag );
      execute_file_edit_.SetEnabled( flag );
      execute_file_ref_button_.SetEnabled( flag );
      execute_arguments_label_.SetEnabled( flag );
      execute_arguments_edit_.SetEnabled( flag );
      execute_arguments_help_.SetEnabled( flag );
      start_on_background_check_.SetEnabled( flag );
      wait_for_process_exit_check_.SetEnabled( flag );
      delete_output_file_check_.SetEnabled( flag );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ExecuteFileReferenceButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {StrT::PPS::AfterProcessFileDialogExeFile.Get(), "*.exe"} );
    dialog.GetFilters().push_back( {StrT::PPS::AfterProcessFileDialogAllFile.Get(), "*.*"} );
    if ( dialog.ShowDialog( *this ) ) {
      execute_file_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  this->SetParameterToControls();

  execute_after_process_check_.Show();
  execute_file_label_.Show();
  execute_file_edit_.Show();
  execute_file_ref_button_.Show();
  execute_arguments_label_.Show();
  execute_arguments_edit_.Show();
  execute_arguments_help_.Show();
  start_on_background_check_.Show();
  wait_for_process_exit_check_.Show();
  delete_output_file_check_.Show();
  return true;
}

void
ParameterPropertySheet::AfterProcessPage::SetParameterToControlsBody( void )
{
  execute_after_process_check_.SetCheck( parameter_.execute_after_process_ );
  execute_file_edit_.SetText( parameter_.after_process_execute_file_ );
  execute_arguments_edit_.SetText( parameter_.after_process_execute_arguments_ );
  start_on_background_check_.SetCheck( parameter_.after_process_start_on_background_ );
  wait_for_process_exit_check_.SetCheck( parameter_.after_process_wait_for_process_exit_ );
  delete_output_file_check_.SetCheck( parameter_.after_process_delete_output_file_ );

  this->CallCommandHandler( CommandID::ExecuteAfterProcessCheck, BN_CLICKED, execute_after_process_check_.GetHandle() );
}


// -- ParameterPropertySheet ---------------------------------------------
ParameterPropertySheet::ParameterPropertySheet( Core::ConvertParameter& parameter, std::optional<std::string> individual_bms_path ) :
TtPropertySheet( false ),

individual_bms_path_( individual_bms_path ),

general_page_( parameter, *this ),
output_page_( parameter, *this ),
parser_page_( parameter ),
mixin_page_( parameter ),
audio_page_( parameter ),
after_process_page_( parameter )
{
  this->AddPage( general_page_ );
  this->AddPage( output_page_ );
  this->AddPage( parser_page_ );
  this->AddPage( mixin_page_ );
  this->AddPage( audio_page_ );
  this->AddPage( after_process_page_ );

  if ( individual_bms_path_ ) {
    // TODO Œ©’¼‚µ
    BL::Parser::Parser parser;
    parser.not_nesting_if_statement_ = true;
    try {
      bms_data_ = parser.Parse( individual_bms_path_.value() );
    }
    catch ( ... ) {
      bms_data_ = std::make_shared<BL::BmsData>();
      bms_data_->path_ = individual_bms_path_.value();
      bms_data_->most_serious_error_level_ = ErrorLevel::Internal;
    }
  }
}

bool
ParameterPropertySheet::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[individual_bms_path_ ? Image::Index::EditIndividualParameter : Image::Index::EditCommonParameter] );
  this->SetText( StrT::PPS::Title.Get() );

  return true;
}

void
ParameterPropertySheet::SetParameterToPagesControl( void )
{
  general_page_.SetParameterToControls();
  output_page_.SetParameterToControls();
  parser_page_.SetParameterToControls();
  mixin_page_.SetParameterToControls();
  audio_page_.SetParameterToControls();
  after_process_page_.SetParameterToControls();
}

void
ParameterPropertySheet::CallAtApplyOfPages( void )
{
  auto tmp = [&] ( auto& page ) {
    if ( page.control_created_ ) {
      page.GetHandlers().at_apply();
    }
  };
  tmp( general_page_ );
  tmp( output_page_ );
  tmp( parser_page_ );
  tmp( mixin_page_ );
  tmp( audio_page_ );
  tmp( after_process_page_ );
}
