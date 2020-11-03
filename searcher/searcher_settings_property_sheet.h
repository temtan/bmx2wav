// searcher_settings_property_sheet.h

#pragma once

#include "tt_property_sheet.h"
#include "tt_window_controls.h"

#include "searcher/searcher_settings.h"


namespace BMX2WAV::Searcher {
  // -- SettingsPropertySheet -------------------------------------------
  class SettingsPropertySheet : public TtPropertySheet {
  private:
    struct CommandID {
      enum ID : int {
        HomeReferenceButton = 18001,
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

    // -- SettingsPropertySheet
  public:
    explicit SettingsPropertySheet( Settings& settings );

    virtual bool Created( void ) override;

  private:
    SettingsPage settings_page_;
  };
}
