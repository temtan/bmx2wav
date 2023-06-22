// squirrel_vm_base.h

#pragma once

#include <unordered_map>
#include <functional>

#include "tt_window.h"

#include "tt_squirrel_virtual_machine.h"

#include "exception.h"
#include "base/bms_data.h"
#include "base/parser.h"


namespace BMX2WAV {
  // -- SquirrelVMBase ---------------------------------------------------
  class SquirrelVMBase : public TtSquirrel::VirtualMachine {
  public:
    static const char* const BASE_FILENAME;

  private:
    static std::unordered_map<HSQUIRRELVM, SquirrelVMBase*> HANDLE_TABLE;

    static void PrintFunctionStatic( HSQUIRRELVM vm, const SQChar* s, ... );

    static TtSquirrel::VirtualMachine::Closure ConvertClosure( std::function<int ( SquirrelVMBase& )> closure );

  public:
    static SquirrelVMBase& GetVM( HSQUIRRELVM vm );
    static SquirrelVMBase& GetVM( TtSquirrel::VirtualMachine& vm );

  public:
    explicit SquirrelVMBase( TtWindow* parent_window );
    virtual ~SquirrelVMBase();

    using PrintFunction = std::function<void ( const std::string& str )>;
    void SetPrintFunction( PrintFunction print_function );

    void Initialize( void );
    void InitializeWaveClass( void );
    void InitializeStringTable( void );

    void CallBmsDataContructorAndPushIt( std::shared_ptr<BL::BmsData> bms_data );
    void CallConvertExceptionConstructorAndPushIt( ConvertException& exception );
    void CallBmsDescriptionExceptionConstructorAndPushIt( ConvertBmsDescriptionException& exception );

  private:
    PrintFunction print_function_;

  protected:
    TtWindow* parent_window_;
  };
}
