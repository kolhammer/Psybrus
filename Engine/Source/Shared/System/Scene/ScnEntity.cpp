/**************************************************************************
*
* File:		ScnEntity.cpp
* Author:	Neil Richardson 
* Ver/Date:	26/11/11
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/ScnEntity.h"
#include "System/Scene/ScnCore.h"

//#include "System/Renderer/RsCore.h"

#ifdef PSY_SERVER
#include "Base/BcStream.h"

//////////////////////////////////////////////////////////////////////////
// import
//virtual
BcBool ScnEntity::import( const Json::Value& Object, CsDependancyList& DependancyList )
{
	/*
	const std::string& FileName = Object[ "source" ].asString();

	// Add root dependancy.
	DependancyList.push_back( FileName );

	// Load texture from file and create the data for export.
	ImgImage* pImage = Img::load( FileName.c_str() );
	
	if( pImage != NULL )
	{
		BcStream HeaderStream;
		BcStream BodyStream( BcFalse, 1024, ( pImage->width() * pImage->height() * 4 ) );
		
		// TODO: Use parameters to pick a format.
		THeader Header = { pImage->width(), pImage->height(), 1, rsTF_RGBA8 };
		HeaderStream << Header;
		
		// Write body.				
		for( BcU32 Y = 0; Y < pImage->height(); ++Y )
		{
			for( BcU32 X = 0; X < pImage->width(); ++X )
			{
				ImgColour Colour = pImage->getPixel( X, Y );
				
				BodyStream << Colour.R_;
				BodyStream << Colour.G_;
				BodyStream << Colour.B_;
				BodyStream << Colour.A_;
			}
		}
		
		// Delete image.
		delete pImage;
		
		// Add chunks and finish up.
		pFile_->addChunk( BcHash( "header" ), HeaderStream.pData(), HeaderStream.dataSize() );
		pFile_->addChunk( BcHash( "body" ), BodyStream.pData(), BodyStream.dataSize() );
				
		//
		return BcTrue;
	}
	*/
	return BcFalse;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
DEFINE_RESOURCE( ScnEntity );

//////////////////////////////////////////////////////////////////////////
// StaticPropertyTable
void ScnEntity::StaticPropertyTable( CsPropertyTable& PropertyTable )
{
	Super::StaticPropertyTable( PropertyTable );

	PropertyTable.beginCatagory( "ScnEntity" )
	.endCatagory();
}

//////////////////////////////////////////////////////////////////////////
// initialise
void ScnEntity::initialise()
{
	// NULL internals.
	pHeader_ = NULL;
	IsAttached_ = BcFalse;
}

//////////////////////////////////////////////////////////////////////////
// create
void ScnEntity::create()
{

}

//////////////////////////////////////////////////////////////////////////
// destroy
void ScnEntity::destroy()
{

}

//////////////////////////////////////////////////////////////////////////
// isReady
BcBool ScnEntity::isReady()
{
	// TODO: Set a flag internally once stuff has loaded. Will I ever fucking get round to this!?
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnComponentRef& Component( *It );

		if( Component.isReady() == BcFalse )
		{
			return BcFalse;
		}
	}

	return BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// update
void ScnEntity::update( BcReal Tick )
{
	// Process attach/detach.
	processAttachDetach();

	// Update components.
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnComponentRef& Component( *It );

		Component->update( Tick );
	}
}

//////////////////////////////////////////////////////////////////////////
// render
void ScnEntity::render( RsFrame* pFrame, RsRenderSort Sort )
{
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnRenderableComponentWeakRef RenderableComponent( *It );
		if( RenderableComponent.isValid() )
		{
			RenderableComponent->render( pFrame, Sort );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// attach
void ScnEntity::attach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to attach a null component!" );

	AttachComponents_.push_back( Component );
}

//////////////////////////////////////////////////////////////////////////
// detach
void ScnEntity::detach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to detach a null component!" );

	DetachComponents_.push_back( Component );
}

//////////////////////////////////////////////////////////////////////////
// reattach
void ScnEntity::reattach( ScnComponent* Component )
{
	detach( Component );
	attach( Component );
}

//////////////////////////////////////////////////////////////////////////
// onAttachScene
void ScnEntity::onAttachScene()
{
	// Process attach/detach.
	processAttachDetach();

	BcAssert( IsAttached_ == BcFalse );
	IsAttached_ = BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// onDetachScene
void ScnEntity::onDetachScene()
{
	BcAssert( IsAttached_ == BcTrue );
	IsAttached_ = BcFalse;
}

//////////////////////////////////////////////////////////////////////////
// isAttached
BcBool ScnEntity::isAttached() const
{
	return IsAttached_;
}

//////////////////////////////////////////////////////////////////////////
// getNoofComponents
BcU32 ScnEntity::getNoofComponents() const
{
	return Components_.size();
}
	
//////////////////////////////////////////////////////////////////////////
// getComponent
ScnComponentRef ScnEntity::getComponent( BcU32 Idx )
{
	BcAssert( Idx < Components_.size() );
	return Components_[ Idx ];
}

//////////////////////////////////////////////////////////////////////////
// setPosition
void ScnEntity::setPosition( const BcVec3d& Position )
{
	Transform_.setTranslation( Position );
}

//////////////////////////////////////////////////////////////////////////
// setRotation
void ScnEntity::setRotation( const BcQuat& Rotation )
{
	Transform_.setRotation( Rotation );
}

//////////////////////////////////////////////////////////////////////////
// setMatrix
void ScnEntity::setMatrix( const BcMat4d& Matrix )
{
	Transform_.setMatrix( Matrix );
}

//////////////////////////////////////////////////////////////////////////
// getTransform
const ScnTransform& ScnEntity::getTransform() const
{
	return Transform_;
}

//////////////////////////////////////////////////////////////////////////
// getAABB
const BcAABB& ScnEntity::getAABB() const
{
	// NEILO TODO!
	static BcAABB AABB;
	return AABB;
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnEntity::fileReady()
{
	// File is ready, get the header chunk.
	getChunk( 0 );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnEntity::fileChunkReady( BcU32 ChunkIdx, const CsFileChunk* pChunk, void* pData )
{
	if( pChunk->ID_ == BcHash( "header" ) )
	{
		// Grab pointer to header.
		pHeader_ = reinterpret_cast< THeader* >( pData );
	}
}

//////////////////////////////////////////////////////////////////////////
// processAttachDetach
void ScnEntity::processAttachDetach()
{
	// Detach first.
	while( DetachComponents_.size() > 0 )
	{
		ScnComponentList DetachComponents( DetachComponents_ );
		DetachComponents_.clear();
		for( ScnComponentListIterator It( DetachComponents.begin() ); It != DetachComponents.end(); ++It )
		{
			internalDetach( (*It) );
		}
	}

	// Attach second.
	while( AttachComponents_.size() > 0 )
	{
		ScnComponentList AttachComponents( AttachComponents_ );
		AttachComponents_.clear();
		for( ScnComponentListIterator It( AttachComponents.begin() ); It != AttachComponents.end(); ++It )
		{
			internalAttach( (*It) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// internalAttach
void ScnEntity::internalAttach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to attach a null component!" );

	// If we're not attached to ourself, bail.
	if( Component->isAttached( this ) == BcFalse )
	{
		BcAssertMsg( Component->isAttached() == BcFalse, "Component is attached to another entity!" );

		// Call the on detach.
		Component->onAttach( ScnEntityWeakRef( this ) );

		// Put into component list.
		Components_.push_back( Component );

		// Tell the scene about it.
		if( IsAttached_ == BcTrue )
		{
			ScnCore::pImpl()->onAttachComponent( ScnEntityWeakRef( this ), Component );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// internalDetach
void ScnEntity::internalDetach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to detach a null component!" );

	// If component isn't attached, don't worry. Only a warning?
	if( Component->isAttached() == BcTrue )
	{
		BcAssertMsg( Component->isAttached( this ) == BcTrue, "Component isn't attached to this entity!" );
		// Call the on detach.
		Component->onDetach( ScnEntityWeakRef( this ) );

		// Remove from the list.
		for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
		{
			ScnComponentRef& ListComponent( *It );

			if( ListComponent == Component )
			{
				// Remove from component list.
				Components_.erase( It );
				break;
			}
		}

		// Tell the scene about it.
		if( IsAttached_ )
		{
			ScnCore::pImpl()->onDetachComponent( ScnEntityWeakRef( this ), Component );
		}
	}
}