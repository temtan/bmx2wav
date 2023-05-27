// searcher/searcher_dragdrop.cpp

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

#include "tt_string.h"

#include "searcher/searcher_dragdrop.h"

#pragma warning(disable : 4239)

using namespace BMX2WAV;


namespace {
  const char* const ITEM_TYPE_NAME = "TTVC_LIST_ITEM";
}

// -- ListDropSource -----------------------------------------------------
Searcher::ListDropSource::ListDropSource( void ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,    static_cast<IDropSource*>( this ) },
  { IID_IDropSource, static_cast<IDropSource*>( this ) }
} )
{
}

Searcher::ListDropSource::~ListDropSource()
{
}


STDMETHODIMP
Searcher::ListDropSource::QueryInterface( REFIID riid, void **ppvObject )
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
Searcher::ListDropSource::AddRef( void )
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_( ULONG )
Searcher::ListDropSource::Release( void )
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
Searcher::ListDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
  if ( fEscapePressed ) {
    return DRAGDROP_S_CANCEL;
  }
  if ( (grfKeyState & MK_LBUTTON) == 0 ) {
    return DRAGDROP_S_DROP;
  }
  return S_OK;
}

STDMETHODIMP
Searcher::ListDropSource::GiveFeedback( DWORD dwEffect )
{
  NOT_USE( dwEffect );
  return DRAGDROP_S_USEDEFAULTCURSORS;
}


// -- Searcher::ListDataObject -----------------------------------------------------
Searcher::ListDataObject::ListDataObject( void ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,       static_cast<IDataObject*>( this ) },
  { IID_IDataObject,    static_cast<IDataObject*>( this ) },
  { IID_IEnumFORMATETC, static_cast<IEnumFORMATETC*>( this ) }
} ),
enumerate_counter_( 0 ),
global_memory_handle_( NULL ),
cf_list_item_type_( ::RegisterClipboardFormat( ITEM_TYPE_NAME ) ),
selected_indices_()
{
}

Searcher::ListDataObject::~ListDataObject()
{
  if ( global_memory_handle_ ) {
    ::GlobalFree( global_memory_handle_ );
  }
}


std::vector<unsigned int>&
Searcher::ListDataObject::GetSelectedIndices( void )
{
  return selected_indices_;
}

void
Searcher::ListDataObject::SetDataAsPathVector( const std::vector<std::string>& paths )
{
  std::string data = TtString::ToRangedStringFromVector( paths );

  FORMATETC formatetc = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

  STGMEDIUM medium;
  medium.tymed          = TYMED_HGLOBAL;
  medium.hGlobal        = ::GlobalAlloc( GHND, sizeof( DROPFILES ) + data.size() + 2 );
  medium.pUnkForRelease = NULL;

  DROPFILES* p = static_cast<DROPFILES*>( ::GlobalLock( medium.hGlobal ) );
  p->pFiles = sizeof( DROPFILES );
  p->pt.x = 0;
  p->pt.y = 0;
  p->fNC = FALSE;
  p->fWide  = FALSE;
  ::CopyMemory( p + 1, data.c_str(), data.size() + 1 );
  ::GlobalUnlock( medium.hGlobal );

  this->SetData( &formatetc, &medium, TRUE );
}


void
Searcher::ListDataObject::ClearData( void )
{
  if ( global_memory_handle_ ) {
    ::GlobalFree( global_memory_handle_ );
    global_memory_handle_ = NULL;
  }
}


STDMETHODIMP
Searcher::ListDataObject::QueryInterface( REFIID riid, void **ppvObject )
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
Searcher::ListDataObject::AddRef( void )
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_( ULONG )
Searcher::ListDataObject::Release( void )
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
Searcher::ListDataObject::GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
  if ( global_memory_handle_ != NULL && pformatetcIn->cfFormat == CF_HDROP ) {
    pmedium->tymed = TYMED_HGLOBAL;
    pmedium->hGlobal = (HGLOBAL)::OleDuplicateData( global_memory_handle_, CF_HDROP, (UINT)NULL );
    pmedium->pUnkForRelease = NULL;
  }
  else if ( pformatetcIn->cfFormat == cf_list_item_type_ ) {
    pmedium->tymed = TYMED_NULL;
    pmedium->pUnkForRelease = NULL;
  }
  else {
    return E_FAIL;
  }
  return S_OK;
}

STDMETHODIMP
Searcher::ListDataObject::GetDataHere( FORMATETC *pformatetc, STGMEDIUM *pmedium )
{
  NOT_USE( pformatetc );
  NOT_USE( pmedium );
  return E_NOTIMPL;
}

STDMETHODIMP
Searcher::ListDataObject::QueryGetData( FORMATETC *pformatetc )
{
  if ( global_memory_handle_ != NULL && pformatetc->cfFormat == CF_HDROP ) {
    return S_OK;
  }
  if ( pformatetc->cfFormat == cf_list_item_type_ ) {
    return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP
Searcher::ListDataObject::GetCanonicalFormatEtc( FORMATETC *pformatectIn, FORMATETC *pformatetcOut )
{
  NOT_USE( pformatectIn );
  NOT_USE( pformatetcOut );
  return E_NOTIMPL;
}

STDMETHODIMP
Searcher::ListDataObject::SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease )
{
  if ( pformatetc->cfFormat == CF_HDROP ) {
    if ( global_memory_handle_ != NULL ) {
      ::GlobalFree( global_memory_handle_ );
      global_memory_handle_ = NULL;
    }

    global_memory_handle_ = (HGLOBAL)::OleDuplicateData( pmedium->hGlobal, CF_HDROP, (UINT)NULL );

    if ( fRelease ) {
      ::GlobalFree( pmedium->hGlobal );
    }
  }
  else if ( pformatetc->cfFormat == cf_list_item_type_ ) {
    // “Á‚É‚â‚é‚±‚Æ–³‚µ
  }
  else {
    return E_FAIL;
  }

  return S_OK;
}

STDMETHODIMP
Searcher::ListDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc )
{
  if ( dwDirection == DATADIR_GET ) {
    return this->QueryInterface( IID_PPV_ARGS( ppenumFormatEtc ) );
  }
  else {
    return E_NOTIMPL;
  }
}

STDMETHODIMP
Searcher::ListDataObject::DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
  NOT_USE( pformatetc );
  NOT_USE( advf );
  NOT_USE( pAdvSink );
  NOT_USE( pdwConnection );
  return E_NOTIMPL;
}

STDMETHODIMP
Searcher::ListDataObject::DUnadvise( DWORD dwConnection )
{
  NOT_USE( dwConnection );
  return E_NOTIMPL;
}

STDMETHODIMP
Searcher::ListDataObject::EnumDAdvise( IEnumSTATDATA **ppenumAdvise )
{
  NOT_USE( ppenumAdvise );
  return E_NOTIMPL;
}


STDMETHODIMP
Searcher::ListDataObject::Next( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched )
{
  NOT_USE( celt );

  std::vector<FORMATETC> v = {
    {static_cast<CLIPFORMAT>( cf_list_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL}
  };
  if ( global_memory_handle_ != NULL ) {
    v.push_back( {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL} );
  }

  if ( enumerate_counter_ >= v.size() ) {
    return S_FALSE;
  }
  *rgelt = v[enumerate_counter_];
  if ( pceltFetched != NULL ) {
    *pceltFetched = 1;
  }

  enumerate_counter_++;
  return S_OK;
}

STDMETHODIMP
Searcher::ListDataObject::Skip( ULONG celt )
{
  NOT_USE( celt );
  return E_NOTIMPL;
}

STDMETHODIMP
Searcher::ListDataObject::Reset( void )
{
  enumerate_counter_ = 0;
  return S_OK;
}

STDMETHODIMP
Searcher::ListDataObject::Clone( IEnumFORMATETC **ppenum )
{
  NOT_USE( ppenum );
  return E_NOTIMPL;
}


// -- ListDropTarget -----------------------------------------------------
Searcher::ListDropTarget::ListDropTarget( MainList& list, MainList::Item::DragHandler& drag_handler ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,    static_cast<IDropTarget*>( this ) },
  { IID_IDropTarget, static_cast<IDropTarget*>( this ) },
} ),
list_( list ),
drag_handler_( drag_handler ),
cf_list_item_type_( ::RegisterClipboardFormat( ITEM_TYPE_NAME ) ),
drop_handler_( nullptr )
{
}

Searcher::ListDropTarget::~ListDropTarget()
{
}

void
Searcher::ListDropTarget::SetDropHandler( std::function<void ( void )> handler )
{
  drop_handler_ = handler;
}


STDMETHODIMP
Searcher::ListDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
Searcher::ListDropTarget::AddRef()
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_(ULONG)
Searcher::ListDropTarget::Release()
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
Searcher::ListDropTarget::DragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( grfKeyState );

  FORMATETC formatetc = {static_cast<CLIPFORMAT>( cf_list_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL};
  if ( pDataObj->QueryGetData( &formatetc ) == S_OK && drag_handler_.IsBegun() ) {
    *pdwEffect = DROPEFFECT_MOVE;
    drag_handler_.Enter( list_, pt.x, pt.y );
  }
  else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP
Searcher::ListDropTarget::DragOver( DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( grfKeyState );

  POINT current = {pt.x - list_.GetPoint().x, pt.y - list_.GetPoint().y};
  if ( drag_handler_.IsEntered() ) {
    *pdwEffect = DROPEFFECT_MOVE;
    MainList::Item hit_item = list_.HitTest( current.x, current.y );
    if ( hit_item.IsValid() && NOT( hit_item.IsDropHilighted() ) ) {
      drag_handler_.ReEnter( current.x, current.y, [&] ( void ) {
        drag_handler_.CancelDropHilighted();
        hit_item.SetDropHilight( true );
        drag_handler_.SetDropHilightedItem( hit_item );
        list_.Update();
      } );
    }
    else if ( hit_item.IsInvalid() ) {
      drag_handler_.ReEnter( current.x, current.y, [&] ( void ) {
        drag_handler_.CancelDropHilighted();
        drag_handler_.SetDropHilightedItem( MainList::Item( nullptr, MainList::Item::INVALID_INDEX ) );
        list_.Update();
      } );
    }
    else {
      drag_handler_.Move( current.x, current.y );
    }
  }
  else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP
Searcher::ListDropTarget::DragLeave( void )
{
  if ( drag_handler_.IsEntered() ) {
    drag_handler_.Leave();
  }
  return S_OK;
}

STDMETHODIMP
Searcher::ListDropTarget::Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( pt );
  NOT_USE( grfKeyState );

  FORMATETC formatetc = {static_cast<CLIPFORMAT>( cf_list_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL};
  STGMEDIUM medium;
  if ( FAILED( pDataObj->GetData( &formatetc, &medium ) ) ) {
    *pdwEffect = DROPEFFECT_NONE;
    return E_FAIL;
  }
  *pdwEffect = DROPEFFECT_MOVE;

  if ( drop_handler_ ) {
    drop_handler_();
  }
  return S_OK;
}
