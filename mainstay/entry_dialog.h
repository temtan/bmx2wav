// mainstay/entry_dialog.h

#pragma once

#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_list_view.h"

#include "core/convert_parameter.h"

#include "mainstay/entry.h"
#include "mainstay/main_squirrel_vm.h"
#include "mainstay/main_frame.h"


namespace BMX2WAV::Mainstay {
  // -- EntryDialog ------------------------------------------------------
  class EntryDialog : public TtDialog {
  public:
    explicit EntryDialog( Entry* entry, MainFrame& parent );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

    void SetUsingParameterLabelFromParameter( void );

  private:
    Entry*     entry_;
    MainFrame& parent_;

    using PathLabel = TtWindowWithStyle<TtStatic, SS_PATHELLIPSIS>;
    using CustomLabel = TtWindowWithStyle<TtStatic, SS_SUNKEN | SS_CENTER | SS_CENTERIMAGE>;
    using CustomList = TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_SHOWSELALWAYS>;

    PathLabel   path_label_;
    TtStatic    using_parameter_title_label_;
    CustomLabel using_parameter_label_;
    TtButton    use_common_parameter_button_;
    TtButton    use_individual_parameter_button_;
    TtButton    convert_button_;
    CustomList  list_;
  };
}
