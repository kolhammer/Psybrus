/**************************************************************************
*
* File:		ScnRenderTarget.cpp
* Author:	Neil Richardson 
* Ver/Date:	16/08/11
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/ScnRenderTarget.h"

#include "System/Content/CsCore.h"

#ifdef PSY_SERVER
#include "Base/BcFile.h"
#include "Base/BcStream.h"
#endif

#ifdef PSY_SERVER
//////////////////////////////////////////////////////////////////////////
// import
//virtual
BcBool ScnRenderTarget::import( const Json::Value& Object, CsDependancyList& DependancyList )
{
	return BcFalse;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
DEFINE_RESOURCE( ScnRenderTarget );

//////////////////////////////////////////////////////////////////////////
// StaticPropertyTable
void ScnRenderTarget::StaticPropertyTable( CsPropertyTable& PropertyTable )
{
	Super::StaticPropertyTable( PropertyTable );

	PropertyTable.beginCatagory( "ScnRenderTarget" )
		//.field( "source",					csPVT_FILE,			csPCT_LIST )
	.endCatagory();
}

//////////////////////////////////////////////////////////////////////////
// initialise
//virtual
void ScnRenderTarget::initialise( BcU32 Width, BcU32 Height )
{
	ScnTexture::initialise();
	
	TextureHeader_.Width_ = Width;
	TextureHeader_.Height_ = Height;
	TextureHeader_.Levels_ = 1;
	TextureHeader_.Format_ = rsTF_RGBA8;	
	
	// Pass header to parent.
	pHeader_ = &TextureHeader_;
	pRenderTarget_ = NULL;
}

//////////////////////////////////////////////////////////////////////////
// create
//virtual
void ScnRenderTarget::create()
{
	// Create render target.
	pRenderTarget_ = RsCore::pImpl()->createRenderTarget( TextureHeader_.Width_, TextureHeader_.Height_, rsCF_A8R8G8B8, rsDSF_D24S8 );
	
	// Get texture from target.
	pTexture_ = pRenderTarget_->getTexture();
	
	// Render target manages this.
	CreateNewTexture_ = BcFalse;
}

//////////////////////////////////////////////////////////////////////////
// destroy
//virtual
void ScnRenderTarget::destroy()
{
	RsCore::pImpl()->destroyResource( pRenderTarget_ );
}

//////////////////////////////////////////////////////////////////////////
// isReady
//virtual
BcBool ScnRenderTarget::isReady()
{
	return ScnTexture::isReady() && ( pRenderTarget_ != NULL && pRenderTarget_->hasHandle() );
}

//////////////////////////////////////////////////////////////////////////
// bind
void ScnRenderTarget::bind( RsFrame* pFrame )
{
	pFrame->setRenderTarget( pRenderTarget_ );	
}

//////////////////////////////////////////////////////////////////////////
// unbind
void ScnRenderTarget::unbind( RsFrame* pFrame )
{
	pFrame->setRenderTarget( NULL );	
}

