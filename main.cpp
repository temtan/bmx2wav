// main.cpp

#include "ttl_define.h"
#include "tt_command_line_analyzer.h"
#include "tt_message_box.h"

#include "common.h"
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
      ( box.AppendMessage() << "コマンドライン引数でエラーが発生しました。\r\n" <<
        "情報 : " << e.Dump() << "\r\n\r\n" <<
        "コマンドライン引数は無視されます。" );
      box.SetCaption( "コマンドラインエラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }

    frame.Show();
    return TtForm::LoopDispatchMessage();
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
  box.SetMessage( "予期しないエラーが発生しました。アプリケーションを終了させます。" );
  box.SetCaption( "予期しないエラー" );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog();
}
