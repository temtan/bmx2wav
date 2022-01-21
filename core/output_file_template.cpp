// core/output_file_template.cpp

#include "tt_utility.h"

#include "core/output_file_template.h"

using namespace BMX2WAV;


// -- Element ------------------------------------------------------------
void
Core::OutputFileTemplate::Element::AppendToText( char c )
{
  text_.append( 1, c );
}

// -- Text ---------------------------------------------------------------
std::string
Core::OutputFileTemplate::Text::MakeText( Maker& )
{
  return text_;
}

// -- Function -----------------------------------------------------------
Core::OutputFileTemplate::Function::Function( bool is_root ) :
is_root_( is_root )
{
}


std::string
Core::OutputFileTemplate::Function::MakeText( Maker& maker )
{
  block_string_.clear();
  for ( auto element : block_ ) {
    block_string_.append( element->MakeText( maker ) );
  }

  if ( NOT( is_root_ ) && maker.table_.contains( name_ ) && maker.table_[name_] ) {
    return maker.table_[name_]( *this );
  }

  return text_ + block_string_ + (this->BlockExist() ? "}%" : "");
}

bool
Core::OutputFileTemplate::Function::BlockExist( void )
{
  return block_.size() > 0 && dynamic_cast<FunctionCloser*>( block_.back().get() ) != 0;
}

// -- FunctionCloser -----------------------------------------------------
std::string
Core::OutputFileTemplate::FunctionCloser::MakeText( Maker& )
{
  return "";
}


// -- Parser -------------------------------------------------------------
std::shared_ptr<Core::OutputFileTemplate::Function>
Core::OutputFileTemplate::Parser::Parse( const std::string& str )
{
  current_ = str.c_str();
  return this->ParseAsRoot();
}


void
Core::OutputFileTemplate::Parser::MoveNext( void )
{
  target_->AppendToText( *current_ );
  current_ += 1;
}

void
Core::OutputFileTemplate::Parser::SkipSpace( void )
{
  for ( ; ; this->MoveNext() ) {
    if ( *current_ != ' ' ) {
      break;
    }
  }
}


std::shared_ptr<Core::OutputFileTemplate::Function>
Core::OutputFileTemplate::Parser::ParseAsRoot( void )
{
  std::shared_ptr<Function> root = std::make_shared<Function>( true );
  root->block_.push_back( std::make_shared<Text>() );
  target_ = root->block_.back();
  this->ParseAsBlock( *root );
  return root;
}

void
Core::OutputFileTemplate::Parser::ParseAsBlock( Function& current_function )
{
  for (;;) {
    if ( current_function.is_root_ ) {
      if ( *current_ == '\0' ) {
        return;
      }
    }
    else {
      if ( *current_ == '\0' ) {
        throw IllegalFunction();
      }
      if ( *current_ == '}' && *(current_ + 1) == '%' ) {
        current_function.block_.push_back( std::make_shared<FunctionCloser>() );
        target_ = current_function.block_.back();
        this->MoveNext();
        this->MoveNext();
        return;
      }
    }

    if ( *current_ == '%' ) {
      std::shared_ptr<Function> child = std::make_shared<Function>( false );
      auto current_backup = current_;
      try {
        target_ = child;
        this->ParseAsFunction( *child );
        current_function.block_.push_back( child );
        current_function.block_.push_back( std::make_shared<Text>() );
        target_ = current_function.block_.back();
        continue;
      }
      catch ( IllegalFunction ) {
        current_ = current_backup;
        target_ = current_function.block_.back();
      }
    }
    current_function.block_.back()->AppendToText( *current_ );
    current_ += 1;
  }
}

void
Core::OutputFileTemplate::Parser::ParseAsFunction( Function& child )
{
  auto is_id_char = [] ( char c ) -> bool { return isalnum( c ) || c == '_'; };

  if ( *current_ != '%' ) {
    throw IllegalFunction();
  }
  this->MoveNext();

  if ( NOT( is_id_char( *current_ ) ) ) {
    throw IllegalFunction();
  }

  for ( ; ; this->MoveNext() ) {
    if ( is_id_char( *current_ ) ) {
      child.name_.append( 1, *current_ );
      continue;
    }
    if ( *current_ == '%' ) {
      this->MoveNext();
      break;
    }
    throw IllegalFunction();
  }

  auto current_backup = current_;
  auto text_backup = target_->text_;
  this->SkipSpace();
  if ( *current_ == '(' ) {
    this->ParseAsArgument( child );
    current_backup = current_;
    text_backup = target_->text_;
  }

  this->SkipSpace();
  if ( *current_ == '{' ) {
    this->MoveNext();
    child.block_.push_back( std::make_shared<Text>() );
    target_ = child.block_.back();
    this->ParseAsBlock( child );
  }
  else {
    current_ = current_backup;
    target_->text_ = text_backup;
  }
}

void
Core::OutputFileTemplate::Parser::ParseAsArgument( Function& child )
{
  this->MoveNext();
  this->SkipSpace();

  if ( isdigit( *current_ ) ) {
    this->ParseAsArgumentInteger( child );
  }
  else if ( *current_ == '\"' ) {
    this->ParseAsArgumentString( child );
  }
  else if ( *current_ != ')' ) {
    throw IllegalFunction();
  }

  this->SkipSpace();
  if ( *current_ != ')' ) {
    return throw IllegalFunction();
  }
  this->MoveNext();
}

void
Core::OutputFileTemplate::Parser::ParseAsArgumentInteger( Function& child )
{
  std::string tmp;
  for ( ; ; this->MoveNext() ) {
    if ( isdigit( *current_ ) ) {
      tmp.append( 1, *current_ );
      continue;
    }
    break;
  }

  if ( int ret; TtUtility::StringToInteger( tmp, &ret ) ) {
    child.argument_integer_ = ret;
  }
  else {
    throw IllegalFunction();
  }
}

void
Core::OutputFileTemplate::Parser::ParseAsArgumentString( Function& child )
{
  this->MoveNext();

  std::string tmp;
  for ( ; ; this->MoveNext() ) {
    if ( *current_ == '\0' ) {
      throw IllegalFunction();
    }

    if ( *current_ == '\"' ) {
      child.argument_string_ = tmp;
      this->MoveNext();
      break;
    }

    if ( *current_ == '\\' ) {
      this->MoveNext();
      if ( *current_ == '\0' ) {
        throw IllegalFunction();
      }
      std::unordered_map<char, char> h = {
        {'n',  '\n'},
        {'t',  '\t'},
        {'r',  '\r'},
      };
      if ( h.contains( *current_ ) ) {
        tmp.append( 1, h[*current_] );
      }
      else {
        tmp.append( 1, *current_ );
      }
    }
    else {
      tmp.append( 1, *current_ );
    }
  }
}

// -- Maker --------------------------------------------------------------
void
Core::OutputFileTemplate::Maker::SetFunction( const std::string& key, FunctionCallback callback )
{
  table_[key] = callback;
}

Core::OutputFileTemplate::Maker::FunctionCallback&
Core::OutputFileTemplate::Maker::operator []( const std::string& key )
{
  return table_[key];
}

std::string
Core::OutputFileTemplate::Maker::MakeText( const std::string& str )
{
  Parser parser;
  auto root = parser.Parse( str );
  return root->MakeText( *this );
}
