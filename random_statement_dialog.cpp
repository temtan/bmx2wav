// random_statement_dialog.cpp

#include "tt_message_box.h"

#include "string_table.h"

#include "random_statement_dialog.h"

using namespace BMX2WAV;


// -- RandomStatementDialog ----------------------------------------------
RandomStatementDialog::RandomStatementDialog( const std::string& path, BL::Parser::RootBlock& root ) :
path_( path ),
root_( root ),
tree_()
{
}

void
RandomStatementDialog::CheckToRootItem( void )
{
  tree_.GetOrigin().GetFirstChild().SetCheck( true );
}


DWORD
RandomStatementDialog::GetStyle( void )
{
  return WS_SIZEBOX;
}

DWORD
RandomStatementDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
RandomStatementDialog::Created( void )
{
  this->SetText( StrT::Random::Title.Get() );

  struct CommandID {
    enum ID : int {
      CheckButton = 13001,
      OkButton,
    };
  };

  tree_.Create( {this} );
  check_button_.Create( {this, CommandID::CheckButton} );
  ok_button_.Create( {this, CommandID::OkButton} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    tree_.SetPositionSize(           2,     28, w - 4, h - 64 );
    check_button_.SetPositionSize(   2,      2,   220,     24 );
    ok_button_.SetPositionSize( w - 68, h - 32,    48,     24 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 350, 500, false );
  this->SetCenterRelativeToParent();

  check_button_.SetText( StrT::Random::CheckButton.Get() );
  ok_button_.SetText(    StrT::Random::OKButton.Get() );

  this->AddCommandHandler( CommandID::CheckButton, [&] ( int, HWND ) -> WMResult {
    this->CheckTreeItems();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OkButton, [&] ( int, HWND ) -> WMResult {
    this->CheckToEvaluationType();
    this->Close();
    return {WMResult::Done};
  } );

  {
    Tree::Item item = tree_.GetOrigin().AddChildLast( path_ );
    this->SetTreeItems( item, &root_ );
    item.Expand();
  }

  tree_.Show();
  check_button_.Show();
  ok_button_.Show();

  return true;
}


namespace {
  template <class TYPE>
  void if_type_match_do( BL::Parser::Statement* statement, std::function<void ( TYPE* )> func ) {
    if ( TYPE* tmp = dynamic_cast<TYPE*>( statement ); tmp ) {
      func( tmp );
    }
  }
}

void
RandomStatementDialog::SetTreeItems( Tree::Item item, BL::Parser::Block* block )
{
  for ( auto statement : block->statements_ ) {
    auto register_child = [&] ( const std::string& title ) -> Tree::Item {
      Tree::Item child = item.AddChildLast( title );
      child.SetParameter( statement.get() );
      return child;
    };

    std::string title = TtUtility::ToStringFrom( statement->raw_line_->number_ );
    if_type_match_do<BL::Parser::IfBlock>( statement.get(), [&] ( BL::Parser::IfBlock* tmp ) {
      title.append( " : #IF " );
      title.append( TtUtility::ToStringFrom( tmp->prerequisite_ ) );
      title.append( "  `  " );
      title.append( TtUtility::ToStringFrom( tmp->end_line_->number_ ) );
      title.append( " : #ENDIF" );
      Tree::Item child = register_child( title );
      this->SetTreeItems( child, tmp );
    } );
    if_type_match_do<BL::Parser::GenerateRandom>( statement.get(), [&] ( BL::Parser::GenerateRandom* tmp ) {
      title.append( " : #RANDOM " );
      title.append( TtUtility::ToStringFrom( tmp->max_ ) );
      register_child( title );
    } );
    if_type_match_do<BL::Parser::SetRandom>( statement.get(), [&] ( BL::Parser::SetRandom* tmp ) {
      title.append( " : #SETRANDOM " );
      title.append( TtUtility::ToStringFrom( tmp->value_ ) );
      register_child( title );
    } );
    if_type_match_do<BL::Parser::EndRandom>( statement.get(), [&] ( BL::Parser::EndRandom* ) {
      title.append( " : #ENDRANDOM" );
      register_child( title );
    } );
  }
}

void
RandomStatementDialog::CheckTreeItems( void )
{
  std::vector<unsigned int> random_value_stack = {0};
  Tree::Item root = tree_.GetOrigin().GetFirstChild();
  root.SetCheck( true );

  std::function<void ( Tree::Item, bool )> check_function = [&] ( Tree::Item item, bool parent_check ) {
    for ( Tree::Item child : item.GetChildEnumerable() ) {
      if_type_match_do<BL::Parser::IfBlock>( child.GetParameter(), [&] ( BL::Parser::IfBlock* tmp ) {
        if ( parent_check && random_value_stack.back() == tmp->prerequisite_ ) {
          child.SetCheck( true );
          check_function( child, true );
        }
        else {
          child.SetCheck( false );
          check_function( child, false );
        }
      } );
      if_type_match_do<BL::Parser::RandomStatement>( child.GetParameter(), [&] ( BL::Parser::RandomStatement* tmp ) {
        random_value_stack.push_back( tmp->Generate() );
        child.SetCheck( parent_check );
      } );
      if_type_match_do<BL::Parser::EndRandom>( child.GetParameter(), [&] ( BL::Parser::EndRandom* ) {
        if ( random_value_stack.size() > 1 ) {
          random_value_stack.pop_back();
        }
        child.SetCheck( parent_check );
      } );
    }
  };
  check_function( root, true );
}

void
RandomStatementDialog::CheckToEvaluationType( void )
{
  Tree::Item root = tree_.GetOrigin().GetFirstChild();

  std::function<void ( Tree::Item )> check_function = [&] ( Tree::Item item ) {
    for ( Tree::Item child : item.GetChildEnumerable() ) {
      if_type_match_do<BL::Parser::IfBlock>( child.GetParameter(), [&] ( BL::Parser::IfBlock* tmp ) {
        tmp->evaluation_type_ = child.GetCheck() ? BL::Parser::IfBlock::EvaluationType::MustApply : BL::Parser::IfBlock::EvaluationType::MustIgnore;
        check_function( child );
      } );
    }
  };
  check_function( root );
}
