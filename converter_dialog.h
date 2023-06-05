// converter_dialog.h

#pragma once

#include <optional>

#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_progress.h"
#include "tt_time.h"
#include "tt_file_stream.h"

#include "exception.h"
#include "core/converter.h"
#include "mainstay/main_squirrel_vm.h"
#include "utility_dialogs.h"


namespace BMX2WAV {
  // -- ConverterDialog --------------------------------------------------
  class ConverterDialog : public TtDialog {
  public:
    explicit ConverterDialog( const Core::ConvertParameter& parameter, bool auto_close = false );

    void SetStartPosition( POINT point );

    enum class Result : int {
      Success = 0,
      WithError,
      Aborted,
      UserAborted,
    };

    bool        IsConverting( void );
    Result      GetResult( void );
    ErrorLevel  GetMostSeriousErrorLevel( void );
    std::string GetOutputtedString( void );

    void RequestAbort( void );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;

  protected:
    virtual bool Created( void ) override;

  private:
    void ThreadInitializeAndStart( void );
    void RegisterConverterCallbacks( void );

    void OutputString( const std::string& str );
    void OutputStringWithoutLF( const std::string& str );
    void OutputToLog( const std::string& str );

    using TagName = Mainstay::SquirrelVM::TagName;

    template <TagName& tag, class... Args>
    void CallCallback( Args... args );

    template <TagName& tag, class ReturnValueType, class... Args>
    std::optional<ReturnValueType> CallCallbackAndGetReturnValueAs( Args... args );

    // -- Callback Functions ----------
    // progress
    void CallBeforeInitialize( void );
    void CallAfterInitialize( void );
    void CallBeforeParse( void );
    void CallAfterParse( void );
    void CallBeforeReadAudioFiles( void );
    void CallAfterReadAudioFiles( void );
    void CallBeforeMixinWaves( void );
    void CallAfterMixinWaves( void );
    void CallBeforeAffectWave( void );
    void CallAfterAffectWave( void );
    void CallBeforeOutputToFile( void );
    void CallAfterOutputToFile( void );
    void CallBeforeFinalize( void );
    void CallAfterFinalize( void );

    // point progress
    std::optional<std::string> CallDecideOutputFilePath( void );
    std::optional<std::string> CallDecideAudioFilePath( const std::string& filename );
    void                       CallAudioFileReadStart( BL::Word word, const std::string& path );
    void                       CallAudioFileReadEnd( BL::Word word, const std::string& path );
    void                       CallCompleteNormalize( double ratio );
    void                       CallAfterProcess( void );

    // abort
    void CallAborted( bool user_abort );

    // exception
    void CallExceptionOccurred( ConvertException& exception );
    void CallParserExceptionOccurred( ConvertBmsDescriptionException& exception );


    void SquirrelErrorHandling( std::function<void ( void )> function );
    bool SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function );

  private:
    Core::Converter                     converter_;
    std::optional<Mainstay::SquirrelVM> vm_;

    std::optional<TtFunctionThread> thread_;
    std::optional<TtFileWriter>     log_file_writer_;

    bool                       auto_close_;
    std::optional<POINT>       start_position_;
    Result                     result_;
    std::optional<std::string> outputted_log_;

    // -- BorderLabel -----
    class BorderLabel : public BasedOnTtWindow<TtEdit::Style::READONLY | TtEdit::Style::MULTILINE, WS_EX_WINDOWEDGE, TtWindowClassName::Edit> {
    public:
      static HBRUSH ReadyBrush;
      static HBRUSH RunningBrush;
      static HBRUSH CompletedBrush;
      static HBRUSH WithErrorBrush;
      static HBRUSH AbortedBrush;

      enum class State {
        Ready,
        Running,
        Completed,
        WithError,
        Aborted,
      };

      void SetState( State state );
      WMResult SetColorToHDCAndReturnWMResult( HDC hdc );

      void SetTextAreaAsStandard( void );

    private:
      State state_;
    };

    using PathLabel = TtWindowWithStyle<TtStatic, SS_PATHELLIPSIS>;

    PathLabel        path_label_;
    TtStatic         title_and_artist_albel_;
    TtStatic         result_label_;
    TtButton         abort_button_;
    TtGroup          status_group_;
    TtStatic         initialize_title_label_;
    BorderLabel      initialize_report_label_;
    TtStatic         parse_title_label_;
    BorderLabel      parse_report_label_;
    TtStatic         wav_title_label_;
    BorderLabel      wav_report_label_;
    TtProgressSmooth wav_progress_;
    TtStatic         mixin_title_label_;
    BorderLabel      mixin_report_label_;
    TtStatic         after_title_label_;
    BorderLabel      after_report_label_;
    TtButton         play_wav_button_;
    TtButton         stop_wav_button_;
    TtButton         save_as_wav_button_;
    TtButton         save_as_ogg_button_;
    TtButton         open_wav_button_;
    TtButton         show_output_box_button_;

    OutputDialog output_dialog_;

    // converting
    BorderLabel*       current_label_;
    BorderLabel::State current_report_end_state_;
    std::string        current_report_end_string_;
    void ResetCurrentLabel( BorderLabel& label );

    unsigned int wav_count_max_;
    unsigned int wav_count_;
    bool         user_abort_;
  };
}
