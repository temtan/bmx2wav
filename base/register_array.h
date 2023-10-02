// base/register_array.h

#pragma once

#include <array>
#include <optional>

#include "common.h"
#include "base/word.h"


namespace BMX2WAV::BL {
  // -- RegisterArray ----------------------------------------------------
  class RegisterArray {
  public:
    using Array = std::array<std::optional<std::string>, Const::WORD_MAX_COUNT>;

    explicit RegisterArray( const std::string& name );

    const std::string& GetName( void ) const;
    Array& GetArray( void );

    void Register( BL::Word position, const std::string& value );

    std::string At( BL::Word position ) const;
    std::string operator []( BL::Word position ) const;

    bool CanApply( const std::string& str );

    unsigned int GetExistCount( void ) const;

    bool IsExists( BL::Word position ) const;
    bool IsNotExists( BL::Word position ) const;

  private:
    const std::string name_;
    Array             array_;
  };
}
