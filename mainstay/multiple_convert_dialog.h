// mainstay/multiple_convert_dialog.h

#pragma once

#include "tt_dialog.h"

#include "converter_dialog.h"


namespace BMX2WAV::Mainstay {
  class MainFrame;

  // -- MultipleConvertDialog --------------------------------------------
  class MultipleConvertDialog : public TtDialogModeless {
  public:
    explicit MultipleConvertDialog( MainFrame& parent, const std::vector<Entry*>& entries );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

    void ShowLogDialog( void );

  public:
    void ConvertStart( void );

  private:
    // -- ConverterDialogCustom ------------------------------------------
    class ConverterDialogCustom : public ConverterDialog {
    public:
      explicit ConverterDialogCustom( const Core::ConvertParameter& parameter, TtCriticalSection& lock );

      virtual bool Created( void ) override;

    private:
      TtCriticalSection& lock_;
    };

    MainFrame&               parent_;
    std::vector<Entry*>      entries_;
    std::vector<std::string> log_table_;

    ConverterDialogCustom* converting_dialog_;

    std::atomic_bool  need_to_abort_;
    std::atomic_bool  is_converting_;
    TtCriticalSection lock_;

    using StatusLabel = TtWindowWithStyle<TtStatic, SS_CENTERIMAGE>;
    using CustomList = TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_SHOWSELALWAYS>;
    StatusLabel status_label_;
    TtButton    abort_button_;
    CustomList  list_;

    OutputDialog log_dialog_;
    TtSubMenu    show_log_menu_;
  };
}
