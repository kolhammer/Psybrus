/**************************************************************************
*
* File:		RsTextureGL.cpp
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "RsTextureGL.h"

#include "RsCore.h"
#include "RsCoreImplGL.h"

//////////////////////////////////////////////////////////////////////////
// RsTextureGL
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Ctor
RsTextureGL::RsTextureGL( BcU32 Width, BcU32 Height, BcU32 Levels, eRsTextureFormat Format, void* pTextureData )
{
	// Setup parameters.
	Width_ = Width;
	Height_ = Height;
	Levels_ = Levels;
	Format_ = Format;
	Locked_ = BcFalse;
	pData_ = pTextureData;
	DataSize_ = RsTextureFormatSize( Format, Width, Height, Levels );
	
	// Create data if we need to.
	if( pData_ == NULL )
	{		
		pData_ = new BcU8[ DataSize_ ];
		DeleteData_ = BcTrue;
	}
}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
RsTextureGL::~RsTextureGL()
{

}

//////////////////////////////////////////////////////////////////////////
// width
//virtual
BcU32 RsTextureGL::width() const
{
	return Width_;
}

//////////////////////////////////////////////////////////////////////////
// height
//virtual
BcU32 RsTextureGL::height() const
{
	return Height_;
}

//////////////////////////////////////////////////////////////////////////
// lockTexture
//virtual
void* RsTextureGL::lockTexture()
{
	BcAssert( Locked_ == BcFalse );
	
	if( Locked_ == BcFalse )
	{
		return pData_;
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// unlockTexture
//virtual
void RsTextureGL::unlockTexture()
{
	BcAssert( Locked_ == BcTrue );

	// Upload new texture data.
	if( Locked_ == BcTrue )
	{
		Locked_ = BcFalse;
	}
}

////////////////////////////////////////////////////////////////////////////////
// create
//virtual
void RsTextureGL::create()
{
	// Create GL texture.
	GLuint Handle;
	glGenTextures( 1, &Handle );
	setHandle( Handle );
	
	if( Handle != 0 )
	{
		GLenum Error = glGetError();

		// Update (slow...)
		update();

		// Destroy if there is a failure.
		Error = glGetError();
		if ( Error != GL_NO_ERROR )
		{
			destroy();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// update
void RsTextureGL::update()
{
	// Bind and upload.
	GLuint Handle = getHandle< GLuint >();
	
	glBindTexture( GL_TEXTURE_2D, Handle );
	/*
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	 */
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width_, Height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData_ );	
	RsGLCatchError;
	
	// Invalidate texture state.
	RsStateBlock* pStateBlock = RsCore::pImpl()->getStateBlock();
	pStateBlock->invalidateTextureState();
}

////////////////////////////////////////////////////////////////////////////////
// destroy
void RsTextureGL::destroy()
{
	// Check that we haven't already freed it.
	GLuint Handle = getHandle< GLuint >();
	if( Handle != 0 )
	{
		// Delete it.
		glDeleteTextures( 1, &Handle );
		setHandle< GLuint >( 0 );
	}
}
