// random_statement_dialog.h

#pragma once

#include "tt_dialog.h"
#include "tt_tree_view.h"

#include "base/parser.h"


namespace BMX2WAV {
  // -- RandomStatementDialog --------------------------------------------
  class RandomStatementDialog : public TtDialog {
  public:
    explicit RandomStatementDialog( const std::string& path, BL::Parser::RootBlock& root );

    void CheckToRootItem( void );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

    class Tree : public TtWindowWithStyle<TtTreeView, TtTreeView::Style::HASBUTTONS | TtTreeView::Style::SHOWSELALWAYS | TtTreeView::Style::LINESATROOT | TtTreeView::Style::HASLINES | TtTreeView::Style::DISABLEDRAGDROP | TtTreeView::Style::CHECKBOXES> {
    public:
      using Item = TtTreeItemWith<BL::Parser::Statement*>;
    };

    void SetTreeItems( Tree::Item item, BL::Parser::Block* block );
    void CheckTreeItems( void );
    void CheckToEvaluationType( void );

  private:
    std::string            path_;
    BL::Parser::RootBlock& root_;

    Tree     tree_;
    TtButton check_button_;
    TtButton ok_button_;
  };
}
