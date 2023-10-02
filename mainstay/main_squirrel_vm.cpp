// mainstay/main_squirrel_vm.cpp

#include "tt_string.h"

#include "exception.h"

#include "mainstay/main_squirrel_vm.h"

using namespace BMX2WAV;

namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  // DEFINE_PARAMETER_NAME_STRING(  );
  DEFINE_PARAMETER_NAME_STRING( Main );
  DEFINE_PARAMETER_NAME_STRING( column_groups );
  DEFINE_PARAMETER_NAME_STRING( name );
  DEFINE_PARAMETER_NAME_STRING( columns );
  DEFINE_PARAMETER_NAME_STRING( display_cell );
  DEFINE_PARAMETER_NAME_STRING( path );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( bms_data );
  DEFINE_PARAMETER_NAME_STRING( Entry );
  DEFINE_PARAMETER_NAME_STRING( parse_as_bms_data );
  DEFINE_PARAMETER_NAME_STRING( parse_as_bms_data_once );
  DEFINE_PARAMETER_NAME_STRING( ConvertParameter );
  DEFINE_PARAMETER_NAME_STRING( get_individual_parameter );
  DEFINE_PARAMETER_NAME_STRING( get_common_parameter );
  DEFINE_PARAMETER_NAME_STRING( first_column );
  DEFINE_PARAMETER_NAME_STRING( entry_processors );
  DEFINE_PARAMETER_NAME_STRING( SubMenu );
  DEFINE_PARAMETER_NAME_STRING( items );
  DEFINE_PARAMETER_NAME_STRING( execute );
  DEFINE_PARAMETER_NAME_STRING( entry_dialog_list_items );
  DEFINE_PARAMETER_NAME_STRING( use_script_file );
  DEFINE_PARAMETER_NAME_STRING( script_file_path );
  DEFINE_PARAMETER_NAME_STRING( output_log );
  DEFINE_PARAMETER_NAME_STRING( log_file_path );
  DEFINE_PARAMETER_NAME_STRING( output_as_ogg );
  DEFINE_PARAMETER_NAME_STRING( ogg_base_quality );
  DEFINE_PARAMETER_NAME_STRING( never_overwrite_output_file );
  DEFINE_PARAMETER_NAME_STRING( remove_can_not_use_character_as_file_path );
  DEFINE_PARAMETER_NAME_STRING( output_file_template );
  DEFINE_PARAMETER_NAME_STRING( ignore_bga_channel );
  DEFINE_PARAMETER_NAME_STRING( not_nesting_if_statement );
  DEFINE_PARAMETER_NAME_STRING( cancel_at_resounding );
  DEFINE_PARAMETER_NAME_STRING( convert_start_bar_number );
  DEFINE_PARAMETER_NAME_STRING( convert_end_bar_number );
  DEFINE_PARAMETER_NAME_STRING( do_triming );
  DEFINE_PARAMETER_NAME_STRING( triming_start_bar_number );
  DEFINE_PARAMETER_NAME_STRING( triming_end_bar_number );
  DEFINE_PARAMETER_NAME_STRING( remove_front_silence );
  DEFINE_PARAMETER_NAME_STRING( insert_front_silence );
  DEFINE_PARAMETER_NAME_STRING( insert_front_silence_second );
  DEFINE_PARAMETER_NAME_STRING( normalize_kind );
  DEFINE_PARAMETER_NAME_STRING( normalize_over_ppm );
  DEFINE_PARAMETER_NAME_STRING( volume );
  DEFINE_PARAMETER_NAME_STRING( execute_after_process );
  DEFINE_PARAMETER_NAME_STRING( after_process_execute_file );
  DEFINE_PARAMETER_NAME_STRING( after_process_execute_arguments );
  DEFINE_PARAMETER_NAME_STRING( after_process_start_on_background );
  DEFINE_PARAMETER_NAME_STRING( after_process_wait_for_process_exit );
  DEFINE_PARAMETER_NAME_STRING( after_process_delete_output_file );
  DEFINE_PARAMETER_NAME_STRING( immediately_abort_error_level );
  DEFINE_PARAMETER_NAME_STRING( delay_abort_error_level );
  DEFINE_PARAMETER_NAME_STRING( bar_resolution_max );
  DEFINE_PARAMETER_NAME_STRING( usable_memory_mega_byte_size );
  DEFINE_PARAMETER_NAME_STRING( compare );
  DEFINE_PARAMETER_NAME_STRING( Converter );
  DEFINE_PARAMETER_NAME_STRING( parameter );
  DEFINE_PARAMETER_NAME_STRING( translate_template_path );
  DEFINE_PARAMETER_NAME_STRING( input_file_path );
  DEFINE_PARAMETER_NAME_STRING( output_file_path );
  DEFINE_PARAMETER_NAME_STRING( get_processing_time );
  DEFINE_PARAMETER_NAME_STRING( read_as_utf8 );

  // -- for callback functions -----
  DEFINE_PARAMETER_NAME_STRING( callback_table );
  DEFINE_PARAMETER_NAME_STRING( decide_output_file_path );
  DEFINE_PARAMETER_NAME_STRING( after_process );
  DEFINE_PARAMETER_NAME_STRING( decide_audio_file_path );
  DEFINE_PARAMETER_NAME_STRING( before_initialize );
  DEFINE_PARAMETER_NAME_STRING( after_initialize );
  DEFINE_PARAMETER_NAME_STRING( before_parse );
  DEFINE_PARAMETER_NAME_STRING( after_parse );
  DEFINE_PARAMETER_NAME_STRING( before_read_audio_files );
  DEFINE_PARAMETER_NAME_STRING( after_read_audio_files );
  DEFINE_PARAMETER_NAME_STRING( before_mixin_waves );
  DEFINE_PARAMETER_NAME_STRING( after_mixin_waves );
  DEFINE_PARAMETER_NAME_STRING( before_affect_wave );
  DEFINE_PARAMETER_NAME_STRING( after_affect_wave );
  DEFINE_PARAMETER_NAME_STRING( before_output_to_file );
  DEFINE_PARAMETER_NAME_STRING( after_output_to_file );
  DEFINE_PARAMETER_NAME_STRING( before_finalize );
  DEFINE_PARAMETER_NAME_STRING( after_finalize );
  DEFINE_PARAMETER_NAME_STRING( exception_occurred );
  DEFINE_PARAMETER_NAME_STRING( parser_exception_occurred );
  DEFINE_PARAMETER_NAME_STRING( audio_file_read_start );
  DEFINE_PARAMETER_NAME_STRING( audio_file_read_end );
  DEFINE_PARAMETER_NAME_STRING( complete_normalize );
  DEFINE_PARAMETER_NAME_STRING( aborted );
}

// -- SquirrelVM ---------------------------------------------------------
const char* const
Mainstay::SquirrelVM::CLASS_FILENAME = "bmx2wav.class.nut";

TtSquirrel::VirtualMachine::Closure
Mainstay::SquirrelVM::ConvertClosure( std::function<int ( SquirrelVM& )> closure )
{
  return [closure] ( TtSquirrel::VirtualMachine& vm ) -> int {
    return closure( *dynamic_cast<SquirrelVM*>( &SquirrelVMBase::GetVM( vm ) ) );
  };
}


Mainstay::SquirrelVM::SquirrelVM( TtWindow* parent_window ) :
SquirrelVMBase( parent_window ),
common_parameter_( nullptr )
{
}


void
Mainstay::SquirrelVM::SetCommonParameter( const Core::ConvertParameter* parameter )
{
  common_parameter_ = parameter;
}


void
Mainstay::SquirrelVM::InitializeForMainstay( void )
{
  // -- Main 実装 ----------
  this->NewSlotOfRootTableByString( Tag::Main, [&] () { this->Native().NewTable(); } );
  {
    TtSquirrel::StackRecoverer recoverer( this );
    this->GetByStringFromRootTable( Tag::Main );

    // -- ConvertParameter ----------
    this->NewSlotOfTopByString(
      Tag::ConvertParameter,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->Native().NewClass( false );
        this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Core::ConvertParameter>() );

        // -- constructor 定義 -----
        this->NewSlotOfTopByString(
          Tag::constructor,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

              Core::ConvertParameter& self = *vm.GetAsPointerOf<Core::ConvertParameter>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();

              vm.SetStringToTopByString(  Tag::input_file_path,                           self.input_file_path_ );
              vm.SetStringToTopByString(  Tag::name,                                      self.name_ );
              vm.SetBooleanToTopByString( Tag::use_script_file,                           self.use_script_file_ );
              vm.SetStringToTopByString(  Tag::script_file_path,                          self.script_file_path_ );
              vm.SetBooleanToTopByString( Tag::output_log,                                self.output_log_ );
              vm.SetStringToTopByString(  Tag::log_file_path,                             self.log_file_path_ );
              vm.SetBooleanToTopByString( Tag::output_as_ogg,                             self.output_as_ogg_ );
              vm.SetFloatToTopByString(   Tag::ogg_base_quality,                          static_cast<float>( self.ogg_base_quality_ ) );
              vm.SetBooleanToTopByString( Tag::never_overwrite_output_file,               self.never_overwrite_output_file_ );
              vm.SetBooleanToTopByString( Tag::remove_can_not_use_character_as_file_path, self.remove_can_not_use_character_as_file_path_ );
              vm.SetStringToTopByString(  Tag::output_file_template,                      self.output_file_template_ );
              vm.SetBooleanToTopByString( Tag::read_as_utf8,                              self.read_as_utf8_ );
              vm.SetBooleanToTopByString( Tag::ignore_bga_channel,                        self.ignore_bga_channel_ );
              vm.SetBooleanToTopByString( Tag::not_nesting_if_statement,                  self.not_nesting_if_statement_ );
              vm.SetBooleanToTopByString( Tag::cancel_at_resounding,                      self.cancel_at_resounding_ );
              vm.SetIntegerToTopByString( Tag::convert_start_bar_number,                  self.convert_start_bar_number_ );
              vm.SetIntegerToTopByString( Tag::convert_end_bar_number,                    self.convert_end_bar_number_ );
              vm.SetBooleanToTopByString( Tag::do_triming,                                self.do_triming_ );
              vm.SetIntegerToTopByString( Tag::triming_start_bar_number,                  self.triming_start_bar_number_ );
              vm.SetIntegerToTopByString( Tag::triming_end_bar_number,                    self.triming_end_bar_number_ );
              vm.SetBooleanToTopByString( Tag::remove_front_silence,                      self.remove_front_silence_ );
              vm.SetBooleanToTopByString( Tag::insert_front_silence,                      self.insert_front_silence_ );
              vm.SetFloatToTopByString(   Tag::insert_front_silence_second,               static_cast<float>( self.insert_front_silence_second_ ) );
              vm.SetIntegerToTopByString( Tag::normalize_kind,                            self.normalize_kind_ );
              vm.SetIntegerToTopByString( Tag::normalize_over_ppm,                        self.normalize_over_ppm_ );
              vm.SetIntegerToTopByString( Tag::volume,                                    self.volume_ );
              vm.SetBooleanToTopByString( Tag::execute_after_process,                     self.execute_after_process_ );
              vm.SetStringToTopByString(  Tag::after_process_execute_file,                self.after_process_execute_file_ );
              vm.SetStringToTopByString(  Tag::after_process_execute_arguments,           self.after_process_execute_arguments_ );
              vm.SetBooleanToTopByString( Tag::after_process_start_on_background,         self.after_process_start_on_background_ );
              vm.SetBooleanToTopByString( Tag::after_process_wait_for_process_exit,       self.after_process_wait_for_process_exit_ );
              vm.SetBooleanToTopByString( Tag::after_process_delete_output_file,          self.after_process_delete_output_file_ );
              vm.SetIntegerToTopByString( Tag::immediately_abort_error_level,             self.immediately_abort_error_level_ );
              vm.SetIntegerToTopByString( Tag::delay_abort_error_level,                   self.delay_abort_error_level_ );
              vm.SetIntegerToTopByString( Tag::bar_resolution_max,                        self.bar_resolution_max_ );
              vm.SetIntegerToTopByString( Tag::usable_memory_mega_byte_size,              self.usable_memory_mega_byte_size_ );

              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 2, "xp" );
          } );

        // -- プロパティ
        this->NewNullSlotOfTopByString( Tag::input_file_path );
        this->NewNullSlotOfTopByString( Tag::name );
        this->NewNullSlotOfTopByString( Tag::use_script_file );
        this->NewNullSlotOfTopByString( Tag::script_file_path );
        this->NewNullSlotOfTopByString( Tag::output_log );
        this->NewNullSlotOfTopByString( Tag::log_file_path );
        this->NewNullSlotOfTopByString( Tag::output_as_ogg );
        this->NewNullSlotOfTopByString( Tag::ogg_base_quality );
        this->NewNullSlotOfTopByString( Tag::never_overwrite_output_file );
        this->NewNullSlotOfTopByString( Tag::remove_can_not_use_character_as_file_path );
        this->NewNullSlotOfTopByString( Tag::output_file_template );
        this->NewNullSlotOfTopByString( Tag::read_as_utf8 );
        this->NewNullSlotOfTopByString( Tag::ignore_bga_channel );
        this->NewNullSlotOfTopByString( Tag::not_nesting_if_statement );
        this->NewNullSlotOfTopByString( Tag::cancel_at_resounding );
        this->NewNullSlotOfTopByString( Tag::convert_start_bar_number );
        this->NewNullSlotOfTopByString( Tag::convert_end_bar_number );
        this->NewNullSlotOfTopByString( Tag::do_triming );
        this->NewNullSlotOfTopByString( Tag::triming_start_bar_number );
        this->NewNullSlotOfTopByString( Tag::triming_end_bar_number );
        this->NewNullSlotOfTopByString( Tag::remove_front_silence );
        this->NewNullSlotOfTopByString( Tag::insert_front_silence );
        this->NewNullSlotOfTopByString( Tag::insert_front_silence_second );
        this->NewNullSlotOfTopByString( Tag::normalize_kind );
        this->NewNullSlotOfTopByString( Tag::normalize_over_ppm );
        this->NewNullSlotOfTopByString( Tag::volume );
        this->NewNullSlotOfTopByString( Tag::execute_after_process );
        this->NewNullSlotOfTopByString( Tag::after_process_execute_file );
        this->NewNullSlotOfTopByString( Tag::after_process_execute_arguments );
        this->NewNullSlotOfTopByString( Tag::after_process_start_on_background );
        this->NewNullSlotOfTopByString( Tag::after_process_wait_for_process_exit );
        this->NewNullSlotOfTopByString( Tag::after_process_delete_output_file );
        this->NewNullSlotOfTopByString( Tag::immediately_abort_error_level );
        this->NewNullSlotOfTopByString( Tag::delay_abort_error_level );
        this->NewNullSlotOfTopByString( Tag::bar_resolution_max );
        this->NewNullSlotOfTopByString( Tag::usable_memory_mega_byte_size );
      } );

    // -- Entry 実装 ----------
    this->NewSlotOfTopByString(
      Tag::Entry,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->Native().NewClass( false );
        this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Mainstay::Entry>() );

        // -- static compare 定義 -----
        this->NewSlotOfTopByString(
          Tag::compare,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Mainstay::Entry& y = *vm.GetInstanceUserPointerAs<Mainstay::Entry>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();
              Mainstay::Entry& x = *vm.GetInstanceUserPointerAs<Mainstay::Entry>( TtSquirrel::Const::StackTop );

              vm.Native().PushInteger( TtString::Compare( x.path_, y.path_ ) );
              return TtSquirrel::Const::ExistReturnValue;
            } ) );
            this->Native().SetParamsCheck( 3, ".xx" );
          }, true );

        // -- constructor 定義 -----
        this->NewSlotOfTopByString(
          Tag::constructor,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

              Mainstay::Entry& self = *vm.GetAsPointerOf<Mainstay::Entry>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();

              self.squirrel_object_ = vm.GetStackTopObject();

              vm.SetStringToTopByString( Tag::path, self.path_ );

              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 2, "xp" );
          } );

        // -- parse_as_bms_data 定義
        this->NewSlotOfTopByString(
          Tag::parse_as_bms_data,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Entry& self = *vm.GetInstanceUserPointerAs<Mainstay::Entry>( TtSquirrel::Const::StackTop );

              self.ParseAsBmsData();
              /* TODO 見直し
                if ( self.GetBmsData().has_fatal_error_ ) {
                  vm.CallPuts( "BMS Parse Error" );
                  vm.CallPuts( "File : " + self.GetBmsData().path_ );
                  vm.CallPuts( self.GetBmsData().TODO_last_error_message_ );
                }
               */

              vm.SetToTopByString( Tag::bms_data, [&] () { vm.CallBmsDataConstructorAndPushIt( self.bms_data_ ); } );
              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 1, "x" );
          } );

        // -- parse_as_bms_data_once 定義 -----
        this->NewSlotOfTopByString(
          Tag::parse_as_bms_data_once,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              TtSquirrel::StackRecoverer recoverer( &vm );

              vm.GetByStringFromTop( Tag::bms_data );
              if ( vm.GetTopType() == TtSquirrel::ObjectType::Null ) {
                vm.Native().PopTop();

                Entry& self = *vm.GetInstanceUserPointerAs<Mainstay::Entry>( TtSquirrel::Const::StackTop );
                self.ParseAsBmsDataOnce();
                vm.SetToTopByString( Tag::bms_data, [&] () { vm.CallBmsDataConstructorAndPushIt( self.bms_data_ ); } );
              }
              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 1, "x" );
          } );

        // -- get_individual_parameter 定義 -----
        this->NewSlotOfTopByString(
          Tag::get_individual_parameter,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Entry& self = *vm.GetInstanceUserPointerAs<Mainstay::Entry>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();
              if ( self.parameter_ ) {
                vm.CallConvertParameterConstructorAndPushIt( *self.parameter_ );
              }
              else {
                vm.Native().PushNull();
              }

              return TtSquirrel::Const::ExistReturnValue;
            } ) );
            this->Native().SetParamsCheck( 1, "x" );
          } );

        // -- プロパティ
        this->NewNullSlotOfTopByString( Tag::path );
        this->NewNullSlotOfTopByString( Tag::bms_data );
      } );

    // get_common_parameter 実装
    this->NewSlotOfTopByString(
      Tag::get_common_parameter,
      [&] () {
        this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
          if ( vm.common_parameter_ ) {
            vm.CallConvertParameterConstructorAndPushIt( *vm.common_parameter_ );
          }
          else {
            vm.Native().PushNull();
          }
          return TtSquirrel::Const::ExistReturnValue;
        } ) );
        this->Native().SetParamsCheck( 1, "." );
      } );

    // translate_template_path 実装
    this->NewSlotOfTopByString(
      Tag::translate_template_path,
      [&] () {
        this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
          std::shared_ptr<BL::BmsData> bms_data = *vm.GetInstanceUserPointerAs<std::shared_ptr<BL::BmsData>>( TtSquirrel::Const::StackTop );
          vm.Native().PopTop();

          bool output_as_ogg = vm.GetAsFromTop<bool>();
          vm.Native().PopTop();

          std::string input_path = vm.GetAsFromTop<std::string>();
          vm.Native().PopTop();

          std::string template_path = vm.GetAsFromTop<std::string>();
          vm.Native().PopTop();

          vm.Native().PushString( Core::TranslateTemplatePath( template_path, input_path, output_as_ogg, bms_data.get() ) );

          return TtSquirrel::Const::ExistReturnValue;
        } ) );
        this->Native().SetParamsCheck( 5, ".ssbx" );
      } );

    // 空配列定義
    this->NewSlotOfTopByString( Tag::column_groups,           [&] () { this->Native().NewArray( 0 );} );
    this->NewSlotOfTopByString( Tag::entry_dialog_list_items, [&] () { this->Native().NewArray( 0 );} );

    // --  entry_processors 定義 -----
    this->NewSlotOfTopByString( Tag::entry_processors, [&] () {
      this->CallAndPushReturnValue(
        [&] () { this->GetByStringFromRootTable( Tag::SubMenu ); },
        [&] () {
          this->Native().PushRootTable();
          this->Native().PushString( "" );
          this->Native().NewArray( 0 );
          return 3;
        } );
    } );

    // メンバ
    this->NewNullSlotOfTopByString( Tag::first_column );

    // -- for Converter --------------------------------------------------
    // コールバック用テーブル定義
    this->NewSlotOfTopByString(
      Tag::callback_table,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->Native().NewTable();

        // progress
        this->NewNullSlotOfTopByString( Tag::before_initialize );
        this->NewNullSlotOfTopByString( Tag::after_initialize );
        this->NewNullSlotOfTopByString( Tag::before_parse );
        this->NewNullSlotOfTopByString( Tag::after_parse );
        this->NewNullSlotOfTopByString( Tag::before_read_audio_files );
        this->NewNullSlotOfTopByString( Tag::after_read_audio_files );
        this->NewNullSlotOfTopByString( Tag::before_mixin_waves );
        this->NewNullSlotOfTopByString( Tag::after_mixin_waves );
        this->NewNullSlotOfTopByString( Tag::before_affect_wave );
        this->NewNullSlotOfTopByString( Tag::after_affect_wave );
        this->NewNullSlotOfTopByString( Tag::before_output_to_file );
        this->NewNullSlotOfTopByString( Tag::after_output_to_file );
        this->NewNullSlotOfTopByString( Tag::before_finalize );
        this->NewNullSlotOfTopByString( Tag::after_finalize );

        // point progress
        this->NewNullSlotOfTopByString( Tag::decide_output_file_path );
        this->NewNullSlotOfTopByString( Tag::decide_audio_file_path );
        this->NewNullSlotOfTopByString( Tag::audio_file_read_start );
        this->NewNullSlotOfTopByString( Tag::audio_file_read_end );
        this->NewNullSlotOfTopByString( Tag::complete_normalize );
        this->NewNullSlotOfTopByString( Tag::after_process );

        // abort
        this->NewNullSlotOfTopByString( Tag::aborted );

        // exception
        this->NewNullSlotOfTopByString( Tag::exception_occurred );
        this->NewNullSlotOfTopByString( Tag::parser_exception_occurred );
      } );

    // -- Converter ----------
    this->NewSlotOfTopByString(
      Tag::Converter,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->Native().NewClass( false );
        this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Core::Converter>() );

        // -- constructor 定義 -----
        this->NewSlotOfTopByString(
          Tag::constructor,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

              Core::Converter& self = *vm.GetAsPointerOf<Core::Converter>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();

              self.squirrel_object_ = vm.GetStackTopObject();

              vm.SetToTopByString( Tag::parameter, [&] () { vm.CallConvertParameterConstructorAndPushIt( self.GetConvertParameter() ); } );
              // bms_data は当初は null のまま
              // output_file_path は当初は null のまま

              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 2, "xp" );
          } );

        // -- get_processing_time 実装 -----
        this->NewSlotOfTopByString(
          Tag::get_processing_time,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Core::Converter& self = *vm.GetInstanceUserPointerAs<Core::Converter>( TtSquirrel::Const::StackTop );
              vm.Native().PushFloat( static_cast<float>( self.GetProcessingTime() ) );
              return TtSquirrel::Const::ExistReturnValue;
            } ) );
            this->Native().SetParamsCheck( 1, "x" );
          } );

        // -- プロパティ
        this->NewNullSlotOfTopByString( Tag::parameter );
        this->NewNullSlotOfTopByString( Tag::bms_data );
        this->NewNullSlotOfTopByString( Tag::output_file_path );
      } );
  } // -- Main 実装 End ----------

}
// -- InitializeForMainstay end ------------------------------------------


std::vector<std::string>
Mainstay::SquirrelVM::GetColumnGroupNames( void )
{
  std::vector<std::string> v;

  this->ArrayEach(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::column_groups} ); },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}


void
Mainstay::SquirrelVM::CallEntryConstructorAndPushIt( Entry* entry )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::Entry} ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( entry );
      return 2;
    } );
}

void
Mainstay::SquirrelVM::CallConvertParameterConstructorAndPushIt( const Core::ConvertParameter& parameter )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::ConvertParameter} ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &parameter );
      return 2;
    } );
}


void
Mainstay::SquirrelVM::CallParseAsBmsData( Entry* entry )
{
  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( [&] () { this->PushObject( entry->squirrel_object_.value() ); }, Tag::parse_as_bms_data );
    },
    [&] () {
      this->PushObject( entry->squirrel_object_.value() );
      return 1;
    } );
}

void
Mainstay::SquirrelVM::CallParseAsBmsDataOnce( Entry* entry )
{
  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( [&] () { this->PushObject( entry->squirrel_object_.value() ); }, Tag::parse_as_bms_data_once );
    },
    [&] () {
      this->PushObject( entry->squirrel_object_.value() );
      return 1;
    } );
}


bool
Mainstay::SquirrelVM::FirstColumnIsNotNull( void )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::first_column} );
  return this->GetTopType() != TtSquirrel::ObjectType::Null;
}

std::string
Mainstay::SquirrelVM::GetFirstColumnName( void )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::first_column} );
  if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  this->GetByStringFromTop( Tag::name );
  return this->ToStringFromTopAndGetAsString();
}

std::string
Mainstay::SquirrelVM::CallFirstColumnDisplayCell( Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::first_column} );
  if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }

  this->CallObjectOfGetByStringFromTopAndPushReturnValue(
    Tag::display_cell,
    [&] ( TtSquirrel::Object object ) {
      this->PushObject( object );
      this->PushObject( entry->squirrel_object_.value() );
      return 2;
    } );

  return this->ToStringFromTopAndGetAsString();
}

int
Mainstay::SquirrelVM::CallFirstColumnCompare( Entry* x, Entry* y )
{
  TtSquirrel::StackRecoverer recoverer( this );
  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::first_column} );
  if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }

  this->CallObjectOfGetByStringFromTopAndPushReturnValue(
    Tag::compare,
    [&] ( TtSquirrel::Object object ) {
      this->PushObject( object );
      this->PushObject( x->squirrel_object_.value() );
      this->PushObject( y->squirrel_object_.value() );
      return 3;
    } );

  return this->GetAsFromTop<int>();
}


std::vector<std::string>
Mainstay::SquirrelVM::GetColumnNamesFromColumnGroupIndex( unsigned int index )
{
  std::vector<std::string> v;

  this->ArrayEach(
    [&] () {
      this->GetByString(
        [&] () { this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::column_groups} ); }, index ); },
        Tag::columns );
    },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}

std::string
Mainstay::SquirrelVM::CallColumnDisplayCell( unsigned int group_index, unsigned int column_index, Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByInteger(
    [&] () { this->GetByString(
      [&] () {
        this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::column_groups} ); }, group_index );
      },
      Tag::columns );
    },
    column_index - 1 );

  this->CallObjectOfGetByStringFromTopAndPushReturnValue(
    Tag::display_cell,
    [&] ( TtSquirrel::Object object ) {
      this->PushObject( object );
      this->PushObject( entry->squirrel_object_.value() );
      return 2;
    } );

  return this->ToStringFromTopAndGetAsString();
}

int
Mainstay::SquirrelVM::CallColumnCompare( unsigned int group_index, unsigned int column_index, Entry* x, Entry* y )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByInteger(
    [&] () { this->GetByString(
      [&] () {
        this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::column_groups} ); }, group_index );
      },
      Tag::columns );
    },
    column_index - 1 );

  this->CallObjectOfGetByStringFromTopAndPushReturnValue(
    Tag::compare,
    [&] ( TtSquirrel::Object object ) {
      this->PushObject( object );
      this->PushObject( x->squirrel_object_.value() );
      this->PushObject( y->squirrel_object_.value() );
      return 3;
    } );

  return this->GetAsFromTop<int>();
}


TtSubMenuCommandMaker
Mainstay::SquirrelVM::GetSubMenuCommandMakerOfEntryProcessors( void )
{
  using ElementVector = TtSubMenuCommandMaker::ElementVector;
  using Item          = TtSubMenuCommandMaker::Item;
  using Separator     = TtSubMenuCommandMaker::Separator;
  using SubMenu       = TtSubMenuCommandMaker::SubMenu;

  class EntryProcessorItem : public Item {
  public:
    explicit EntryProcessorItem( const std::string& name, TtSquirrel::Object object ) : name_( name ), object_( object ) {}
    virtual std::string GetName( void ) const override {
      return name_;
    };
    virtual void AfterMake( TtMenuItem& item ) override {
      item.SetParameterAs<TtSquirrel::Object*>( &object_ );
    }

  private:
    const std::string  name_;
    TtSquirrel::Object object_;
  };

  TtSubMenuCommandMaker maker;
  std::function<void ( ElementVector& )> func = [this, &func] ( ElementVector& current ) {
    unsigned int max = this->Native().GetSize( TtSquirrel::Const::StackTop );
    for ( unsigned int i = 0; i < max; ++i ) {
      TtSquirrel::StackRecoverer recoverer( this );

      this->GetByIntegerFromTop( i );
      if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
        current.push_back( std::make_shared<Separator>() );
      }
      else if ( this->GetTopType() == TtSquirrel::ObjectType::Instance &&
                this->InstanceAtTopOf( [&] () { this->GetByStringFromRootTable( Tag::SubMenu ); } ) ) {
        TtSquirrel::StackRecoverer tmp( this );
        SubMenu sub;
        sub.name_ = this->GetByStringFromTopAndGetAs<std::string>( Tag::name );
        this->GetByStringFromTop( Tag::items );
        func( sub.elements_ );
        current.push_back( std::make_shared<SubMenu>( sub ) );
      }
      else {
        std::string name = this->GetByStringFromTopAndGetAs<std::string>( Tag::name );
        current.push_back( std::make_shared<EntryProcessorItem>( name, this->GetStackTopObject() ) );
      }
    }
  };
  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::entry_processors, Tag::items} );
  func( maker.GetRoot() );
  return maker;
}

void
Mainstay::SquirrelVM::CallExecuteOf( TtSquirrel::Object object, Entry* entry )
{
  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( [&] () { this->PushObject( object ); }, Tag::execute );
    },
    [&] () {
      this->PushObject( object );
      this->PushObject( entry->squirrel_object_.value() );
      return 2;
    } );
}


std::vector<std::pair<std::string, std::string>>
Mainstay::SquirrelVM::GetEntryDialogListItems( Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  std::vector<std::pair<std::string, std::string>> v;
  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::entry_dialog_list_items} );

  int count = this->Native().GetSize( TtSquirrel::Const::StackTop );

  for ( int i = 0; i < count; ++i ) {
    TtSquirrel::StackRecoverer recoverer_loop( this );
    this->GetByIntegerFromTop( i );

    std::string name = this->GetByStringFromTopAndGetAs<std::string>( Tag::name );

    this->CallObjectOfGetByStringFromTopAndPushReturnValue(
      Tag::display_cell,
      [&] ( TtSquirrel::Object object ) {
        this->PushObject( object );
        this->PushObject( entry->squirrel_object_.value() );
        return 2;
      } );
    std::string value = this->ToStringFromTopAndGetAsString();
    v.push_back( {name, value} );
  }
  return v;
}


// -- for Converter ------------------------------------------------------
void
Mainstay::SquirrelVM::CallConverterConstructor( Core::Converter& converter )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::Converter} ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &converter );
      return 2;
    } );
}


// -- for callback functions -----
#define DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( name ) Mainstay::SquirrelVM::TagName Mainstay::SquirrelVM::TagName::name( Tag::name )
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( decide_output_file_path );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( decide_audio_file_path );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_process );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_initialize );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_initialize );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_parse );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_parse );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_read_audio_files );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_read_audio_files );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_mixin_waves );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_mixin_waves );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_affect_wave );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_affect_wave );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_output_to_file );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_output_to_file );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( before_finalize );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( after_finalize );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( exception_occurred );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( parser_exception_occurred );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( audio_file_read_start );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( audio_file_read_end );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( complete_normalize );
DEFINE_CALLBACK_FUNCTIONS_TAG_NAME( aborted );


void
Mainstay::SquirrelVM::SetBmsDataToConverterObject( Core::Converter& converter )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->PushObject( converter.squirrel_object_.value() );
  this->SetToTopByString(
    Tag::bms_data,
    [&] () {
      this->CallBmsDataConstructorAndPushIt( converter.info_.bms_data_ );
    } );
}

void
Mainstay::SquirrelVM::SetOutputFilePathToConverterObject( Core::Converter& converter )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->PushObject( converter.squirrel_object_.value() );
  this->SetStringToTopByString( Tag::output_file_path, converter.info_.output_file_path_ );
}


bool
Mainstay::SquirrelVM::ExistCallbackFunction( TagName& tag )
{
  TtSquirrel::StackRecoverer recoverer( this );
  this->GetByStringFromRootTableJoined( {Tag::Main, Tag::callback_table, tag.name_} );
  return this->GetTopType() != TtSquirrel::ObjectType::Null;
}


template <>
void
TtSquirrel::VirtualMachine::PushAuto<BL::Word&>( BL::Word& word )
{
  this->PushAuto( word.ToInteger() );
}

template <>
void
TtSquirrel::VirtualMachine::PushAuto<ConvertException&>( ConvertException& exception )
{
  this->PushObject( exception.squirrel_object_.value() );
}

template <>
void
TtSquirrel::VirtualMachine::PushAuto<ConvertBmsDescriptionException&>( ConvertBmsDescriptionException& exception )
{
  this->PushObject( exception.squirrel_object_.value() );
}


void
Mainstay::SquirrelVM::CallTagNameInCallbackTable( TagName& tag, TtSquirrel::VirtualMachine::ParametersOperation parameters )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Main, Tag::callback_table, tag.name_} ); },
    parameters );
}


void
Mainstay::SquirrelVM::CallErrorCallback( TagName& tag, Core::Converter& converter, ConvertException& exception )
{
  this->CallTagNameInCallbackTable( tag, [&] () -> unsigned int {
    this->Native().PushRootTable();
    this->PushObject( converter.squirrel_object_.value() );
    this->PushObject( exception.squirrel_object_.value() );
    return 3;
  } );
}
