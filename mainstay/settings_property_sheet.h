// mainstay/settings_property_sheet.h

#pragma once

#include "tt_property_sheet.h"
#include "tt_window_controls.h"
#include "tt_list_box.h"

#include "mainstay/settings.h"


namespace BMX2WAV::Mainstay {
  // -- SettingsPropertySheet -------------------------------------------
  class SettingsPropertySheet : public TtPropertySheet {
  private:
    struct CommandID {
      enum ID : int {
        LanguageSelectButton = 17001,
      };
    };

    // -- LanguagePage ---------------------------------------------------
    class LanguagePage : public Page {
    public:
      explicit LanguagePage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings& settings_;
      std::string language_string_;

      using ListBox = TtListBoxWithStyle<WS_VSCROLL | WS_HSCROLL>;

      TtStatic language_label_;
      TtStatic notice_label_;
      TtGroup  language_group_;
      TtButton select_button_;
      ListBox  dll_list_box_;
    };

    // -- SettingsPropertySheet
  public:
    explicit SettingsPropertySheet( Settings& settings );

    virtual bool Created( void ) override;

  private:
    LanguagePage language_page_;
  };
}
