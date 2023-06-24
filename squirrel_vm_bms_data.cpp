// squirrel_vm_bms_data.cpp

#include "squirrel_vm_base.h"

using namespace BMX2WAV;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( BmsData );
  DEFINE_PARAMETER_NAME_STRING( path );
  DEFINE_PARAMETER_NAME_STRING( headers );
  DEFINE_PARAMETER_NAME_STRING( object_count );
  DEFINE_PARAMETER_NAME_STRING( object_count_of_1P );
  DEFINE_PARAMETER_NAME_STRING( object_count_of_2P );
  DEFINE_PARAMETER_NAME_STRING( most_serious_error_level );
  DEFINE_PARAMETER_NAME_STRING( has_random_statement );
  DEFINE_PARAMETER_NAME_STRING( max_resolution );
  DEFINE_PARAMETER_NAME_STRING( bar_number_of_max_resolution );
  DEFINE_PARAMETER_NAME_STRING( calculate_playing_time );
  DEFINE_PARAMETER_NAME_STRING( Parser );
  DEFINE_PARAMETER_NAME_STRING( parse );
  DEFINE_PARAMETER_NAME_STRING( must_read_as_utf8 );
  DEFINE_PARAMETER_NAME_STRING( not_nesting_if_statement );
  DEFINE_PARAMETER_NAME_STRING( convert_to_file_format );
  // DEFINE_PARAMETER_NAME_STRING(  );
}


// -- SquirrelVM ---------------------------------------------------------
void
SquirrelVMBase::InitializeBmsDataClass( void )
{
  // -- BmsData 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::BmsData,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<std::shared_ptr<BL::BmsData>>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::shared_ptr<BL::BmsData> p_self = *vm.GetAsPointerOf<std::shared_ptr<BL::BmsData>>( TtSquirrel::Const::StackTop );
            BL::BmsData& self = *p_self;
            vm.Native().PopTop();

            // "std::shared_ptr" の生ポインタを扱う
            std::shared_ptr<BL::BmsData>* pp_self = new std::shared_ptr<BL::BmsData>( p_self );
            vm.Native().SetInstanceUserPointer( TtSquirrel::Const::StackTop, pp_self );
            SQRELEASEHOOK hook = [] ( SQUserPointer p, SQInteger ) -> SQInteger {
              delete static_cast<std::shared_ptr<BL::BmsData>*>( p );
              return 0;
            };
            vm.Native().SetReleaseHook( TtSquirrel::Const::StackTop, hook );

            vm.SetToTopByString(
              Tag::headers,
              [&] () {
                vm.Native().NewTable();
                for ( auto& one : self.headers_ ) {
                  vm.NewStringSlotOfTopByString( one.first, one.second );
                }
              } );

            vm.SetStringToTopByString(  Tag::path,                         self.path_ );
            vm.SetIntegerToTopByString( Tag::object_count,                 self.GetObjectCount() );
            vm.SetIntegerToTopByString( Tag::object_count_of_1P,           self.GetObjectCountOf1P() );
            vm.SetIntegerToTopByString( Tag::object_count_of_2P,           self.GetObjectCountOf2P() );
            vm.SetIntegerToTopByString( Tag::most_serious_error_level,     self.most_serious_error_level_ );
            vm.SetBooleanToTopByString( Tag::has_random_statement,         self.has_random_statement_ );
            vm.SetIntegerToTopByString( Tag::max_resolution,               self.GetMaxResolution() );
            vm.SetIntegerToTopByString( Tag::bar_number_of_max_resolution, self.GetBarNumberOfMaxResolution() );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::headers );
      this->NewNullSlotOfTopByString( Tag::path );
      this->NewNullSlotOfTopByString( Tag::object_count );
      this->NewNullSlotOfTopByString( Tag::object_count_of_1P );
      this->NewNullSlotOfTopByString( Tag::object_count_of_2P );
      this->NewNullSlotOfTopByString( Tag::most_serious_error_level );
      this->NewNullSlotOfTopByString( Tag::has_random_statement );
      this->NewNullSlotOfTopByString( Tag::max_resolution );
      this->NewNullSlotOfTopByString( Tag::bar_number_of_max_resolution );

      // -- convert_to_file_format 定義
      this->NewSlotOfTopByString(
        Tag::convert_to_file_format,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            BL::BmsData& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<BL::BmsData>>( TtSquirrel::Const::StackTop );

            vm.Native().PushString( self.ConvertToFileFormat() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- calculate_playing_time 定義
      this->NewSlotOfTopByString(
        Tag::calculate_playing_time,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            BL::BmsData& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<BL::BmsData>>( TtSquirrel::Const::StackTop );

            try {
              vm.Native().PushFloat( static_cast<float>( self.CalculatePlayingTime() ) );
            }
            catch ( TtException ) {
              vm.Native().PushNull();
            }
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

    } );


  // -- Parser 実装 -----
  this->NewSlotOfRootTableByString(
    Tag::Parser,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.SetBooleanToTopByString( Tag::must_read_as_utf8, false );
            vm.SetBooleanToTopByString( Tag::not_nesting_if_statement, true );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::must_read_as_utf8 );
      this->NewNullSlotOfTopByString( Tag::not_nesting_if_statement );

      // -- parse 定義
      this->NewSlotOfTopByString(
        Tag::parse,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string path = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            BL::Parser::Parser parser;
            parser.must_read_as_utf8_        = vm.GetByStringFromTopAndGetAs<bool>( Tag::must_read_as_utf8 );
            parser.not_nesting_if_statement_ = vm.GetByStringFromTopAndGetAs<bool>( Tag::not_nesting_if_statement );

            std::shared_ptr<BL::BmsData> bms_data;
            try {
              bms_data = parser.Parse( path );
            }
            catch ( TtException& ) {
              bms_data = std::make_shared<BL::BmsData>();
              bms_data->path_ = path;
              bms_data->most_serious_error_level_ = ErrorLevel::Internal;
            }
            catch ( ... ) {
              bms_data = std::make_shared<BL::BmsData>();
              bms_data->path_ = path;
              bms_data->most_serious_error_level_ = ErrorLevel::Internal;
            }

            vm.CallBmsDataContructorAndPushIt( bms_data );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xs" );
        } );
    } );
}


void
SquirrelVMBase::CallBmsDataContructorAndPushIt( std::shared_ptr<BL::BmsData> bms_data )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTable( Tag::BmsData ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &bms_data );
      return 2;
    } );
}
