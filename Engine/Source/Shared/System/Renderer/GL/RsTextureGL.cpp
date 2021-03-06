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

#include "System/Renderer/GL/RsTextureGL.h"

#include "System/Renderer/RsCore.h"
#include "System/Renderer/GL/RsCoreImplGL.h"

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
	wait();
	return pData_;
}

//////////////////////////////////////////////////////////////////////////
// unlockTexture
//virtual
void RsTextureGL::unlockTexture()
{
	UpdateSyncFence_.increment();
	RsCore::pImpl()->updateResource( this );
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

		UpdateSyncFence_.increment();

		// Update.
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
	RsGLCatchError;

	// Call the appropriate method to load the texture.
	switch( Format_ )
	{
	case rsTF_RGB8:
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, Width_, Height_, 0, GL_RGB, GL_UNSIGNED_BYTE, pData_ );	
		break;

	case rsTF_RGBA8:
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width_, Height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData_ );	
		break;

	case rsTF_I8:
		glTexImage2D( GL_TEXTURE_2D, 0, GL_INTENSITY8, Width_, Height_, 0, GL_INTENSITY8, GL_UNSIGNED_BYTE, pData_ );
		break;

	case rsTF_DXT1:
		glCompressedTexImage2D( GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, Width_, Height_, 0, DataSize_, pData_ );
		break;

	case rsTF_DXT3:
		glCompressedTexImage2D( GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, Width_, Height_, 0, DataSize_, pData_ );
		break;

	case rsTF_DXT5:
		glCompressedTexImage2D( GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, Width_, Height_, 0, DataSize_, pData_ );
		break;

	default:
		BcBreakpoint; // Unsupported format for platform.
	}

	GLenum Error = glGetError();
	BcAssertMsg( Error == 0, "RsTextureGL: Error (0x%x) creating texture (%ux%u, format %u, %u bytes).\n", Error, Width_, Height_, Format_, DataSize_ );

	UpdateSyncFence_.decrement();
	
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
