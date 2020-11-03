// utility_dialogs.cpp

#include "ttl_define.h"
#include "tt_string.h"

#include "common.h"

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
  const int ButtonCommandID = 10001;

  name_label_.Create( {this} );
  version_label_.Create( {this} );
  date_label_.Create( {this} );
  close_button_.Create( {this, ButtonCommandID} );

  this->SetText( "ƒo[ƒWƒ‡ƒ“î•ñ" );

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
  close_button_.SetText( "•Â" );

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
