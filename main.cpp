// main.cpp

#include "ttl_define.h"
#include "tt_command_line_analyzer.h"
#include "tt_message_box.h"

#include "common.h"
#include "utility.h"
#include "string_table.h"
#include "mainstay/main_frame.h"

static void show_unexpected_error_message_box( void );


int WINAPI
WinMain( HINSTANCE h_instance,
         HINSTANCE h_prev_instance,
         PSTR lp_cmd_line,
         int n_cmd_show )
{
  NOT_USE( h_instance );
  NOT_USE( h_prev_instance );
  NOT_USE( n_cmd_show );

  try {
    // 言語選択
    BMX2WAV::StringTable::Initialize( BMX2WAV::IniFileOperation::LoadLanguage() );

    BMX2WAV::Image::Initialize();
    BMX2WAV::Mainstay::MainFrame frame;
    frame.Create();
    frame.LoadPlacementFromIniFile();
    if ( frame.IsMinimized() ) {
      frame.Restore();
    }

    try {
      TtCommandLine::Analyzer analyzer;
      auto rest = analyzer.AnalyzeForWindows( lp_cmd_line );
      for ( auto& one : rest ) {
        frame.AddEntry( one );
      }
      frame.IfToggleButtonIsPressedDisplayCell();
    }
    catch ( TtCommandLine::Exception& e ) {
      TtMessageBoxOk box;
      box.SetMessage( BMX2WAV::Utility::Format( BMX2WAV::StrT::Message::CommandLineError.Get(), e.Dump() ) );
      box.SetCaption( BMX2WAV::StrT::Message::CommandLineErrorCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }

    frame.Show();
    return TtForm::LoopDispatchMessage();
  }
  catch ( BMX2WAV::LanguageException& e ) {
    // ここだけは生メッセージ
    if ( BMX2WAV::Utility::UserDefaultLocaleIsJapanese() ) {
      TtMessageBoxOk box;
      box.AppendMessage() << "言語 DLL の読み込み時にエラーが発生しました。\r\n";
      box.AppendMessage() << "言語 : " << e.GetLanguage();
      box.SetCaption( "言語選択エラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }
    else {
      TtMessageBoxOk box;
      box.AppendMessage() << "Error occurred while loading the language DLL.\r\n";
      box.AppendMessage() << "Language : " << e.GetLanguage();
      box.SetCaption( "Language selection error" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }
    return 3;
  }
  catch ( TtException& e ) {
    BMX2WAV::IniFileOperation::SaveErrorLogDump( e );
    show_unexpected_error_message_box();
    return 2;
  }
  catch ( ... ) {
    show_unexpected_error_message_box();
    return 1;
  }
}

static void
show_unexpected_error_message_box( void )
{
  TtMessageBoxOk box;
  box.SetMessage( BMX2WAV::StrT::Message::UnexpectedError.Get() );
  box.SetCaption( BMX2WAV::StrT::Message::UnexpectedErrorCaption.Get() );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog();
}
