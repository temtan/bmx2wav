// utility_dialogs.h

#pragma once

#include "tt_form.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"


namespace BMX2WAV {
  // -- OutputDialog --------------------------------------------
  class OutputDialog : public TtDialogModeless {
  public:
    explicit OutputDialog( void );
    explicit OutputDialog( const std::string& title );

    void AddText( const std::string& data );
    void PutsText( const std::string& data );
    void ClearText( void );

    std::string GetTextWithoutCR( void );

    void ShowAndSetTop( void );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

  private:
    std::string title_;

    using OutputEdit = TtWindowOverwriteStyle<TtEdit, TtEdit::Style::READONLY | TtEdit::Style::MULTILINE | WS_VSCROLL | WS_HSCROLL>;
    OutputEdit output_edit_;
  };

  // -- VersionDialog ----------------------------------------------------
  class VersionDialog : public TtDialog {
  public:
    explicit VersionDialog( void );

    virtual DWORD GetExtendedStyle( void );

    virtual bool Created( void );

  private:
    TtIcon   icon_;
    TtStatic name_label_;
    TtStatic version_label_;
    TtStatic date_label_;
    TtButton close_button_;
  };
}
