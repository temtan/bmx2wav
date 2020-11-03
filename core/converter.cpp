// core/converter.cpp

#include "tt_path.h"
#include "tt_string.h"

#include "utility.h"

#include "exception.h"

#include "core/converter.h"

using namespace BMX2WAV;


// -- Converter ----------------------------------------------------------
Core::Converter::Converter( const ConvertParameter& parameter ) :
parameter_( parameter ),

callbacks_(),
info_(),
squirrel_object_( std::nullopt )
{
}


const Core::ConvertParameter&
Core::Converter::GetConvertParameter( void )
{
  return parameter_;
}


void
Core::Converter::SetOutputFilePath( const std::string& path )
{
  info_.output_file_path_ = path;
}

void
Core::Converter::SetTemporaryAudioFilePath( const std::string& path )
{
  info_.temporary_audio_file_path_ = path;
}

void
Core::Converter::SetNeedToAbortImediately( bool flag )
{
  info_.need_to_abort_imediately_.store( flag );
}

bool
Core::Converter::IsConverting( void )
{
  return info_.is_converting_;
}


double
Core::Converter::GetProcessingTime( void )
{
  return TtTime::GetNow() - info_.start_time_ - info_.correction_time_;
}


bool
Core::Converter::Convert( void )
{
  TtUtility::DestructorCall finalizer( [&] ( void ) {
    this->SafeCallback<&Callbacks::before_finalize_>();
    this->Finalize();
    this->SafeCallback<&Callbacks::after_finalize_>();
  } );

  try {
    auto throw_abort_if_need = [this] ( std::function<void ( void )> func ) {
      func();
      if ( info_.need_to_abort_delay_.load() ) {
        throw AbortController();
      }
    };

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_initialize_>();
      this->Initialize();
      this->SafeCallback<&Callbacks::after_initialize_>();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_parse_>();
      this->BmsFileParse();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->BmsDataPreprocess();
      this->SafeCallback<&Callbacks::after_parse_>();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_read_audio_files_>();
      this->ReadAudioFiles();
      this->SafeCallback<&Callbacks::after_read_audio_files_>();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_mixin_waves_>();
      this->MixinWaves();
      this->SafeCallback<&Callbacks::after_mixin_waves_>();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_affect_wave_>();
      this->AffectWave();
      this->SafeCallback<&Callbacks::after_affect_wave_>();
    } );

    throw_abort_if_need( [this] ( void ) {
      this->SafeCallback<&Callbacks::before_output_to_file_>();
      this->OutputToFile();
      this->SafeCallback<&Callbacks::after_output_to_file_>();
    } );

    this->AfterProcess();
    return true;
  }
  catch ( AbortController ) {
    info_.is_aborted_ = true;
    if ( callbacks_.aborted_ ) {
      callbacks_.aborted_( *this );
    }
  }
  return false;
}

void
Core::Converter::Initialize( void )
{
  info_.bms_data_ = nullptr;

  info_.bpm_ = 120.0;
  info_.extended_bpm_table_.clear();
  info_.stop_sequence_table_.clear();
  info_.lnobj_word_ = BL::Word::MIN;

  info_.wave_table_.clear();

  info_.wave_ = nullptr;

  info_.exceptions_.clear();
  info_.most_serious_error_level_ = ErrorLevel::None;

  info_.start_time_ = TtTime::GetNow();
  info_.correction_time_ = 0.0;

  info_.need_to_abort_imediately_.store( false );
  info_.need_to_abort_delay_.store( false );
  info_.is_converting_.store( true );
  info_.is_aborted_ = false;
}

void
Core::Converter::BmsFileParse( void )
{
  BL::Parser::Parser parser;
  parser.not_nesting_if_statement_ = parameter_.not_nesting_if_statement_;
  parser.must_abort_error_level_   = parameter_.immediately_abort_error_level_;
  parser.bar_resolution_max_       = parameter_.bar_resolution_max_;

  parser.callbacks_.resolution_extended_ = [usable_memory_mega_byte_size = parameter_.usable_memory_mega_byte_size_] ( BL::Parser::Parser& parser, BL::Parser::Channel& channel, unsigned int new_resolution ) {
    auto use_size = Utility::GetCurrentProcessWorkingSetSize();
    if ( usable_memory_mega_byte_size != 0 && use_size / 1000 / 1000 > usable_memory_mega_byte_size ) {
      std::string message = "使用メモリが設定された限界を超えました。実使用メモリ : " + TtUtility::ToStringFrom( use_size / 1000 / 1000 ) + "MB. 要求分解能 : " + TtUtility::ToStringFrom( new_resolution );
      parser.CauseErrorWithoutCallback( std::make_shared<BL::Parser::BmsDescriptionCustomMessageException>( *channel.raw_line_, ErrorLevel::ImmediatelyAbort, message ) );
    }
  };
  parser.callbacks_.exception_occurred_for_converter_ = [&] ( BL::Parser::Parser&, BL::Parser::BmsDescriptionException& exception ) {
    this->SafeErrorCallback<&Callbacks::parser_exception_occurred_>( std::ref( exception ) );
  };

  this->SafeCallback<&Callbacks::just_before_parse_>( std::ref( parser ) );
  try {
    info_.bms_data_ = parser.Parse( parameter_.input_file_path_ );
    if ( info_.bms_data_->most_serious_error_level_.ToValue() < info_.most_serious_error_level_.ToValue() ) {
      info_.most_serious_error_level_ = info_.bms_data_->most_serious_error_level_;
    }
    if ( info_.need_to_abort_imediately_.load() ) {
      throw AbortController();
    }
  }
  catch ( BL::Parser::FileAccessException& ex ) {
    this->SafeErrorCallback<&Callbacks::bms_file_access_error_>( ex.GetPath(), ex.GetErrorNumber() );
    throw AbortController();
  }
  catch ( std::bad_alloc e ) {
    this->SafeErrorCallback<&Callbacks::bad_allocation_error_>( e );
    throw AbortController();
  }
  this->SafeCallback<&Callbacks::just_after_parse_>( std::ref( parser ) );
}


void
Core::Converter::BmsDataPreprocess( void )
{
  // BPM の読み込み
  if ( auto it = info_.bms_data_->headers_.find( "BPM" ); it != info_.bms_data_->headers_.end() ) {
    if ( NOT( TtUtility::StringToDouble( it->second, &info_.bpm_ ) ) || info_.bpm_ == 0.0 ) {
      this->SafeErrorCallback<&Callbacks::invalid_format_as_bpm_header_>();
      this->PushExceptionMessage( "BPM はデフォルト値の 120 を使用します。" );
      info_.bpm_ = 120.0;
    }
  }
  else {
    this->PushExceptionMessage( "BPM が指定されなかった為、デフォルト値の 120 を使用します。" );
    info_.bpm_ = 120.0;
  }

  // 拡張 BPM, STOP sequence の読み込み
  for ( BL::Word i( 0 ); i < Const::WORD_MAX_COUNT; i.Increase() ) {
    if ( info_.bms_data_->extended_bpm_array_.IsExists( i ) ) {
      if ( double tmp; NOT( TtUtility::StringToDouble( info_.bms_data_->extended_bpm_array_[i], &tmp ) ) || tmp == 0.0 ) {
        this->SafeErrorCallback<&Callbacks::invalid_format_as_extended_bpm_change_value_>( i );
        this->PushExceptionMessage( "BPM%s は指定されなかったとします。", i.ToCharPointer() );
      }
      else {
        info_.extended_bpm_table_[i] = tmp;
      }
    }
    if ( info_.bms_data_->stop_sequence_array_.IsExists( i ) ) {
      if ( int tmp; NOT( TtUtility::StringToInteger( info_.bms_data_->stop_sequence_array_[i], &tmp ) ) ) {
        this->SafeErrorCallback<&Callbacks::invalid_format_as_stop_sequence_>( i );
        if ( double tmp_double; NOT( TtUtility::StringToDouble( info_.bms_data_->stop_sequence_array_[i], &tmp_double ) ) ) {
          this->PushExceptionMessage( "STOP%s は指定されなかったとします。", i.ToCharPointer() );
        }
        else {
          this->PushExceptionMessage( "STOP%s に小数が指定されました。仕様上は整数のみ指定可能ですが指定された小数を使います。", i.ToCharPointer() );
          info_.stop_sequence_table_[i] = tmp_double;
        }
      }
      else {
        info_.stop_sequence_table_[i] = static_cast<double>( tmp );
      }
    }
  }

  // LNOBJ の検証
  if ( auto it = info_.bms_data_->headers_.find( "LNOBJ" ); it != info_.bms_data_->headers_.end() ) {
    std::string tmp = it->second;
    if ( tmp.size() != 2 || NOT( BL::Word::CanConstructAsWord( tmp ) ) || BL::Word( tmp ) == BL::Word::MIN ) {
      this->SafeErrorCallback<&Callbacks::invalid_format_as_lnobj_>();
      this->PushExceptionMessage( "LNOBJ は指定されなかったとします。" );
    }
    else {
      info_.lnobj_word_ = BL::Word( tmp );
    }
  }

  // 出力 BMS ファイルパスの決定
  this->SafeCallback<&Callbacks::decide_output_file_path_>();
}


void
Core::Converter::ReadAudioFiles( void )
{
  std::string bms_dir = TtPath::DirName( parameter_.input_file_path_ );
  Core::WaveMaker wave_maker( true );

  for ( BL::Word i( 0 ); i < Const::WORD_MAX_COUNT; i.Increase() ) {
    if ( info_.bms_data_->wav_array_.IsExists( i ) ) {
      this->SafeCallback<&Callbacks::one_audio_file_read_>();

      this->SafeCallback<&Callbacks::decide_audio_file_path_>( info_.bms_data_->wav_array_[i] );
      std::string path = info_.temporary_audio_file_path_;

      if ( NOT( TtPath::FileExists( path ) ) ) {
        this->SafeErrorCallback<&Callbacks::entried_audio_file_not_found_>( i, path );
        this->PushExceptionMessage( "WAV%s は指定されなかったとします。", i.ToCharPointer() );
        continue;
      }

      if ( TtPath::AreTheseSamePath( path, info_.output_file_path_ ) ) {
        this->SafeErrorCallback<&Callbacks::output_file_is_input_file_path_>( i, path );
        this->PushExceptionMessage( "WAV%s は指定されなかったとします。", i.ToCharPointer() );
        continue;
      }

      try {
        this->SafeCallback<&Callbacks::audio_file_read_start_>( i, path );
        if ( TtString::EndWith( TtString::ToLower( path ), ".ogg" ) ) {
          info_.wave_table_[i] = wave_maker.MakeNewWaveFromOggFile( path );
        }
        else {
          info_.wave_table_[i] = wave_maker.MakeNewWaveFromWavFile( path );
        }
        this->SafeCallback<&Callbacks::audio_file_read_end_>( i, path );
      }
      catch ( AudioFileError& ex ) {
        this->SafeErrorCallbackOf( ex.ToSharedPointer(), callbacks_.wav_file_access_error_ );
        this->PushExceptionMessage( "WAV%s には空の音声ファイルが指定されたとします。", i.ToCharPointer() );
        info_.wave_table_[i] = std::make_shared<Core::Wave>();
      }
    }
  }
}


void
Core::Converter::MixinWaves( void )
{
  std::vector<int> last_used_wave_positions( Const::WORD_MAX_COUNT );
  for ( auto& tmp : info_.wave_table_ ) {
    last_used_wave_positions[tmp.first] = (-1) * static_cast<int>( tmp.second->GetPositiveLength() );
  }

  std::unordered_map<BL::Word, BL::ObjectWithLocation> on_long_note_table;
  unsigned int convert_start_bar = parameter_.convert_start_bar_number_;
  unsigned int convert_end_bar   = std::max( parameter_.convert_end_bar_number_, info_.bms_data_->GetBarNumberLastObjectExists() );
  info_.wave_ = std::make_shared<Core::Wave>();
  double wave_position = 0.0;
  unsigned int triming_start_wave_position = std::numeric_limits<unsigned int>::max();
  unsigned int triming_end_wave_position   = std::numeric_limits<unsigned int>::max();

  for ( unsigned int current_bar_number = 0; current_bar_number <= convert_end_bar; ++current_bar_number ) {
    BL::Bar& current_bar = info_.bms_data_->bars_[current_bar_number];

    if ( current_bar_number == parameter_.triming_start_bar_number_ ) {
      triming_start_wave_position = static_cast<int>( wave_position );
    }
    if ( current_bar_number < convert_start_bar ) {
      continue;
    }

    for ( unsigned int position_of_bar = 0; position_of_bar < current_bar.GetResolution(); ++position_of_bar ) {
      const int int_wave_position = static_cast<int>( wave_position );
      double current_stop_sequence = 0.0;

      for ( BL::Channel& current_channel : current_bar ) {
        BL::Word current_word = current_channel[position_of_bar];
        if ( current_word == BL::Word::MIN ) {
          continue;
        }

        if ( current_channel.IsShoudPlayChannel() ) {
          if ( current_channel.IsLongNoteChannel() ) {
            if ( on_long_note_table[current_channel.GetChannelNumber()].object_number_ == BL::Word::MIN ) {
              on_long_note_table[current_channel.GetChannelNumber()] = {current_bar_number, current_channel.GetChannelNumber(), current_word};
            }
            else {
              auto tmp = on_long_note_table[current_channel.GetChannelNumber()];
              if ( current_word != tmp.object_number_ ) {
                this->SafeErrorCallback<&Callbacks::long_note_object_invalid_enclose_>( tmp, BL::ObjectWithLocation( current_bar_number, current_channel.GetChannelNumber(), current_word ) );
              }
              on_long_note_table.erase( current_channel.GetChannelNumber() );
              continue;
            }
          }

          if ( current_channel.IsPlayerNoteChannel() && current_word == info_.lnobj_word_ ) {
            // LNOBJ 指定 WAV は鳴らさない
          }
          else if ( auto it = info_.wave_table_.find( current_word ); it == info_.wave_table_.end() ) {
            this->SafeErrorCallback<&Callbacks::not_entried_wav_was_used_>( BL::ObjectWithLocation( current_bar_number, current_channel.GetChannelNumber(), current_word ) );
          }
          else {
            // 同じ WAV が鳴ったとき前のを消す
            if ( parameter_.cancel_at_resounding_ ) {
              if ( int_wave_position - last_used_wave_positions[current_word] < info_.wave_table_[current_word]->GetPositiveLength() ) {
                info_.wave_->DeductAt( int_wave_position, *info_.wave_table_[current_word], int_wave_position - last_used_wave_positions[current_word] );
              }
            }
            info_.wave_->MixinAt( int_wave_position, *info_.wave_table_[current_word] );
            last_used_wave_positions[current_word] = int_wave_position;
          }
        }
        // BPM変更
        else if ( current_channel.IsBpmChangeChannel() ) {
          if ( int new_bpm; NOT( TtUtility::StringToInteger( current_word.ToCharPointer(), &new_bpm, 16 ) ) ) {
            this->SafeErrorCallback<&Callbacks::invalid_format_as_bpm_change_value_>( current_bar_number, current_word );
            this->PushExceptionMessage( "BPM は変更されなかったとします。 小節番号 : %d", current_bar_number );
          }
          else {
            info_.bpm_ = static_cast<double>( new_bpm );
          }
        }
        // 拡張BPM変更
        else if ( current_channel.IsExtendedBpmChangeChannel() ) {
          if ( auto it = info_.extended_bpm_table_.find( current_word ); it == info_.extended_bpm_table_.end() ) {
            this->SafeErrorCallback<&Callbacks::extended_bpm_change_entry_not_exist_>( current_bar_number, current_word );
            this->PushExceptionMessage( "BPM は変更されなかったとします。 小節番号 : %d  オブジェクト : %s", current_bar_number, current_word.ToCharPointer() );
          }
          else {
            info_.bpm_ = it->second;
          }
        }
        // STOP sequence
        else if ( current_channel.IsStopSequenceChannel() ) {
          if ( auto it = info_.stop_sequence_table_.find( current_word ); it == info_.stop_sequence_table_.end() ) {
            // this->SafeErrorCallbackOf<StopSequenceEntryNotExistException>( &Callbacks::stop_sequence_entry_not_exist_, current_bar_number, current_word );
            this->SafeErrorCallback<&Callbacks::stop_sequence_entry_not_exist_>( current_bar_number, current_word );
            this->PushExceptionMessage( "ストップシーケンスはなかったとします。 小節番号 : %d  オブジェクト : %s", current_bar_number, current_word.ToCharPointer() );
          }
          else {
            current_stop_sequence = info_.stop_sequence_table_[current_word];
          }
        }

      } // end of each channel
      wave_position += ( (static_cast<double>( Core::Wave::FREQUENCY * 60 ) / info_.bpm_) /
                         (static_cast<double>( current_bar.GetResolution() ) / 4.0) *
                         current_bar.GetRatio() );
      if ( current_stop_sequence != 0.0 ) {
        wave_position += ( (static_cast<double>( Core::Wave::FREQUENCY * 60 ) / info_.bpm_) *
                           (current_stop_sequence / 192.0 * 4.0 ) );
      }
    } // end of each position

    if ( current_bar_number == parameter_.triming_end_bar_number_ ) {
      triming_end_wave_position = static_cast<int>( wave_position );
    }
  } // end of each bar

  for ( auto& one : on_long_note_table ) {
    if ( one.second.object_number_ != BL::Word::MIN ) {
      this->SafeErrorCallback<&Callbacks::long_note_object_not_enclosed_>( one.second );
    }
  }

  // トリミング
  if ( parameter_.do_triming_ ) {
    int start = std::min( info_.wave_->GetPositiveLength(), triming_start_wave_position );
    int end = std::min( info_.wave_->GetPositiveLength(), triming_end_wave_position );
    if ( start < end ) {
      info_.wave_->Trim( start, end );
    }
  }
}


void
Core::Converter::AffectWave( void )
{
  if ( parameter_.remove_front_silence_ ) {
    info_.wave_->TrimFrontSilence();
  }
  if ( parameter_.insert_front_silence_ ) {
    info_.wave_->InsertFrontSilenceBySecond( parameter_.insert_front_silence_second_ );
  }

  // ノーマライズ
  switch ( parameter_.normalize_kind_.ToValue() ) {
  case Parameter::Normalize::Peak: {
    double changed_ratio = info_.wave_->PeakNormalize();
    this->SafeCallback<&Callbacks::complete_normalize_>( changed_ratio );
    break;
  }

  case Parameter::Normalize::Average: {
    double changed_ratio = info_.wave_->AverageNormalize();
    this->SafeCallback<&Callbacks::complete_normalize_>( changed_ratio );
    break;
  }

  case Parameter::Normalize::Default:
  case Parameter::Normalize::Over: {
    double over_ratio = static_cast<double>( parameter_.normalize_over_ppm_ ) * 0.000001;
    double changed_ratio = info_.wave_->OverNormalize( over_ratio );
    this->SafeCallback<&Callbacks::complete_normalize_>( changed_ratio );
    break;
  }

  case Parameter::Normalize::None:
  default:
    break;
  }

  // 音量調整
  if ( parameter_.volume_ != 100 ) {
    info_.wave_->ChangeVolume( static_cast<double>( parameter_.volume_ ) / 100.0 );
  }
}

void
Core::Converter::OutputToFile( void )
{
  if ( NOT( info_.output_file_path_.empty() ) ) {
    try {
      if ( parameter_.output_as_ogg_ ) {
        info_.wave_->WriteToFileAsOgg( info_.output_file_path_, static_cast<float>( parameter_.ogg_base_quality_ ) );
      }
      else {
        info_.wave_->WriteToFile( info_.output_file_path_ );
      }
    }
    catch ( AudioFileOpenException& exception ) {
      this->SafeErrorCallback<&Callbacks::output_file_access_error_>( std::ref( exception ) );
      throw AbortController();
    }
  }
}

void
Core::Converter::AfterProcess( void )
{
  this->SafeCallback<&Callbacks::after_process_>();
}


void
Core::Converter::Finalize( void )
{
  info_.is_converting_.store( false );
}

void
Core::Converter::Dispose( void )
{
  info_.bms_data_ = nullptr;

  info_.extended_bpm_table_.clear();
  info_.stop_sequence_table_.clear();

  info_.wave_table_.clear();

  info_.wave_ = nullptr;

  info_.exceptions_.clear();
}


namespace {
  template <class T> struct ExceptionOf {};
  template <class T>
  struct ExceptionOf<std::function<void ( Core::Converter&, T& )> Core::Converter::Callbacks::*> {
    using Type = typename T;
  };
}

template <class Exception>
void
Core::Converter::SafeErrorCallbackOf( std::shared_ptr<Exception> exception, Callbacks::FunctionOf<Exception&> f )
{
  info_.exceptions_.push_back( exception );

  if ( exception->GetErrorLevel().ToValue() < info_.most_serious_error_level_.ToValue() ) {
    info_.most_serious_error_level_ = exception->GetErrorLevel();
  }

  if ( callbacks_.exception_occurred_ ) {
    callbacks_.exception_occurred_( *this, *exception );
  }
  if ( f ) {
    f( *this, *exception );
  }
  if ( exception->GetErrorLevel().ToInteger() <= parameter_.immediately_abort_error_level_.ToInteger() ) {
    info_.need_to_abort_imediately_.store( true );
  }
  if ( exception->GetErrorLevel().ToInteger() <= parameter_.delay_abort_error_level_.ToInteger() ) {
    info_.need_to_abort_delay_.store( true );
  }
  if ( info_.need_to_abort_imediately_.load() ) {
    throw AbortController();
  }
}

template <auto Core::Converter::Callbacks::* callback, class... Args>
void
Core::Converter::SafeErrorCallback( Args... args )
{
  auto exception = std::make_shared<ExceptionOf<decltype( callback )>::Type>( args... );
  this->SafeErrorCallbackOf( exception, callbacks_.*callback );
}


template <auto Core::Converter::Callbacks::* callback, class... Args>
void
Core::Converter::SafeCallback( Args... args )
{
  if ( callbacks_.*callback ) {
    (callbacks_.*callback)( *this, args... );
  }
  if ( NOT( info_.is_aborted_ ) && info_.need_to_abort_imediately_.load() ) {
    throw AbortController();
  }
}

template <class... Args>
void
Core::Converter::PushExceptionMessage( const std::string& format, Args... args )
{
  std::string message = Exception::MakeMessage( format.c_str(), args... );
  this->SafeErrorCallback<&Callbacks::message_only_exception_>( message );
}
