/**************************************************************************
*
* File:		ScnRenderableComponent.cpp
* Author:	Neil Richardson 
* Ver/Date:	28/12/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/ScnRenderableComponent.h"
#include "System/Scene/ScnEntity.h"
#include "System/Renderer/RsCore.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
DEFINE_RESOURCE( ScnRenderableComponent );

//////////////////////////////////////////////////////////////////////////
// StaticPropertyTable
void ScnRenderableComponent::StaticPropertyTable( CsPropertyTable& PropertyTable )
{
	Super::StaticPropertyTable( PropertyTable );

	PropertyTable.beginCatagory( "ScnRenderableComponent" )
	.endCatagory();
}

//////////////////////////////////////////////////////////////////////////
// initialise
void ScnRenderableComponent::initialise()
{
	Super::initialise();
}

//////////////////////////////////////////////////////////////////////////
// update
//virtual
void ScnRenderableComponent::update( BcReal Tick )
{
	Super::update( Tick );

}

//////////////////////////////////////////////////////////////////////////
// render
//virtual
void ScnRenderableComponent::render( RsFrame* pFrame, RsRenderSort Sort )
{
	// Do nothing.
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void ScnRenderableComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void ScnRenderableComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}
