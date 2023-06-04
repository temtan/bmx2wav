// parameter_property_sheet.h

#pragma once

#include "tt_property_sheet.h"
#include "tt_window_controls.h"
#include "tt_dialog.h"

#include "base/bms_data.h"

#include "core/convert_parameter.h"
#include "mainstay/entry.h"


namespace BMX2WAV {
  // -- ParameterPropertySheet -------------------------------------------
  class ParameterPropertySheet : public TtPropertySheet {
  private:
    struct CommandID {
      enum ID : int {
        LoadButton = 11001,
        SaveButton,
        UseScriptCheck,
        ScriptReferenceButton,
        OutputLogCheck,
        LogReferenceButton,
        OutputAsOggCheck,
        OutputTemplateEdit,
        OutputTemplateHelpButton,
        DoTrimingCheck,
        InsertFrontSilenceCheck,
        NormalizeKindNoneRadio,
        NormalizeKindPeakRadio,
        NormalizeKindAverageRadio,
        NormalizeKindOverRadio,

        ExecuteAfterProcessCheck,
        ExecuteFileReferenceButton,
      };
    };

    // -- ì¬Ïƒtƒ‰ƒOŠÇ——p
    class WithCreatedFlag {
    public:
      explicit WithCreatedFlag( void ) : control_created_( false ) {}

      void SetParameterToControls( void ) {
        if ( control_created_ ) {
          this->SetParameterToControlsBody();
        }
      }
      virtual void SetParameterToControlsBody( void ) = 0;

      bool control_created_;
    };

    // -- GeneralPage --------------------------------------------------
    class GeneralPage : public Page,
                        public WithCreatedFlag {
    public:
      explicit GeneralPage( Core::ConvertParameter& parameter, ParameterPropertySheet& parent );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;
      ParameterPropertySheet& parent_;

      TtGroup    load_save_group_;
      TtButton   load_button_;
      TtButton   save_button_;
      TtStatic   name_label_;
      TtEdit     name_edit_;
      TtCheckBox use_script_check_;
      TtStatic   script_label_;
      TtEdit     script_edit_;
      TtButton   script_ref_button_;
      TtCheckBox output_log_check_;
      TtStatic   log_label_;
      TtEdit     log_edit_;
      TtButton   log_ref_button_;
    };

    // -- OutputPage -----------------------------------------------------
    class OutputPage : public Page,
                       public WithCreatedFlag {
    public:
      explicit OutputPage( Core::ConvertParameter& parameter, ParameterPropertySheet& parent );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;
      ParameterPropertySheet& parent_;

      Core::TemplatePathTranslateHelpDialog output_file_help_dialog_;

      using ResultEdit = TtEditWithStyle<TtEdit::Style::READONLY>;

      TtCheckBox output_as_ogg_check_;
      TtStatic   ogg_base_quality_label_;
      TtEdit     ogg_base_quality_edit_;
      TtCheckBox never_overwrite_check_;
      TtCheckBox remove_char_check_;
      TtStatic   output_file_template_label_;
      TtButton   output_file_help_button_;
      TtEdit     output_file_template_edit_;
      TtStatic   output_file_template_input_bms_label_;
      ResultEdit output_file_template_input_bms_edit_;
      TtStatic   output_file_template_result_label_;
      ResultEdit output_file_template_result_edit_;

      TtFont     font_for_output_template_edit_;
    };

    // -- ParserPage -----------------------------------------------------
    class ParserPage : public Page,
                       public WithCreatedFlag {
    public:
      explicit ParserPage( Core::ConvertParameter& parameter );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;

      TtCheckBox read_as_utf8_check_;
      TtCheckBox ignore_bga_channel_check_;
      TtCheckBox not_nesting_if_statement_check_;
    };

    // -- MixinPage ------------------------------------------------------
    class MixinPage : public Page,
                      public WithCreatedFlag {
    public:
      explicit MixinPage( Core::ConvertParameter& parameter );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;

      TtCheckBox   cancel_at_resounding_check_;
      TtStatic     start_label_;
      TtStatic     end_label_;
      TtStatic     convert_start_end_bar_number_label_;
      TtEditNumber convert_start_bar_number_edit_;
      TtEditNumber convert_end_bar_number_edit_;
      TtCheckBox   do_triming_check_;
      TtEditNumber triming_start_bar_number_edit_;
      TtEditNumber triming_end_bar_number_edit_;
      TtCheckBox   remove_front_silence_check_;
      TtCheckBox   insert_front_silence_check_;
      TtStatic     insert_front_silence_second_label_;
      TtEdit       insert_front_silence_second_edit_;
    };

    // -- AudioPage ------------------------------------------------------
    class AudioPage : public Page,
                      public WithCreatedFlag {
    public:
      explicit AudioPage( Core::ConvertParameter& parameter );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;

      TtRadioButton normalize_kind_none_radio_;
      TtRadioButton normalize_kind_peak_radio_;
      TtRadioButton normalize_kind_average_radio_;
      TtRadioButton normalize_kind_over_radio_;
      TtGroup       normalize_kind_group_;
      TtStatic      normalize_over_ppm_label_;
      TtEditNumber  normalize_over_ppm_edit_;
      TtStatic      volume_label_;
      TtEditNumber  volume_edit_;
    };

    // -- AfterProcessPage -----------------------------------------------
    class AfterProcessPage : public Page,
                             public WithCreatedFlag {
    public:
      explicit AfterProcessPage( Core::ConvertParameter& patameter );

      virtual bool Created( void ) override;
      virtual void SetParameterToControlsBody( void ) override;

    private:
      Core::ConvertParameter& parameter_;

      TtCheckBox execute_after_process_check_;
      TtStatic   execute_file_label_;
      TtEdit     execute_file_edit_;
      TtButton   execute_file_ref_button_;
      TtStatic   execute_arguments_label_;
      TtEdit     execute_arguments_edit_;
      TtStatic   execute_arguments_help_;
      TtCheckBox start_on_background_check_;
      TtCheckBox wait_for_process_exit_check_;
      TtCheckBox delete_output_file_check_;
    };

    // -- ParameterPropertySheet
  public:
    explicit ParameterPropertySheet( Core::ConvertParameter& parameter, std::optional<std::string> individual_bms_path );

    virtual bool Created( void ) override;
    void SetParameterToPagesControl( void );
    void CallAtApplyOfPages( void );

  public:
    // if nullopt then common parameter
    std::optional<std::string>   individual_bms_path_;
    std::shared_ptr<BL::BmsData> bms_data_;

  private:
    GeneralPage      general_page_;
    OutputPage       output_page_;
    ParserPage       parser_page_;
    MixinPage        mixin_page_;
    AudioPage        audio_page_;
    AfterProcessPage after_process_page_;
  };
}
