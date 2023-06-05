// converter_dialog.cpp

#include "tt_string.h"
#include "tt_enum.h"
#include "tt_message_box.h"
#include "tt_file_dialog.h"
#include "tt_path.h"

#include "random_statement_dialog.h"
#include "string_table.h"
#include "utility.h"

#include "converter_dialog.h"

using namespace BMX2WAV;


// -- ConverterDialog ----------------------------------------------------
// -- Enums --------------------------------------------------------------
template <>
TtEnumTable<ConverterDialog::Result>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( ConverterDialog::Result::NAME, #NAME )
  REGISTER( Success );
  REGISTER( WithError );
  REGISTER( Aborted );
  REGISTER( UserAborted );
#undef REGISTER
}

TtEnumTable<ConverterDialog::Result>
TtEnumBase<ConverterDialog::Result>::TABLE;


ConverterDialog::ConverterDialog( const Core::ConvertParameter& parameter, bool auto_close ) :
converter_( parameter ),
auto_close_( auto_close ),
result_( Result::Success )
{
}

void
ConverterDialog::SetStartPosition( POINT point )
{
  start_position_.emplace( point );
}


bool
ConverterDialog::IsConverting( void )
{
  return converter_.IsConverting();
}

ConverterDialog::Result
ConverterDialog::GetResult( void )
{
  return result_;
}

ErrorLevel
ConverterDialog::GetMostSeriousErrorLevel( void )
{
  return converter_.info_.most_serious_error_level_;
}

std::string
ConverterDialog::GetOutputtedString( void )
{
  if ( outputted_log_ ) {
    return outputted_log_.value();
  }
  return output_dialog_.GetTextWithoutCR();
}


void
ConverterDialog::RequestAbort( void )
{
  if ( this->IsConverting() ) {
    user_abort_ = true;
    converter_.SetNeedToAbortImediately( true );
  }
}


DWORD
ConverterDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_SYSMENU;
}

DWORD
ConverterDialog::GetExtendedStyle( void )
{
  return 0;
}


bool
ConverterDialog::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( StrT::Convert::Title.Get() );

  struct CommandID {
    enum ID : int {
      AbortButton = 10001,
      PlayWavButton,
      StopWavButton,
      SaveAsWavButton,
      SaveAsOggButton,
      OpenWavButton,
      ShowOutputButton,
    };
  };

  path_label_.Create( {this} );
  title_and_artist_albel_.Create( {this} );
  result_label_.Create( {this} );
  abort_button_.Create( {this, CommandID::AbortButton} );
  status_group_.Create( {this} );
  initialize_title_label_.Create( {this} );
  initialize_report_label_.Create( {this} );
  parse_title_label_.Create( {this} );
  parse_report_label_.Create( {this} );
  wav_title_label_.Create( {this} );
  wav_report_label_.Create( {this} );
  wav_progress_.Create( {this} );
  mixin_title_label_.Create( {this} );
  mixin_report_label_.Create( {this} );
  after_title_label_.Create( {this} );
  after_report_label_.Create( {this} );
  play_wav_button_.Create( {this, CommandID::PlayWavButton} );
  stop_wav_button_.Create( {this, CommandID::StopWavButton} );
  save_as_wav_button_.Create( {this, CommandID::SaveAsWavButton} );
  save_as_ogg_button_.Create( {this, CommandID::SaveAsOggButton} );
  open_wav_button_.Create( {this, CommandID::OpenWavButton} );
  show_output_box_button_.Create( {this, CommandID::ShowOutputButton} );

  this->SetSize( 376, 420 );
  if ( start_position_ ) {
    this->SetPosition( start_position_->x, start_position_->y );
  }
  else {
    this->SetCenterRelativeToParent();
  }

  path_label_.SetPositionSize(                8,   8, 348,  18 );
  title_and_artist_albel_.SetPositionSize(    8,  24, 348,  18 );
  result_label_.SetPositionSize(             32,  48, 214,  18 );
  abort_button_.SetPositionSize(            250,  44, 100,  20 );
  status_group_.SetPositionSize(              4,  64, 352, 200 );
  initialize_title_label_.SetPositionSize(   16,  88, 160,  18 );
  initialize_report_label_.SetPositionSize( 180,  84, 160,  18 );
  parse_title_label_.SetPositionSize(        16, 114, 160,  18 );
  parse_report_label_.SetPositionSize(      180, 110, 160,  18 );
  wav_title_label_.SetPositionSize(          16, 140, 160,  18 );
  wav_report_label_.SetPositionSize(        180, 136, 160,  18 );
  wav_progress_.SetPositionSize(             16, 164, 324,  32 );
  mixin_title_label_.SetPositionSize(        16, 212, 160,  18 );
  mixin_report_label_.SetPositionSize(      180, 208, 160,  18 );
  after_title_label_.SetPositionSize(        16, 236, 160,  18 );
  after_report_label_.SetPositionSize(      180, 232, 160,  18 );
  play_wav_button_.SetPositionSize(          16, 280, 132,  24 );
  stop_wav_button_.SetPositionSize(          16, 308, 132,  24 );
  save_as_wav_button_.SetPositionSize(      208, 280, 132,  24 );
  save_as_ogg_button_.SetPositionSize(      208, 308, 132,  24 );
  open_wav_button_.SetPositionSize(          16, 348, 132,  24 );
  show_output_box_button_.SetPositionSize(  208, 348, 132,  24 );

  abort_button_.SetText(            StrT::Convert::AbortButton.Get() );
  initialize_title_label_.SetText(  StrT::Convert::InitializeTitleLabel.Get() );
  initialize_report_label_.SetText( StrT::Convert::InitializeReportLabel.Get() );
  parse_title_label_.SetText(       StrT::Convert::ParseTitleLabel.Get() );
  parse_report_label_.SetText(      StrT::Convert::ParseReportLabel.Get() );
  wav_title_label_.SetText(         StrT::Convert::WavTitleLabel.Get() );
  wav_report_label_.SetText(        StrT::Convert::WavReportLabel.Get() );
  mixin_title_label_.SetText(       StrT::Convert::MixinTitleLabel.Get() );
  mixin_report_label_.SetText(      StrT::Convert::MixinReportLabel.Get() );
  after_title_label_.SetText(       StrT::Convert::AfterTitleLabel.Get() );
  after_report_label_.SetText(      StrT::Convert::AfterReportLabel.Get() );
  play_wav_button_.SetText(         StrT::Convert::PlayWavButton.Get() );
  stop_wav_button_.SetText(         StrT::Convert::StopWavButton.Get() );
  save_as_wav_button_.SetText(      StrT::Convert::SaveAsWavButton.Get() );
  save_as_ogg_button_.SetText(      StrT::Convert::SaveAsOggButton.Get() );
  open_wav_button_.SetText(         StrT::Convert::OpenWavButton.Get() );
  show_output_box_button_.SetText(  StrT::Convert::ShowOutputBoxButton.Get() );

  initialize_report_label_.SetTextAreaAsStandard();
  parse_report_label_.SetTextAreaAsStandard();
  wav_report_label_.SetTextAreaAsStandard();
  mixin_report_label_.SetTextAreaAsStandard();
  after_report_label_.SetTextAreaAsStandard();

  initialize_report_label_.SetState( BorderLabel::State::Ready );
  parse_report_label_.SetState(      BorderLabel::State::Ready );
  wav_report_label_.SetState(        BorderLabel::State::Ready );
  mixin_report_label_.SetState(      BorderLabel::State::Ready );
  after_report_label_.SetState(      BorderLabel::State::Ready );

  this->RegisterSingleHandler( WM_CTLCOLORSTATIC, [this] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    HWND handle = reinterpret_cast<HWND>( l_param );
    HDC  hdc    = reinterpret_cast<HDC>( w_param );
    if ( handle == initialize_report_label_.GetHandle() ) {
      return initialize_report_label_.SetColorToHDCAndReturnWMResult( hdc );
    }
    if ( handle == parse_report_label_.GetHandle() ) {
      return parse_report_label_.SetColorToHDCAndReturnWMResult( hdc );
    }
    if ( handle == wav_report_label_.GetHandle() ) {
      return wav_report_label_.SetColorToHDCAndReturnWMResult( hdc );
    }
    if ( handle == mixin_report_label_.GetHandle() ) {
      return mixin_report_label_.SetColorToHDCAndReturnWMResult( hdc );
    }
    if ( handle == after_report_label_.GetHandle() ) {
      return after_report_label_.SetColorToHDCAndReturnWMResult( hdc );
    }
    return {WMResult::Incomplete};
  }, false );

  this->RegisterWMClose( [&] ( void ) -> WMResult {
    if ( this->IsConverting() ) {
      return {WMResult::Done};
    }
    thread_->Join();

    Core::DirectSoundStream::Player::PlayStop();

    outputted_log_.emplace( output_dialog_.GetTextWithoutCR() );
    output_dialog_.EndDialog( 0 );
    this->EndDialog( static_cast<int>( result_ ) );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AbortButton, [&] ( int, HWND ) -> WMResult {
    this->RequestAbort();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::PlayWavButton, [&] ( int, HWND ) -> WMResult {
    if ( converter_.info_.wave_ ) {
      Core::DirectSoundStream::Player::PlayWaveAsync( *this, converter_.info_.wave_ );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::StopWavButton, [&] ( int, HWND ) -> WMResult {
    Core::DirectSoundStream::Player::PlayStop();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::SaveAsWavButton, [&] ( int, HWND ) -> WMResult {
    if ( converter_.info_.wave_ ) {
      TtSaveFileDialog dialog;
      dialog.GetFilters().push_back( {StrT::Convert::FileDialogWavFile.Get(), "*.wav"} );
      dialog.GetFilters().push_back( {StrT::Convert::FileDialogAllFile.Get(), "*.*"} );
      if ( dialog.ShowDialog( *this ) ) {
        std::string tmp = dialog.GetFileName();
        if ( TtPath::FindExtension( tmp ).empty() ) {
          tmp += ".wav";
        }
        converter_.info_.wave_->WriteToFile( tmp );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::SaveAsOggButton, [&] ( int, HWND ) -> WMResult {
    if ( converter_.info_.wave_ ) {
      TtSaveFileDialog dialog;
      dialog.GetFilters().push_back( {StrT::Convert::FileDialogOggFile.Get(), "*.ogg"} );
      dialog.GetFilters().push_back( {StrT::Convert::FileDialogAllFile.Get(), "*.*"} );
      if ( dialog.ShowDialog( *this ) ) {
        std::string tmp = dialog.GetFileName();
        if ( TtPath::FindExtension( tmp ).empty() ) {
          tmp += ".ogg";
        }
        converter_.info_.wave_->WriteToFileAsOgg( tmp, static_cast<float>( converter_.GetConvertParameter().ogg_base_quality_ ) );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OpenWavButton, [&] ( int, HWND ) -> WMResult {
    if ( NOT( this->IsConverting() ) ) {
      int64_t ret = reinterpret_cast<int64_t>( ::ShellExecute( NULL, "open", converter_.info_.output_file_path_.c_str(), NULL, NULL, SW_SHOWNORMAL ) );
      if ( ret <= 32 ) {
        DWORD error_code = ::GetLastError();
        TtMessageBoxOk box;
        box.SetMessage( Utility::Format( StrT::Convert::MBOpenWavMessage.Get(), converter_.info_.output_file_path_, TtUtility::GetWindowsSystemErrorMessage( error_code ) ) );
        box.SetCaption( StrT::Convert::MBOpenWavCaption.Get() );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::ShowOutputButton, [&] ( int, HWND ) -> WMResult {
    if ( NOT( output_dialog_.GetVisible() ) ) {
      POINT p = this->GetPoint();
      output_dialog_.SetPosition( p.x, p.y + this->GetHeight() );
    }
    output_dialog_.ShowAndSetTop();
    return {WMResult::Done};
  } );

  path_label_.Show();
  title_and_artist_albel_.Show();
  result_label_.Show();
  abort_button_.Show();
  status_group_.Show();
  initialize_title_label_.Show();
  initialize_report_label_.Show();
  parse_title_label_.Show();
  parse_report_label_.Show();
  wav_title_label_.Show();
  wav_report_label_.Show();
  wav_progress_.Show();
  mixin_title_label_.Show();
  mixin_report_label_.Show();
  after_title_label_.Show();
  after_report_label_.Show();
  play_wav_button_.Show();
  stop_wav_button_.Show();
  save_as_wav_button_.Show();
  save_as_ogg_button_.Show();
  open_wav_button_.Show();
  show_output_box_button_.Show();

  // Show しないと表示されないので注意
  output_dialog_.ShowDialog( *this );
  output_dialog_.SetText( StrT::Convert::OutputDialogTitle.Get() );

  this->RegisterConverterCallbacks();

  // log 機能
  if ( converter_.GetConvertParameter().output_log_ ) {
    try {
      log_file_writer_.emplace( converter_.GetConvertParameter().log_file_path_, true, false );
    }
    catch ( TtFileAccessException& e ) {
      output_dialog_.PutsText( Utility::Format( StrT::Convert::OpenLogErrorMessage.Get(),
                                                converter_.GetConvertParameter().log_file_path_,
                                                TtUtility::GetANSIErrorMessage( e.GetErrorNumber() ) ) );
    }
  }

  // SquirrelVM 初期化
  {
    vm_.emplace( this );
    vm_->Initialize();
    vm_->SetPrintFunction( [&] ( const std::string& str ) {
      this->OutputStringWithoutLF( str );
    } );

    this->SquirrelErrorHandling( [&] ( void ) {
      vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + SquirrelVMBase::BASE_FILENAME );
      vm_->InitializeForMainstay();
      vm_->CallConverterConstructor( converter_ );
      vm_->DoFile( TtPath::GetExecutingDirectoryPath() + "\\" + Mainstay::SquirrelVM::CLASS_FILENAME );
      if ( converter_.GetConvertParameter().use_script_file_ ) {
        vm_->DoFile( converter_.GetConvertParameter().script_file_path_ );
      }
    } );
  }

  this->ThreadInitializeAndStart();

  return true;
}


void
ConverterDialog::ThreadInitializeAndStart( void )
{
  thread_ = TtFunctionThread( [&] ( void ) {
    try {
      converter_.Convert();

      if ( result_ == Result::Aborted || result_ == Result::UserAborted ) {
        TtMessageBoxOk box;
        box.SetMessage( StrT::Convert::MBErrorAbortMessage.Get() );
        box.SetCaption( StrT::Convert::MBErrorAbortCaption.Get() );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
      }
    }
    catch ( std::bad_alloc ) {
      TtMessageBoxOk box;
      box.SetMessage( StrT::Convert::MBErrorMemoryMessage.Get() );
      box.SetCaption( StrT::Convert::MBErrorMemoryCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    catch ( ... ) {
      TtMessageBoxOk box;
      box.SetMessage( StrT::Convert::MBErrorUnexpectedMessage.Get() );
      box.SetCaption( StrT::Convert::MBErrorUnexpectedCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }

    if ( auto_close_ ) {
      this->PostMessage( WM_CLOSE );
    }
  } );
  thread_->Start();
}

void
ConverterDialog::RegisterConverterCallbacks( void )
{
  // エラー系
  converter_.callbacks_.exception_occurred_ = [&] ( Core::Converter&, ConvertException& exception ) {
    if ( auto* tmp = dynamic_cast<ConvertBmsDescriptionException*>( &exception ); tmp ) {
      vm_->CallBmsDescriptionExceptionConstructorAndPushIt( *tmp );
      vm_->Native().PopTop();
      this->CallParserExceptionOccurred( *tmp );
    }
    else {
      vm_->CallConvertExceptionConstructorAndPushIt( exception );
      vm_->Native().PopTop();
      this->CallExceptionOccurred( exception );
    }

    if ( exception.GetErrorLevel() <= ErrorLevel::Tiny ) {
      current_report_end_state_  = BorderLabel::State::WithError;
      current_report_end_string_ = StrT::Convert::ReportLabelErrorExist.Get();

      result_ = Result::WithError;
    }
  };

  converter_.callbacks_.aborted_ = [&] ( Core::Converter& ) {
    if ( current_label_ ) {
      current_label_->SetState( BorderLabel::State::Aborted );
      current_label_->SetText( StrT::Convert::ReportLabelAbort.Get() );
    }
    result_label_.SetText( StrT::Convert::ReportResultAbort.Get() );
    result_ = user_abort_ ? Result::UserAborted : Result::Aborted;

    this->CallAborted( user_abort_ );
  };

  // -- 初期化・前処理
  converter_.callbacks_.before_initialize_ = [&] ( Core::Converter& ) {
    user_abort_ = false;
    abort_button_.SetEnabled( true );
    play_wav_button_.SetEnabled( false );
    stop_wav_button_.SetEnabled( false );
    save_as_wav_button_.SetEnabled( false );
    save_as_ogg_button_.SetEnabled( false );
    open_wav_button_.SetEnabled( false );

    this->ResetCurrentLabel( initialize_report_label_ );
    initialize_report_label_.SetState( BorderLabel::State::Running );
    initialize_report_label_.SetText( StrT::Convert::ReportLabelInitializeRunning.Get() );

    path_label_.SetText( converter_.GetConvertParameter().input_file_path_ );
    result_label_.SetText( StrT::Convert::ReportResultInitializeRunning.Get() );

    this->CallBeforeInitialize();
  };

  converter_.callbacks_.after_initialize_ = [&] ( Core::Converter& ) {
    initialize_report_label_.SetState( current_report_end_state_ );
    initialize_report_label_.SetText( current_report_end_string_ );

    this->CallAfterInitialize();
  };

  // BMS 構文解析
  converter_.callbacks_.before_parse_ = [&] ( Core::Converter& ) {
    this->ResetCurrentLabel( parse_report_label_ );
    parse_report_label_.SetState( BorderLabel::State::Running );
    parse_report_label_.SetText( StrT::Convert::ReportLabelParseRunning.Get() );

    result_label_.SetText( StrT::Convert::ReportResultParseRunning.Get() );

    this->CallBeforeParse();
  };

  converter_.callbacks_.just_before_parse_ = [&] ( Core::Converter&, BL::Parser::Parser& parser ) {
    // Parser Callbacks
    parser.callbacks_.exception_occurred_ = [&] ( BL::Parser::Parser&, BL::Parser::BmsDescriptionException& ) {
      // converter_.callbacks_.exception_occurred_ が呼ばれるので特にやること無し
    };

    parser.callbacks_.aborted_ = [&] ( BL::Parser::Parser& ) {
      converter_.SetNeedToAbortImediately( true );
    };

    // BGAのチャンネルの読込回避
    parser.callbacks_.header_detected_ = [&] ( BL::Parser::Parser& parser, const std::string& header ) {
      if ( converter_.GetConvertParameter().ignore_bga_channel_ && parser.frame_->bms_data_.bmp_array_.CanApply( header ) ) {
        parser.frame_->skip_parse_line_ = true;
      }
    };

    parser.callbacks_.channel_number_detected_ = [&] ( BL::Parser::Parser& parser, BL::Word channel_number ) {
      if ( converter_.GetConvertParameter().ignore_bga_channel_ && BL::Channel::NumberIsBmpChannel( channel_number ) ) {
        parser.frame_->skip_parse_line_ = true;
      }
    };

    // RANDOM 構文関連
    parser.callbacks_.complete_random_syntactic_analysis_ = [&] ( BL::Parser::Parser& parser ) {
      bool has_random = false;
      for ( auto& statement : parser.frame_->root_.statements_ ) {
        if ( auto* tmp = dynamic_cast<BL::Parser::IfBlock*>( statement.get() ); tmp ) {
          has_random = true;
        }
      }
      if ( has_random ) {
        RandomStatementDialog dialog( converter_.GetConvertParameter().input_file_path_, parser.frame_->root_ );

        // root にチェックを付ける
        this->RegisterSingleHandler( WM_ENTERIDLE, [&, done = false] ( WPARAM w_param, LPARAM l_param ) mutable -> WMResult {
          if ( NOT( done ) && w_param == MSGF_DIALOGBOX && reinterpret_cast<HWND>( l_param ) == dialog.GetHandle() ) {
            done = true;
            dialog.CheckToRootItem();

            TtMessageBoxOk box;
            box.SetMessage( StrT::Convert::MBInfoRandomDialogMessage.Get() );
            box.SetCaption( StrT::Convert::MBInfoRandomDialogCaption.Get() );
            box.SetIcon( TtMessageBox::Icon::INFORMATION );
            box.ShowDialog( *this );
          }
          return {WMResult::Done};
        }, true );

        auto tmp = TtTime::GetNow();
        dialog.ShowDialog( *this );
        converter_.info_.correction_time_ += TtTime::GetNow() - tmp;
      }
    };
  };

  converter_.callbacks_.just_after_parse_ = [&] ( Core::Converter& converter, BL::Parser::Parser& ) {
    vm_->SetBmsDataToConverterObject( converter );
  };

  converter_.callbacks_.decide_output_file_path_ = [&] ( Core::Converter& converter ) {
    std::optional<std::string> tmp = this->CallDecideOutputFilePath();
    if ( tmp ) {
      converter.SetOutputFilePath( tmp.value() );
      vm_->SetOutputFilePathToConverterObject( converter );
    }
  };

  converter_.callbacks_.after_parse_ = [&] ( Core::Converter& converter ) {
    parse_report_label_.SetState( current_report_end_state_ );
    parse_report_label_.SetText( current_report_end_string_ );

    auto f = [&] ( const std::string& key ) -> std::string {
      if ( auto it = converter.info_.bms_data_->headers_.find( key ); it != converter.info_.bms_data_->headers_.end() ) {
        return it->second;
      }
      return "";
    };
    title_and_artist_albel_.SetText( f( "TITLE" ) + " / " + f( "ARTIST" ) );

    this->CallAfterParse();
  };

  // 音声ファイル読み込み
  converter_.callbacks_.before_read_audio_files_ = [&] ( Core::Converter& converter ) {
    this->ResetCurrentLabel( wav_report_label_ );
    wav_report_label_.SetState( BorderLabel::State::Running );
    wav_report_label_.SetText( StrT::Convert::ReportLabelReadAudioFilesRunning.Get() );

    result_label_.SetText( StrT::Convert::ReportResultReadAudioFilesRunning.Get() );

    wav_count_max_ = converter.info_.bms_data_->wav_array_.GetExistCount();
    wav_count_ = 0;
    wav_progress_.SetMaximum( static_cast<short>( wav_count_max_ ) );
    wav_progress_.SetAmount( 0 );
    wav_progress_.SetStep( 1 );

    this->CallBeforeReadAudioFiles();
  };

  converter_.callbacks_.one_audio_file_read_ = [&] ( Core::Converter& ) {
    wav_count_ += 1;
    wav_progress_.StepIt();

    wav_report_label_.SetText( Utility::Format( "%d / %d " + StrT::Convert::ReportLabelOneAudioFileReadRunning.Get(), wav_count_, wav_count_max_ ) );
  };

  converter_.callbacks_.decide_audio_file_path_ = [&] ( Core::Converter& converter, const std::string& filename ) {
    std::optional<std::string> tmp = this->CallDecideAudioFilePath( filename );
    converter.SetTemporaryAudioFilePath( tmp ? tmp.value() : "" );
  };

  converter_.callbacks_.audio_file_read_start_ = [&] ( Core::Converter&, BL::Word word, const std::string& path ) {
    this->CallAudioFileReadStart( word, path );
  };

  converter_.callbacks_.audio_file_read_end_ = [&] ( Core::Converter&, BL::Word word, const std::string& path ) {
    this->CallAudioFileReadEnd( word, path );
  };

  converter_.callbacks_.after_read_audio_files_ = [&] ( Core::Converter& ) {
    wav_report_label_.SetState( current_report_end_state_ );
    wav_report_label_.SetText( Utility::Format( "%d / %d " + current_report_end_string_, wav_count_, wav_count_max_ ) );

    this->CallAfterReadAudioFiles();
  };

  // 音声合成
  converter_.callbacks_.before_mixin_waves_ = [&] ( Core::Converter& ) {
    this->ResetCurrentLabel( mixin_report_label_ );
    mixin_report_label_.SetState( BorderLabel::State::Running );
    mixin_report_label_.SetText( StrT::Convert::ReportLabelMixinRunning.Get() );

    result_label_.SetText( StrT::Convert::ReportResultMixinRunning.Get() );

    this->CallBeforeMixinWaves();
  };

  converter_.callbacks_.after_mixin_waves_ = [&] ( Core::Converter& ) {
    mixin_report_label_.SetState( current_report_end_state_ );
    mixin_report_label_.SetText( current_report_end_string_ );

    this->CallAfterMixinWaves();
  };

  // 調整・出力
  converter_.callbacks_.before_affect_wave_ = [&] ( Core::Converter& ) {
    this->ResetCurrentLabel( after_report_label_ );
    after_report_label_.SetState( BorderLabel::State::Running );
    after_report_label_.SetText( StrT::Convert::ReportLabelAffectWaveRunning.Get() );

    result_label_.SetText( StrT::Convert::ReportResultAffectWaveRunning.Get() );

    this->CallBeforeAffectWave();
  };

  converter_.callbacks_.complete_normalize_ = [&] ( Core::Converter&, double ratio ) {
    this->CallCompleteNormalize( ratio );
  };

  converter_.callbacks_.after_affect_wave_ = [&] ( Core::Converter& ) {
    this->CallAfterAffectWave();
  };

  // -- ファイル出力 -----
  converter_.callbacks_.before_output_to_file_ = [&] ( Core::Converter& ) {
    result_label_.SetText( StrT::Convert::ReportResultOutputToFileRunning.Get() );

    this->CallBeforeOutputToFile();
  };

  converter_.callbacks_.after_output_to_file_ = [&] ( Core::Converter& ) {
    after_report_label_.SetState( current_report_end_state_ );
    after_report_label_.SetText( current_report_end_string_ );

    if ( converter_.info_.output_file_path_.empty() ) {
      after_report_label_.SetState( BorderLabel::State::WithError );
      after_report_label_.SetText( StrT::Convert::ReportLabelAfterOutputToFileNoOutput.Get() );
    }

    result_label_.SetText( Utility::Format( StrT::Convert::ReportResultAfterOutputToFileCompleted.Get(), converter_.GetProcessingTime() ) );

    this->CallAfterOutputToFile();
  };

  // 後処理
  converter_.callbacks_.after_process_ = [&] ( Core::Converter& ) {
    this->CallAfterProcess();
  };

  // 終了
  converter_.callbacks_.before_finalize_ = [&] ( Core::Converter& ) {
    this->CallBeforeFinalize();
  };

  converter_.callbacks_.after_finalize_ = [&] ( Core::Converter& converter ) {
    abort_button_.SetEnabled( false );
    if ( converter.info_.wave_ ) {
      play_wav_button_.SetEnabled( true );
      stop_wav_button_.SetEnabled( true );
      save_as_wav_button_.SetEnabled( true );
      save_as_ogg_button_.SetEnabled( true );
      open_wav_button_.SetEnabled( true );
    }

    this->CallAfterFinalize();
  };
}


void
ConverterDialog::OutputString( const std::string& str )
{
  output_dialog_.PutsText( str );
  this->OutputToLog( str + "\n" );
}

void
ConverterDialog::OutputStringWithoutLF( const std::string& str )
{
  output_dialog_.AddText( str );
  this->OutputToLog( str );
}

void
ConverterDialog::OutputToLog( const std::string& str )
{
  if ( log_file_writer_ ) {
    try {
      log_file_writer_->WriteString( str );
    }
    catch ( TtFileAccessException ) {
      output_dialog_.PutsText( StrT::Convert::OutputToLogError.Get() );
    }
    log_file_writer_->Flush();
  }
}


template <Mainstay::SquirrelVM::TagName& tag, class... Args>
void
ConverterDialog::CallCallback( Args... args )
{
  this->SquirrelErrorHandling( [&] ( void ) {
    if ( vm_->ExistCallbackFunction( tag ) ) {
      vm_->CallCallbackFunctionAndNoReturnValue<Args...>( tag, converter_, args... );
    }
  } );
}


template <Mainstay::SquirrelVM::TagName& tag, class ReturnValueType, class... Args>
std::optional<ReturnValueType>
ConverterDialog::CallCallbackAndGetReturnValueAs( Args... args )
{
  std::optional<ReturnValueType> ret = std::nullopt;
  this->SquirrelErrorHandling( [&] ( void ) {
    if ( vm_->ExistCallbackFunction( tag ) ) {
      ret = vm_->CallCallbackFunctionAndGetReturnValueAs<ReturnValueType>( tag, converter_, args... );
    }
  } );
  return ret;
}

// -- Callback Functions ----------
// progress
void
ConverterDialog::CallBeforeInitialize( void )
{
  this->CallCallback<TagName::before_initialize>();
}

void
ConverterDialog::CallAfterInitialize( void )
{
  this->CallCallback<TagName::after_initialize>();
}

void
ConverterDialog::CallBeforeParse( void )
{
  this->CallCallback<TagName::before_parse>();
}

void
ConverterDialog::CallAfterParse( void )
{
  this->CallCallback<TagName::after_parse>();
}

void
ConverterDialog::CallBeforeReadAudioFiles( void )
{
  this->CallCallback<TagName::before_read_audio_files>();
}

void
ConverterDialog::CallAfterReadAudioFiles( void )
{
  this->CallCallback<TagName::after_read_audio_files>();
}

void
ConverterDialog::CallBeforeMixinWaves( void )
{
  this->CallCallback<TagName::before_mixin_waves>();
}

void
ConverterDialog::CallAfterMixinWaves( void )
{
  this->CallCallback<TagName::after_mixin_waves>();
}

void
ConverterDialog::CallBeforeAffectWave( void )
{
  this->CallCallback<TagName::before_affect_wave>();
}

void
ConverterDialog::CallAfterAffectWave( void )
{
  this->CallCallback<TagName::after_affect_wave>();
}

void
ConverterDialog::CallBeforeOutputToFile( void )
{
  this->CallCallback<TagName::before_output_to_file>();
}

void
ConverterDialog::CallAfterOutputToFile( void )
{
  this->CallCallback<TagName::after_output_to_file>();
}

void
ConverterDialog::CallBeforeFinalize( void )
{
  this->CallCallback<TagName::before_finalize>();
}

void
ConverterDialog::CallAfterFinalize( void )
{
  this->CallCallback<TagName::after_finalize>();
}

// point progress
std::optional<std::string>
ConverterDialog::CallDecideOutputFilePath( void )
{
  return this->CallCallbackAndGetReturnValueAs<TagName::decide_output_file_path, std::string>();
}

std::optional<std::string>
ConverterDialog::CallDecideAudioFilePath( const std::string& filename )
{
  return this->CallCallbackAndGetReturnValueAs<TagName::decide_audio_file_path, std::string>( filename );
}

void
ConverterDialog::CallAudioFileReadStart( BL::Word word, const std::string& path )
{
  this->CallCallback<TagName::audio_file_read_start>( word, path );
}

void
ConverterDialog::CallAudioFileReadEnd( BL::Word word, const std::string& path )
{
  this->CallCallback<TagName::audio_file_read_end>( word, path );
}

void
ConverterDialog::CallCompleteNormalize( double ratio )
{
  this->CallCallback<TagName::complete_normalize>( ratio );
}

void
ConverterDialog::CallAfterProcess( void )
{
  this->CallCallback<TagName::after_process>();
}

// abort
void
ConverterDialog::CallAborted( bool user_abort )
{
  this->CallCallback<TagName::aborted>( user_abort );
}

// exception
void
ConverterDialog::CallExceptionOccurred( ConvertException& exception )
{
  this->CallCallback<TagName::exception_occurred, ConvertException&>( exception );
}

void
ConverterDialog::CallParserExceptionOccurred( ConvertBmsDescriptionException& exception )
{
  this->CallCallback<TagName::parser_exception_occurred, ConvertBmsDescriptionException&>( exception );
}


void
ConverterDialog::SquirrelErrorHandling( std::function<void ( void )> function )
{
  this->SquirrelErrorHandlingReturnErrorNotOccurred( [&] ( void ) {
    function();
    return true;
  } );
}

bool
ConverterDialog::SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function )
{
  try {
    return function();
  }
  catch ( TtSquirrel::Exception& ex ) {
    std::string tmp = StrT::Convert::MBErrorScriptMessage.Get();
    std::string message = ex.GetStandardMessage();
    if ( NOT( message.empty() ) ) {
      tmp.append( "\r\n\r\n" );
      tmp.append( message );
    }
    tmp.append( "\r\n" );
    this->OutputString( tmp );

    TtMessageBoxOk box;
    box.SetMessage( tmp );
    box.SetCaption( StrT::Convert::MBErrorScriptCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
}


// -- BorderLabel -----
HBRUSH
ConverterDialog::BorderLabel::ReadyBrush     = ::CreateSolidBrush( RGB( 0xCC, 0xCC, 0xCC ) );
HBRUSH
ConverterDialog::BorderLabel::RunningBrush   = ::CreateSolidBrush( RGB( 0x00, 0x00, 0x77 ) );
HBRUSH
ConverterDialog::BorderLabel::CompletedBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF ) );
HBRUSH
ConverterDialog::BorderLabel::WithErrorBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0x00 ) );
HBRUSH
ConverterDialog::BorderLabel::AbortedBrush   = ::CreateSolidBrush( RGB( 0xFF, 0x00, 0x00 ) );

void
ConverterDialog::BorderLabel::SetState( State state )
{
  state_ = state;
}

TtWindow::WMResult
ConverterDialog::BorderLabel::SetColorToHDCAndReturnWMResult( HDC hdc )
{
  const COLORREF White = RGB( 0xFF, 0xFF, 0xFF );
  const COLORREF Black = RGB( 0x00, 0x00, 0x00 );

  ::SetBkMode( hdc, TRANSPARENT );
  switch ( state_ ) {
  case State::Ready:
  default:
    ::SetTextColor( hdc, Black );
    return {WMResult::DoneAndDirectReturnAtDialog, reinterpret_cast<LRESULT>( ReadyBrush )};

  case State::Running:
    ::SetTextColor( hdc, White );
    return {WMResult::DoneAndDirectReturnAtDialog, reinterpret_cast<LRESULT>( RunningBrush )};

  case State::Completed:
    ::SetTextColor( hdc, Black );
    return {WMResult::DoneAndDirectReturnAtDialog, reinterpret_cast<LRESULT>( CompletedBrush )};

  case State::WithError:
    ::SetTextColor( hdc, Black );
    return {WMResult::DoneAndDirectReturnAtDialog, reinterpret_cast<LRESULT>( WithErrorBrush )};

  case State::Aborted:
    ::SetTextColor( hdc, Black );
    return {WMResult::DoneAndDirectReturnAtDialog, reinterpret_cast<LRESULT>( AbortedBrush )};
  }
}

void
ConverterDialog::BorderLabel::SetTextAreaAsStandard( void )
{
  RECT rect = {2, 2, this->GetWidth() - 4, this->GetHeight() - 4};
  this->SendMessage( EM_SETRECT, 0, reinterpret_cast<LPARAM>( &rect ) );
}

void
ConverterDialog::ResetCurrentLabel( BorderLabel& label )
{
  current_label_             = &label;
  current_report_end_state_  = BorderLabel::State::Completed;
  current_report_end_string_ = StrT::Convert::ReportLabelCompleted.Get();
}
