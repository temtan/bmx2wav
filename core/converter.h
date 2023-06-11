// core/converter.h

#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <atomic>

#include "tt_time.h"

#include "tt_squirrel_virtual_machine.h"

#include "base/word.h"
#include "base/bms_data.h"
#include "base/parser.h"
#include "base/bmson_parser.h"
#include "core/convert_parameter.h"
#include "core/wave.h"
#include "core/wave_maker.h"
#include "exception.h"


namespace BMX2WAV::Core {
  // -- Converter --------------------------------------------------------
  class Converter {
  public:
    explicit Converter( const ConvertParameter& parameter );

    const ConvertParameter& GetConvertParameter( void );

    void SetOutputFilePath( const std::string& path );
    void SetTemporaryAudioFilePath( const std::string& path );
    void SetNeedToAbortImediately( bool flag );
    bool IsConverting( void );

    double GetProcessingTime( void );

    bool Convert( void );

  private:
    void Initialize( void );
    void BmsFileParse( void );
    void BmsDataPreprocess( void );
    void ReadAudioFiles( void );
    void MixinWaves( void );
    void AffectWave( void );
    void OutputToFile( void );
    void AfterProcess( void );
    void Finalize( void );

    void Dispose( void );

  public:
    // for Error
    struct AbortController {};

    struct Callbacks {
      template <class... Args>
      using FunctionOf = std::function<void ( Converter&, Args... )>;

      // progress
      FunctionOf<> before_initialize_;
      FunctionOf<> after_initialize_;
      FunctionOf<> before_parse_;
      FunctionOf<> after_parse_;
      FunctionOf<> before_read_audio_files_;
      FunctionOf<> after_read_audio_files_;
      FunctionOf<> before_mixin_waves_;
      FunctionOf<> after_mixin_waves_;
      FunctionOf<> before_affect_wave_;
      FunctionOf<> after_affect_wave_;
      FunctionOf<> before_output_to_file_;
      FunctionOf<> after_output_to_file_;
      FunctionOf<> before_finalize_;
      FunctionOf<> after_finalize_;

      // point progress
      FunctionOf<BL::Parser::Parser&>           just_before_parse_;
      FunctionOf<BL::Parser::Parser&>           just_after_parse_;
      FunctionOf<>                              decide_output_file_path_;
      FunctionOf<>                              one_audio_file_read_;
      FunctionOf<const std::string&>            decide_audio_file_path_;
      FunctionOf<BL::Word, const std::string&>  audio_file_read_start_;
      FunctionOf<BL::Word, const std::string&>  audio_file_read_end_;
      FunctionOf<double>                        complete_normalize_;
      FunctionOf<>                              after_process_;

      // abort
      FunctionOf<> aborted_;

      // exception
      FunctionOf<ConvertException&>                               exception_occurred_;
      FunctionOf<ConvertBmsDescriptionException&>                 parser_exception_occurred_;

      FunctionOf<MessageOnlyException&>                           message_only_exception_;
      FunctionOf<BmsFileAccessException&>                         bms_file_access_error_;
      FunctionOf<ConvertBmsonException&>                          bmson_error_;
      FunctionOf<BadAllocationException&>                         bad_allocation_error_;
      FunctionOf<EntriedAudioFileNotFoundException&>              entried_audio_file_not_found_;
      FunctionOf<AudioFileError&>                                 wav_file_access_error_;
      FunctionOf<OutputFileIsInputFilePathException&>             output_file_is_input_file_path_;
      FunctionOf<InvalidFormatAsBpmHeaderException&>              invalid_format_as_bpm_header_;
      FunctionOf<InvalidFormatAsExtendedBpmChangeValueException&> invalid_format_as_extended_bpm_change_value_;
      FunctionOf<InvalidFormatAsStopSequenceException&>           invalid_format_as_stop_sequence_;
      FunctionOf<InvalidFormatAsLongNoteObjectHeaderException&>   invalid_format_as_lnobj_;
      FunctionOf<LongNoteObjectInvalidEncloseException&>          long_note_object_invalid_enclose_;
      FunctionOf<NotEntriedWavWasUsedException&>                  not_entried_wav_was_used_;
      FunctionOf<InvalidFormatAsBpmChangeValueException&>         invalid_format_as_bpm_change_value_;
      FunctionOf<ExtendedBpmChangeEntryNotExistException&>        extended_bpm_change_entry_not_exist_;
      FunctionOf<StopSequenceEntryNotExistException&>             stop_sequence_entry_not_exist_;
      FunctionOf<LongNoteObjectNotEnclosedException&>             long_note_object_not_enclosed_;
      FunctionOf<OutputFileAccessException&>                      output_file_access_error_;
    };

  private:
    template <class Exception>
    void SafeErrorCallbackOf( std::shared_ptr<Exception> exception, Callbacks::FunctionOf<Exception&> f );

    template <auto Callbacks::* callback, class... Args>
    void SafeErrorCallback( Args... args );

    template <auto Callbacks::* callback, class... Args>
    void SafeCallback( Args... args );

    template <class... Args>
    void PushExceptionMessage( const std::string& format, Args... args );

  public:
    struct ConvertingInformation {
      std::shared_ptr<BL::BmsData> bms_data_;
      std::string                  output_file_path_;

      double                               bpm_;
      std::unordered_map<BL::Word, double> extended_bpm_table_;
      std::unordered_map<BL::Word, double> stop_sequence_table_;
      BL::Word                             lnobj_word_;

      std::string                                               temporary_audio_file_path_;
      std::unordered_map<BL::Word, std::shared_ptr<Core::Wave>> wave_table_;

      std::shared_ptr<Core::Wave> wave_;

      std::vector<std::shared_ptr<ConvertException>> exceptions_;
      TtEnum<ErrorLevel>                             most_serious_error_level_;

      TtTime start_time_;
      double correction_time_;

    private:
      friend class Converter;
      std::atomic_bool need_to_abort_imediately_;
      std::atomic_bool need_to_abort_delay_;
      std::atomic_bool is_converting_;
      bool             is_aborted_;
    };

  private:
    ConvertParameter                  parameter_;

  public:
    Callbacks                         callbacks_;
    ConvertingInformation             info_;
    std::optional<TtSquirrel::Object> squirrel_object_;
  };
}
