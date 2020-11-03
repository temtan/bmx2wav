// mainstay/main_squirrel_vm.h

#pragma once

#include "tt_menu.h"

#include "squirrel_vm_base.h"

#include "core/convert_parameter.h"
#include "core/converter.h"
#include "mainstay/entry.h"


namespace BMX2WAV::Mainstay {
  // -- SquirrelVM -------------------------------------------------------
  class SquirrelVM : public BMX2WAV::SquirrelVMBase {
  public:
    static const char* const CLASS_FILENAME;

  private:
    static TtSquirrel::VirtualMachine::Closure ConvertClosure( std::function<int ( SquirrelVM& )> closure );

  public:
    explicit SquirrelVM( TtWindow* parent_window );

    void SetCommonParameter( const Core::ConvertParameter* parameter );
    void InitializeForMainstay( void );

    std::vector<std::string> GetColumnGroupNames( void );

    void CallEntryConstructorAndPushIt( Entry* entry );
    void CallConvertParameterConstructorAndPushIt( const Core::ConvertParameter& parameter );

    void CallParseAsBmsData( Entry* entry );
    void CallParseAsBmsDataOnce( Entry* entry );

    bool        FirstColumnIsNotNull( void );
    std::string GetFirstColumnName( void );
    std::string CallFirstColumnDisplayCell( Entry* entry );
    int         CallFirstColumnCompare( Entry* x, Entry* y );

    std::vector<std::string> GetColumnNamesFromColumnGroupIndex( unsigned int index );
    std::string CallColumnDisplayCell( unsigned int group_index, unsigned int column_index, Entry* entry );
    int CallColumnCompare( unsigned int group_index, unsigned int column_index, Entry* x, Entry* y );

    TtSubMenuCommandMaker GetSubMenuCommandMakerOfEntryProcessors( void );
    void CallExecuteOf( TtSquirrel::Object object, Entry* entry );

    std::vector<std::pair<std::string, std::string>> GetEntryDialogListItems( Entry* entry );

    // -- for Converter --------------------------------------------------
    void CallConverterConstructor( Core::Converter& converter );

    // -- for callback functions -----
    class TagName {
    public:
      // progress
      static TagName before_initialize;
      static TagName after_initialize;
      static TagName before_parse;
      static TagName after_parse;
      static TagName before_read_audio_files;
      static TagName after_read_audio_files;
      static TagName before_mixin_waves;
      static TagName after_mixin_waves;
      static TagName before_affect_wave;
      static TagName after_affect_wave;
      static TagName before_output_to_file;
      static TagName after_output_to_file;
      static TagName before_finalize;
      static TagName after_finalize;

      // point progress
      static TagName decide_output_file_path;
      static TagName decide_audio_file_path;
      static TagName audio_file_read_start;
      static TagName audio_file_read_end;
      static TagName complete_normalize;
      static TagName after_process;

      // abort
      static TagName aborted;

      // exception
      static TagName exception_occurred;
      static TagName parser_exception_occurred;

    private:
      friend class SquirrelVM;
      explicit TagName( const std::string& name ) : name_( name ) {}

      const std::string& name_;
    };

    void SetBmsDataToConverterObject( Core::Converter& converter );
    void SetOutputFilePathToConverterObject( Core::Converter& converter );

    // -- for CallCallbackFunction -----
    bool ExistCallbackFunction( TagName& tag );

    // template <> void TtSquirrel::VirtualMachine::PushAuto<BL::Word&>( BL::Word& word );
    // template <> void TtSquirrel::VirtualMachine::PushAuto<ConvertException&>( ConvertException& exception );
    // template <> void TtSquirrel::VirtualMachine::PushAuto<ConvertBmsDescriptionException&>( ConvertBmsDescriptionException& exception );

    void EachPush( void ) {};

    template <class Head, class... Rest>
    void EachPush( Head&& head, Rest&&... rest ) {
      this->PushAuto<Head>( head );
      this->EachPush( std::forward<Rest>( rest )... );
    }

    void CallTagNameInCallbackTable( TagName& tag, TtSquirrel::VirtualMachine::ParametersOperation parameters );

    void CallErrorCallback( TagName& tag, Core::Converter& converter, ConvertException& exception );

    template <class... Args>
    void CallCallbackFunctionAndPushReturnValue( TagName& tag, Core::Converter& converter, Args... args ) {
      this->CallTagNameInCallbackTable( tag, [&] () -> unsigned int {
        this->Native().PushRootTable();
        this->PushObject( converter.squirrel_object_.value() );
        this->EachPush( args... );
        return sizeof...( args ) + 2;
      } );
    }

    template <class... Args>
    void CallCallbackFunctionAndNoReturnValue( TagName& tag, Core::Converter& converter, Args... args ) {
      TtSquirrel::StackRecoverer recoverer( this );
      this->CallCallbackFunctionAndPushReturnValue<Args...>( tag, converter, args... );
    }

    template <class ReturnValueType, class... Args>
    ReturnValueType CallCallbackFunctionAndGetReturnValueAs( TagName& tag, Core::Converter& converter, Args... args ) {
      TtSquirrel::StackRecoverer recoverer( this );
      this->CallCallbackFunctionAndPushReturnValue( tag, converter, args... );
      return this->GetAsFromTop<ReturnValueType>();
    }

  private:
    const Core::ConvertParameter* common_parameter_;
  };
}
