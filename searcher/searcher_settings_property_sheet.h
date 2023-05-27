// searcher/searcher_settings_property_sheet.h

#pragma once

#include "tt_property_sheet.h"
#include "tt_window_controls.h"
#include "tt_list_box.h"

#include "searcher/searcher_settings.h"


namespace BMX2WAV::Searcher {
  // -- SettingsPropertySheet -------------------------------------------
  class SettingsPropertySheet : public TtPropertySheet {
  private:
    struct CommandID {
      enum ID : int {
        HomeReferenceButton = 18001,
        LanguageSelectButton,
      };
    };

    // -- SettingsPage --------------------------------------------------
    class SettingsPage : public Page {
    public:
      explicit SettingsPage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings& settings_;

      TtStatic   home_label_;
      TtEdit     home_edit_;
      TtButton   home_ref_button_;

      TtCheckBox auto_display_check_;
      TtCheckBox asynchronous_check_;
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
    SettingsPage settings_page_;
    LanguagePage language_page_;
  };
}
