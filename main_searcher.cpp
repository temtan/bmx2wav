// main_searcher.cpp

#include "ttl_define.h"
#include "tt_message_box.h"

#include "searcher/searcher_common.h"
#include "searcher/searcher_frame.h"

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
  NOT_USE( lp_cmd_line );

  TtWindow::InitializeCommonControls();

  try {
    BMX2WAV::Searcher::Image::Initialize();
    BMX2WAV::Searcher::MainFrame frame;
    frame.Create();

    frame.LoadSettingsFromFile();
    frame.SetAutoDisplayCellButtonFromSettings();
    frame.SetSelectTargetFolderFromSettings();

    frame.LoadPlacementFromIniFile();
    if ( frame.IsMinimized() ) {
      frame.Restore();
    }

    frame.Show();
    return TtForm::LoopDispatchMessage();
  }
  catch ( TtException& e ) {
    BMX2WAV::Searcher::IniFileOperation::SaveErrorLogDump( e );
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
