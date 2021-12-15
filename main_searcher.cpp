// main_searcher.cpp

#include "ttl_define.h"
#include "tt_message_box.h"

#include "utility.h"
#include "string_table.h"

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
    // 言語選択
    BMX2WAV::StringTable::Initialize( BMX2WAV::IniFileOperation::LoadLanguage() );

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
  box.SetMessage( BMX2WAV::StrT::Message::UnexpectedError.Get() );
  box.SetCaption( BMX2WAV::StrT::Message::UnexpectedErrorCaption.Get() );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog();
}
