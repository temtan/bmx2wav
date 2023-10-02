// utility_dialogs.cpp

#include "ttl_define.h"
#include "tt_string.h"

#include "common.h"
#include "utility.h"

#include "utility_dialogs.h"

using namespace BMX2WAV;


// -- OutputDialog -------------------------------------------------------
OutputDialog::OutputDialog( void ) :
OutputDialog( "" )
{
}

OutputDialog::OutputDialog( const std::string& title ) :
title_( title )
{
}

void
OutputDialog::AddText( const std::string& data )
{
  int length = output_edit_.GetTextLength();
  output_edit_.SendMessage( EM_SETSEL, length, length );
  std::string str = TtString::AddCR( data );
  // std::string str = std::regex_replace( data, std::regex( "(?!\\r)\\n" ), "\r\n" );
  output_edit_.SendMessage( EM_REPLACESEL, 0, reinterpret_cast<WPARAM>( str.c_str() ) );
}

void
OutputDialog::PutsText( const std::string& data )
{
  this->AddText( data + "\r\n" );
}

void
OutputDialog::ClearText( void )
{
  output_edit_.SetText( "" );
}


std::string
OutputDialog::GetTextWithoutCR( void )
{
  // return std::regex_replace( output_edit_.GetText(), std::regex( "\\r\\n" ), "\n" );
  return TtString::RemoveCR( output_edit_.GetText() );
}


void
OutputDialog::ShowAndSetTop( void )
{
  this->Show();
  this->SetTop();
}


DWORD
OutputDialog::GetStyle( void )
{
  return WS_SYSMENU | WS_SIZEBOX;
}

DWORD
OutputDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
OutputDialog::Created( void )
{
  this->SetText( title_ );

  output_edit_.Create( {this} );
  output_edit_.SetTextLimit( 1024 * 1024 * 2 );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    output_edit_.SetPositionSize( 2, 2, w - 4, h - 4 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 550, 350, false );

  this->RegisterWMClose( [this] ( void ) -> WMResult {
    this->Hide();
    // •Â‚¶‚È‚¢
    return {WMResult::Done};
  }, false );

  output_edit_.Show();
  return {true};
}

// -- UserInputDialogBase ------------------------------------------------
UserInputDialogBase::UserInputDialogBase( const std::string& title, const std::string& explanation, std::unique_ptr<TtEdit> edit ) :
title_( title ),
explanation_( explanation ),
input_(),

edit_( std::move( edit ) )
{
}



DWORD
UserInputDialogBase::GetStyle( void )
{
  return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX;
}

DWORD
UserInputDialogBase::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
UserInputDialogBase::Created( void )
{
  this->SetText( title_ );

  struct CommandID {
    enum ID : int {
      Edit = 10001,
      Ok,
    };
  };
  explanation_label_.Create( {this} );
  edit_->Create( {this, CommandID::Edit} );
  ok_button_.Create( {this, CommandID::Ok} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    explanation_label_.SetPositionSize(  4,  4,     w - 20, 20 );
    edit_->SetPositionSize(              4, 24, w - 4 - 48, 20 );
    ok_button_.SetPositionSize(     w - 44, 24,         40, 20 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 392, 54, false );
  this->RegisterWMSizing( this->MakeCanChangeOnlyHorizontalHandler(), false );

  explanation_label_.SetText( explanation_ );
  ok_button_.SetText( "OK" );

  edit_->SetFocus();

  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    input_ = edit_->GetText();
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );

  explanation_label_.Show();
  edit_->Show();
  ok_button_.Show();

  this->DragAcceptFiles( true );
  this->RegisterWMDropFiles( [this] ( HDROP drop ) -> WMResult {
    const unsigned int buf_size = ::DragQueryFile( drop, 0, nullptr, 0 ) + 1;
    auto buf = std::make_unique<char[]>( buf_size );
    ::DragQueryFile( drop, 0, buf.get(), buf_size );
    edit_->SetText( buf.get() );

    ::DragFinish( drop );
    return {WMResult::Done};
  } );

  return true;
}


const std::string&
UserInputDialogBase::GetTitle( void )
{
  return title_;
}

void
UserInputDialogBase::SetTitle( const std::string& title )
{
  title_ = title;
}


const std::string&
UserInputDialogBase::GetExplanation( void )
{
  return explanation_;
}

void
UserInputDialogBase::SetExplanation( const std::string& explanation )
{
  explanation_ = explanation;
}


const std::string&
UserInputDialogBase::GetInput( void )
{
  return input_;
}


// -- VersionDialog ------------------------------------------------------
VersionDialog::VersionDialog( void ) :
icon_( Image::ICONS[Image::Index::Main] ),
name_label_(),
version_label_(),
date_label_(),
close_button_()
{
}


DWORD
VersionDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
VersionDialog::Created( void )
{
  std::string title = "version information";
  std::string close_mark = "X";
  if ( BMX2WAV::Utility::UserDefaultLocaleIsJapanese() ) {
    title = "ƒo[ƒWƒ‡ƒ“î•ñ";
    close_mark = "•Â";
  }
  const int ButtonCommandID = 10001;

  name_label_.Create( {this} );
  version_label_.Create( {this} );
  date_label_.Create( {this} );
  close_button_.Create( {this, ButtonCommandID} );

  this->SetText( title );

  this->SetSize( 300, 108 );

  name_label_.SetPositionSize(     60, 12, 180, 24 );
  version_label_.SetPositionSize(  60, 28, 100, 24 );
  date_label_.SetPositionSize(     60, 44, 200, 24 );
  close_button_.SetPositionSize(  230, 12,  40, 24 );

  name_label_.SetText( BMX2WAV::APPLICATION_NAME );
  version_label_.SetText( std::string( "version " ) + BMX2WAV::VERSION );
  {
    std::string date_str;
    date_str.append( "Build at : " );
    date_str.append( TTL_DATE_STRING );
    date_str.append( " " );
    date_str.append( __TIME__ );
    date_label_.SetText( date_str );
  }
  close_button_.SetText( close_mark );

  this->AddCommandHandler( ButtonCommandID, [this] ( int, HWND ) -> WMResult {
    this->Close();
    return {WMResult::Done};
  } );

  this->RegisterWMPaint( [this] ( void ) -> WMResult {
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint( handle_, &ps );
    ::DrawIcon( hdc, 10, 10, icon_.GetHandle() );
    ::EndPaint( handle_, &ps );
    return {WMResult::Done};
  }, false );

  name_label_.Show();
  version_label_.Show();
  date_label_.Show();
  close_button_.Show();

  close_button_.SetFocus();

  this->SetCenterRelativeToParent();

  return true;
}
