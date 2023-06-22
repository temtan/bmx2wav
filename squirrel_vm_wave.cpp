// squirrel_vm_wave.cpp

#include "tt_string.h"

#include "core/wave.h"
#include "core/wave_maker.h"

#include "squirrel_vm_base.h"

using namespace BMX2WAV;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( Wave );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( write_to_file );
  DEFINE_PARAMETER_NAME_STRING( write_to_file_as_ogg );
  DEFINE_PARAMETER_NAME_STRING( get_length );
  DEFINE_PARAMETER_NAME_STRING( get_positive_length );
  DEFINE_PARAMETER_NAME_STRING( get_negative_length );
  DEFINE_PARAMETER_NAME_STRING( change_volume );
  DEFINE_PARAMETER_NAME_STRING( peak_normalize );
  DEFINE_PARAMETER_NAME_STRING( get_over_normalize_ratio );
  DEFINE_PARAMETER_NAME_STRING( over_normalize );
  DEFINE_PARAMETER_NAME_STRING( trim );
  DEFINE_PARAMETER_NAME_STRING( trim_front_silence );
  DEFINE_PARAMETER_NAME_STRING( insert_front_silence );
  DEFINE_PARAMETER_NAME_STRING( insert_front_silence_by_second );
  DEFINE_PARAMETER_NAME_STRING( fade_in );
  DEFINE_PARAMETER_NAME_STRING( fade_out );
  DEFINE_PARAMETER_NAME_STRING( clear );
  DEFINE_PARAMETER_NAME_STRING( mixin_at );
  // DEFINE_PARAMETER_NAME_STRING(  );
}


// -- SquirrelVM ---------------------------------------------------------
void
SquirrelVMBase::InitializeWaveClass( void )
{
  // -- Wave 実装 -----
  this->NewSlotOfRootTableByString(
    Tag::Wave,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<std::shared_ptr<Core::Wave>>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            while ( vm.Native().GetTopIndex() > 2 ) {
              vm.Native().PopTop();
            }
            // "std::shared_ptr" の生ポインタを扱う
            std::shared_ptr<Core::Wave>* pp_wave = new std::shared_ptr<Core::Wave>( std::make_shared<Core::Wave>() );

            if ( vm.Native().GetTopIndex() == 2 ) {
              std::string path = vm.GetAsFromTop<std::string>();
              vm.Native().PopTop();

              Core::WaveMaker wave_maker( true );
              if ( TtString::EndWith( TtString::ToLower( path ), ".ogg" ) ) {
                *pp_wave = wave_maker.MakeNewWaveFromOggFile( path );
              }
              else {
                *pp_wave = wave_maker.MakeNewWaveFromWavFile( path );
              }
            }

            vm.Native().SetInstanceUserPointer( TtSquirrel::Const::StackTop, pp_wave );
            SQRELEASEHOOK hook = [] ( SQUserPointer p, SQInteger ) -> SQInteger {
              delete static_cast<std::shared_ptr<Core::Wave>*>( p );
              return 0;
            };
            vm.Native().SetReleaseHook( TtSquirrel::Const::StackTop, hook );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( -1, "xs" );
        } );

      // -- get_length 実装 -----
      this->NewSlotOfTopByString(
        Tag::get_length,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            vm.Native().PushInteger( self.GetLength() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- get_positive_length 実装 -----
      this->NewSlotOfTopByString(
        Tag::get_positive_length,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            vm.Native().PushInteger( self.GetPositiveLength() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- get_negative_length 実装 -----
      this->NewSlotOfTopByString(
        Tag::get_negative_length,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            vm.Native().PushInteger( self.GetNegativeLength() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- change_volume 実装 -----
      this->NewSlotOfTopByString(
        Tag::change_volume,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            float ratio = vm.GetAsFromTop<float>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.ChangeVolume( ratio );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xf" );
        } );

      // -- mixin_at 実装 -----
      this->NewSlotOfTopByString(
        Tag::mixin_at,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            while ( vm.Native().GetTopIndex() > 4 ) {
              vm.Native().PopTop();
            }

            int other_start = 0;
            if ( vm.Native().GetTopIndex() == 4 ) {
              other_start = vm.GetAsFromTop<int>();
              vm.Native().PopTop();
            }
            Core::Wave& other = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            int start = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.MixinAt( start, other, other_start );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( -3, "xixi" );
        } );


      // -- peak_normalize 実装 -----
      this->NewSlotOfTopByString(
        Tag::peak_normalize,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            vm.Native().PushFloat( static_cast<float>( self.PeakNormalize() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- get_over_normalize_ratio -----
      this->NewSlotOfTopByString(
        Tag::get_over_normalize_ratio,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            float over_ratio = vm.GetAsFromTop<float>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            vm.Native().PushFloat( static_cast<float>( self.GetOverNormalizeRatio( over_ratio ) ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xf" );
        } );

      // -- over_normalize -----
      this->NewSlotOfTopByString(
        Tag::over_normalize,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            float over_ratio = vm.GetAsFromTop<float>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            vm.Native().PushFloat( static_cast<float>( self.OverNormalize( over_ratio ) ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xf" );
        } );

      // -- trim -----
      this->NewSlotOfTopByString(
        Tag::trim,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int end = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            int start = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.Trim( start, end );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "xii" );
        } );

      // -- trim_front_silence -----
      this->NewSlotOfTopByString(
        Tag::trim_front_silence,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            self.TrimFrontSilence();
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- insert_front_silence -----
      this->NewSlotOfTopByString(
        Tag::insert_front_silence,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int length = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.InsertFrontSilence( length );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xi" );
        } );

      // -- insert_front_silence_by_second -----
      this->NewSlotOfTopByString(
        Tag::insert_front_silence_by_second,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            float second = vm.GetAsFromTop<float>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.InsertFrontSilenceBySecond( second );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xf" );
        } );

      // -- fade_in -----
      this->NewSlotOfTopByString(
        Tag::fade_in,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            while ( vm.Native().GetTopIndex() > 4 ) {
              vm.Native().PopTop();
            }

            float ratio = 0.0;
            if ( vm.Native().GetTopIndex() == 4 ) {
              ratio = vm.GetAsFromTop<float>();
              vm.Native().PopTop();
            }
            int end = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            int start = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.FadeIn( start, end, ratio );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( -3, "xiif" );
        } );

      // -- fade_out -----
      this->NewSlotOfTopByString(
        Tag::fade_out,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            while ( vm.Native().GetTopIndex() > 4 ) {
              vm.Native().PopTop();
            }

            float ratio = 0.0;
            if ( vm.Native().GetTopIndex() == 4 ) {
              ratio = vm.GetAsFromTop<float>();
              vm.Native().PopTop();
            }
            int end = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            int start = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            self.FadeOut( start, end, ratio );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( -3, "xiif" );
        } );

      // -- clear -----
      this->NewSlotOfTopByString(
        Tag::clear,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );
            self.Clear();
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- write_to_file 実装 -----
      this->NewSlotOfTopByString(
        Tag::write_to_file,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string path = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            try {
              self.WriteToFile( path );
            }
            catch ( Exception& e ) {
              vm.Native().ThrowError( e.GetMessage() );
              return TtSquirrel::Const::ErrorClosure;
            }
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xs" );
        } );

      // -- write_to_file_as_ogg 実装 -----
      this->NewSlotOfTopByString(
        Tag::write_to_file_as_ogg,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            float quality = vm.GetAsFromTop<float>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            Core::Wave& self = **vm.GetInstanceUserPointerAs<std::shared_ptr<Core::Wave>>( TtSquirrel::Const::StackTop );

            try {
              self.WriteToFileAsOgg( path, quality );
            }
            catch ( Exception& e ) {
              vm.Native().ThrowError( e.GetMessage() );
              return TtSquirrel::Const::ErrorClosure;
            }
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "xsf" );
        } );

    } );
}
