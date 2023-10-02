// searcher/searcher_squirrel_vm.cpp

#include "item_id_list.h"
#include "searcher_common.h"

#include "searcher_squirrel_vm.h"

using namespace BMX2WAV;

namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  // DEFINE_PARAMETER_NAME_STRING(  );
  DEFINE_PARAMETER_NAME_STRING( Entry );
  DEFINE_PARAMETER_NAME_STRING( Searcher );
  DEFINE_PARAMETER_NAME_STRING( SubMenu );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( path );
  DEFINE_PARAMETER_NAME_STRING( items );
  DEFINE_PARAMETER_NAME_STRING( display_name );
  DEFINE_PARAMETER_NAME_STRING( type_name );
  DEFINE_PARAMETER_NAME_STRING( display_filters );
  DEFINE_PARAMETER_NAME_STRING( name );
  DEFINE_PARAMETER_NAME_STRING( filtering );
  DEFINE_PARAMETER_NAME_STRING( column_groups );
  DEFINE_PARAMETER_NAME_STRING( parse_as_bms_data );
  DEFINE_PARAMETER_NAME_STRING( parse_as_bms_data_once );
  DEFINE_PARAMETER_NAME_STRING( bms_data );
  DEFINE_PARAMETER_NAME_STRING( columns );
  DEFINE_PARAMETER_NAME_STRING( display_cell );
  DEFINE_PARAMETER_NAME_STRING( search_methods );
  DEFINE_PARAMETER_NAME_STRING( search );
  DEFINE_PARAMETER_NAME_STRING( search_hit );
  DEFINE_PARAMETER_NAME_STRING( by_each_directory );
  DEFINE_PARAMETER_NAME_STRING( compare );
  DEFINE_PARAMETER_NAME_STRING( entry_processors );
  DEFINE_PARAMETER_NAME_STRING( execute );
  DEFINE_PARAMETER_NAME_STRING( is_file );
  DEFINE_PARAMETER_NAME_STRING( is_directory );
  DEFINE_PARAMETER_NAME_STRING( add_directory_entry_to_list );
}


// -- SquirrelVM ---------------------------------------------------------
const char* const
Searcher::SquirrelVM::CLASS_FILENAME = "bmx2wav_searcher.class.nut";

TtSquirrel::VirtualMachine::Closure
Searcher::SquirrelVM::ConvertClosure( std::function<int ( SquirrelVM& )> closure )
{
  return [closure] ( TtSquirrel::VirtualMachine& vm ) -> int {
    return closure( *dynamic_cast<SquirrelVM*>( &SquirrelVMBase::GetVM( vm ) ) );
  };
}


void
Searcher::SquirrelVM::InitializeForSearcher( EntryPool* entry_pool )
{
  entry_pool_ = entry_pool;

  // -- Searcher 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::Searcher,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewTable();

      // -- add_directory_entry_to_list
      this->NewSlotOfTopByString(
        Tag::add_directory_entry_to_list,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [entry_pool = entry_pool_] ( SquirrelVM& vm ) -> int {
            Entry& tmp = *vm.GetInstanceUserPointerAs<Searcher::Entry>( TtSquirrel::Const::StackTop );
            if ( NOT( tmp.IsDirectory() ) ) {
              return static_cast<int>( vm.Native().ThrowError( "ディレクトリでないエントリオブジェクトを引数に add_directory_entry_to_list  関数は呼び出せません。" ) );
            }
            entry_pool->AddNewDirectoryEntry( tmp.CastToDirectory()->item_id_list_ );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".x" );
        }, true );

      // 空配列定義
      auto make_new_array = [&] ( const std::string& name ) {
        this->NewSlotOfTopByString( name, [&] () { Native().NewArray( 0 );} );
      };
      make_new_array( Tag::display_filters );
      make_new_array( Tag::column_groups );
      make_new_array( Tag::search_methods );

      // --  entry_processors定義 -----
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
    } );

  {
    TtSquirrel::StackRecoverer recoverer( this );
    this->GetByStringFromRootTable( Tag::Searcher );

    // -- Entry 実装 ----------
    this->NewSlotOfRootTableByString(
      Tag::Entry,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        Native().NewClass( false );
        Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Searcher::Entry>() );

        // -- static compare 定義 -----
        this->NewSlotOfTopByString(
          Tag::compare,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Entry& y = *vm.GetInstanceUserPointerAs<Searcher::Entry>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();
              Entry& x = *vm.GetInstanceUserPointerAs<Searcher::Entry>( TtSquirrel::Const::StackTop );

              vm.Native().PushInteger( x.entry_number_ - y.entry_number_ );
              return TtSquirrel::Const::ExistReturnValue;
            } ) );
            Native().SetParamsCheck( 3, ".xx" );
          }, true );

        // -- constructor 定義 -----
        this->NewSlotOfTopByString(
          Tag::constructor,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

              Searcher::Entry& self = *vm.GetAsPointerOf<Searcher::Entry>( TtSquirrel::Const::StackTop );
              vm.Native().PopTop();

              self.squirrel_object_.emplace( vm.GetStackTopObject() );

              vm.SetBooleanToTopByString( Tag::is_file,      self.IsFile() );
              vm.SetBooleanToTopByString( Tag::is_directory, self.IsDirectory() );

              vm.SetStringToTopByString( Tag::path,         self.GetPath() );
              vm.SetStringToTopByString( Tag::display_name, self.GetDisplayName() );
              vm.SetStringToTopByString( Tag::type_name,    self.GetTypeName() );

              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            Native().SetParamsCheck( 2, "xp" );
          } );

        // -- parse_as_bms_data 定義
        this->NewSlotOfTopByString(
          Tag::parse_as_bms_data,
          [&] () {
            this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
              Entry& tmp = *vm.GetInstanceUserPointerAs<Searcher::Entry>( TtSquirrel::Const::StackTop );
              if ( NOT( tmp.IsFile() ) ) {
                return static_cast<int>( vm.Native().ThrowError( "ファイルでないエントリオブジェクトには parse_as_bms_data 関数は呼び出せません。" ) );
              }
              FileEntry& self = *tmp.CastToFile();

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
            Native().SetParamsCheck( 1, "x" );
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

                Entry& tmp = *vm.GetInstanceUserPointerAs<Searcher::Entry>( TtSquirrel::Const::StackTop );
                if ( NOT( tmp.IsFile() ) ) {
                  return static_cast<int>( vm.Native().ThrowError( "ファイルでないエントリオブジェクトには parse_as_bms_data_once 関数は呼び出せません。" ) );
                }
                FileEntry& self = *tmp.CastToFile();

                self.ParseAsBmsDataOnce();
                vm.SetToTopByString( Tag::bms_data, [&] () { vm.CallBmsDataConstructorAndPushIt( self.bms_data_ ); } );
              }
              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            this->Native().SetParamsCheck( 1, "x" );
          } );

        // -- プロパティ
        this->NewNullSlotOfTopByString( Tag::is_file );
        this->NewNullSlotOfTopByString( Tag::is_directory );

        this->NewNullSlotOfTopByString( Tag::path );
        this->NewNullSlotOfTopByString( Tag::display_name );
        this->NewNullSlotOfTopByString( Tag::type_name );

        this->NewNullSlotOfTopByString( Tag::bms_data );
        this->NewNullSlotOfTopByString( Tag::search_hit );
      } );
  }
}


std::vector<std::string>
Searcher::SquirrelVM::GetDisplayFilterNames( void )
{
  std::vector<std::string> v;

  this->ArrayEach(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::display_filters} ); },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}

std::vector<std::string>
Searcher::SquirrelVM::GetColumnGroupNames( void )
{
  std::vector<std::string> v;

  this->ArrayEach(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::column_groups} ); },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}

std::vector<std::string>
Searcher::SquirrelVM::GetSearchMethodNames( void )
{
  std::vector<std::string> v;

  this->ArrayEach(
    [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::search_methods} ); },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}


void
Searcher::SquirrelVM::CallEntryConstructor( Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTable( Tag::Entry ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( entry );
      return 2;
    } );
}

bool
Searcher::SquirrelVM::CallDisplayFilter( unsigned int index, Searcher::Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  auto get_instance_func = [&] () {
    this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::display_filters} ); }, index );
  };

  this->CallAndPushReturnValue(
    [&] () {
      this->GetByString( get_instance_func, Tag::filtering );
    },
    [&] () {
      get_instance_func();
      this->PushObject( *entry->squirrel_object_ );
      return 2;
    } );

  if ( this->GetTopType() == TtSquirrel::ObjectType::Boolean ) {
    return this->GetAsFromTop<bool>();
  }
  return false;
}

bool
Searcher::SquirrelVM::CallSearchMethodSearch( unsigned int index, Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  auto get_instance_func = [&] () {
    this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::search_methods} ); }, index );
  };

  this->CallAndPushReturnValue(
    [&] () {
      this->GetByString( get_instance_func, Tag::search );
    },
    [&] () {
      get_instance_func();
      this->PushObject( *entry->squirrel_object_ );
      return 2;
    } );

  if ( this->GetTopType() == TtSquirrel::ObjectType::Boolean ) {
    return this->GetAsFromTop<bool>();
  }
  return false;
}

void
Searcher::SquirrelVM::CallSearchMethodByEachDirectory( unsigned int index, std::shared_ptr<DirectoryEntry> directory_entry, std::vector<Entry*>& entries )
{
  TtSquirrel::StackRecoverer recoverer( this );

  auto get_instance_func = [&] () {
    this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::search_methods} ); }, index );
  };

  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( get_instance_func, Tag::by_each_directory );
    },
    [&] () {
      get_instance_func();
      this->PushObject( directory_entry->squirrel_object_.value() );
      this->Native().NewArray( 0 );
      for ( Entry* entry : entries ) {
        this->AppendToArrayOfStackTop( [&] () {
          this->PushObject( *entry->squirrel_object_ );
          this->SetBooleanToTopByString( Tag::search_hit, true );
        } );
      }
      return 3;
    } );

  for ( Entry* entry : entries ) {
    TtSquirrel::StackRecoverer recoverer_nest( this );

    this->GetByString( [&] () { this->PushObject( *entry->squirrel_object_ ); }, Tag::search_hit );

    if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
      entry->search_hit_ = false;
    }
    else if ( this->GetTopType() == TtSquirrel::ObjectType::Boolean ) {
      entry->search_hit_ = this->GetAsFromTop<bool>();
    }
    else {
      entry->search_hit_ = true;
    }
  }
}


void
Searcher::SquirrelVM::CallParseAsBmsDataOnce( Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( [&] () { this->PushObject( *entry->squirrel_object_ ); }, Tag::parse_as_bms_data_once );
    },
    [&] () {
      this->PushObject( *entry->squirrel_object_ );
      return 1;
    } );
}


std::vector<std::string>
Searcher::SquirrelVM::GetColumnNamesFromColumnGroupIndex( unsigned int index )
{
  std::vector<std::string> v;
  this->ArrayEach(
    [&] () {
      this->GetByString(
        [&] () { this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::column_groups} ); }, index ); },
        Tag::columns );
    },
    [&] ( unsigned int ) {
      v.push_back( this->GetByStringFromTopAndGetAs<std::string>( Tag::name ) );
    } );
  return v;
}

std::string
Searcher::SquirrelVM::CallColumnDisplayCell( unsigned int group_index, unsigned int column_index, Entry* entry )
{
  TtSquirrel::StackRecoverer recoverer( this );

  auto get_instance_func = [&] () {
    this->GetByInteger(
      [&] () { this->GetByString(
        [&] () {
          this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::column_groups} ); }, group_index );
        },
        Tag::columns );
      },
      column_index - 1 );
  };

  this->CallAndPushReturnValue(
    [&] () {
      this->GetByString( get_instance_func, Tag::display_cell );
    },
    [&] () {
      get_instance_func();
      this->PushObject( *entry->squirrel_object_ );
      return 2;
    } );

  this->Native().ToString( TtSquirrel::Const::StackTop );
  return this->GetAsFromTop<std::string>();
}

int
Searcher::SquirrelVM::CallColumnCompare( unsigned int group_index, unsigned int column_index, Entry* x, Entry* y )
{
  TtSquirrel::StackRecoverer recoverer( this );

  auto get_instance_func = [&] () {
    this->GetByInteger(
      [&] () { this->GetByString(
        [&] () {
          this->GetByInteger( [&] () { this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::column_groups} ); }, group_index );
        },
        Tag::columns );
      },
      column_index - 1 );
  };

  this->CallAndPushReturnValue(
    [&] () {
      this->GetByString( get_instance_func, Tag::compare );
    },
    [&] () {
      get_instance_func();
      this->PushObject( *x->squirrel_object_ );
      this->PushObject( *y->squirrel_object_ );
      return 3;
    } );

  return this->GetAsFromTop<int>();
}


TtSubMenuCommandMaker
Searcher::SquirrelVM::GetSubMenuCommandMakerOfEntryProcessors( void )
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
  this->GetByStringFromRootTableJoined( {Tag::Searcher, Tag::entry_processors, Tag::items} );
  func( maker.GetRoot() );
  return maker;
}

void
Searcher::SquirrelVM::CallExecuteOf( TtSquirrel::Object object, Entry* target_entry, std::vector<Entry*> entries )
{
  this->CallAndNoReturnValue(
    [&] () {
      this->GetByString( [&] () { this->PushObject( object ); }, Tag::execute );
    },
    [&] () {
      this->PushObject( object );
      this->PushObject( *target_entry->squirrel_object_ );
      this->Native().NewArray( 0 );
      for ( Entry* entry : entries ) {
        this->AppendToArrayOfStackTop( [&] () {
          this->PushObject( *entry->squirrel_object_ );
        } );
      }
      return 3;
    } );
}
