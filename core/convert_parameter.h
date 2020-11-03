// core/convert_parameter.h

#pragma once

#include "tt_enum.h"
#include "tt_dialog.h"
#include "tt_list_view.h"

#include "base/bms_data.h"

#include "common.h"

#undef ERROR
#undef IGNORE


namespace BMX2WAV::Core {
  namespace Parameter {
    // -- Normalize ------------------------------------------------------
    enum class Normalize {
      Default,
      None,
      Peak,
      Average,
      Over,
    };
  }

  // -- ConvertParameter -------------------------------------------------
  class ConvertParameter {
  public:
    explicit ConvertParameter( void );

    void ReadFromFile( const std::string& path );
    void WriteToFile( const std::string& path ) const;

  public:
    // 特殊
    std::string input_file_path_;

    // 全般
    std::string name_;
    bool        use_script_file_;
    std::string script_file_path_;
    bool        output_log_;
    std::string log_file_path_;

    // 出力
    bool        output_as_ogg_;
    double      ogg_base_quality_;
    bool        never_overwrite_output_file_;
    bool        remove_can_not_use_character_as_file_path_;
    std::string output_file_template_;

    // BMS
    bool ignore_bga_channel_;
    bool not_nesting_if_statement_;

    // 変換
    bool         cancel_at_resounding_;
    unsigned int convert_start_bar_number_;
    unsigned int convert_end_bar_number_;
    bool         do_triming_;
    unsigned int triming_start_bar_number_;
    unsigned int triming_end_bar_number_;
    bool         remove_front_silence_;
    bool         insert_front_silence_;
    double       insert_front_silence_second_;

    // 音量
    TtEnum<Parameter::Normalize> normalize_kind_;
    unsigned int                 normalize_over_ppm_;
    int                          volume_;

    // 後処理
    bool        execute_after_process_;
    std::string after_process_execute_file_;
    std::string after_process_execute_arguments_;
    bool        after_process_start_on_background_;
    bool        after_process_wait_for_process_exit_;
    bool        after_process_delete_output_file_;

    // 制御
    TtEnum<ErrorLevel> immediately_abort_error_level_;
    TtEnum<ErrorLevel> delay_abort_error_level_;
    unsigned int       bar_resolution_max_;
    unsigned int       usable_memory_mega_byte_size_;
  };


  // -- 出力テンプレート変換関数 -----
  std::string TranslateTemplatePath( const ConvertParameter& parameter, BL::BmsData* bms_data );
  std::string TranslateTemplatePath( const std::string& template_path,
                                     const std::string& input_path,
                                     bool               output_as_ogg,
                                     BL::BmsData*       bms_data );

  // -- 出力テンプレート変換一覧用ダイアログ
  // -- TemplatePathTranslateHelpDialog -----
  class TemplatePathTranslateHelpDialog : public TtDialogModeless {
  public:
    explicit TemplatePathTranslateHelpDialog( void );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

  private:
    using HelpList = TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS>;
    HelpList list_;

    TtSubMenu menu_;
  };
}
