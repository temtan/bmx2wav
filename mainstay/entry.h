// mainstay/entry.h

#pragma once

#include <optional>
#include <memory>

#include "tt_dialog.h"
#include "tt_list_view.h"

#include "tt_squirrel_virtual_machine.h" 

#include "base/bms_data.h"

#include "core/convert_parameter.h"


namespace BMX2WAV::Mainstay {
  // -- Entry ------------------------------------------------------------
  class Entry {
  public:
    explicit Entry( void );
    explicit Entry( const std::string& path );

    void ParseAsBmsData( void );
    void ParseAsBmsDataOnce( void );

  public:
    std::string                           path_;
    std::optional<Core::ConvertParameter> parameter_;
    std::shared_ptr<BL::BmsData>          bms_data_;

    std::optional<TtSquirrel::Object>     squirrel_object_;
  };
}
