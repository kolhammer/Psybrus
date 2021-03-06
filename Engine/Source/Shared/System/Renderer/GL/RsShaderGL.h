/**************************************************************************
 *
 * File:	RsShaderGL.h
 * Author: 	Neil Richardson 
 * Ver/Date:	
 * Description:
 *		GL Shader.
 *		
 *
 *
 * 
 **************************************************************************/

#ifndef __RSSHADERGL_H__
#define __RSSHADERGL_H__

#include "System/Renderer/RsShader.h"
#include "System/Renderer/GL/RsGL.h"

////////////////////////////////////////////////////////////////////////////////
// RsShaderGL
class RsShaderGL:
	public RsShader
{
public:
	/**
	 * Create shader.
	 * @param ShaderType Shader type.
	 * @param ShaderDataType Shader data type.
	 * @param pShaderData Shader data.
	 * @param ShaderDataSize Shader data size.
	 */
	RsShaderGL( eRsShaderType ShaderType, eRsShaderDataType ShaderDataType, void* pShaderData, BcU32 ShaderDataSize );
	virtual ~RsShaderGL();
	
	void								create();
	virtual void						update();
	virtual void						destroy();	
	
private:
	GLenum								Type_;
	eRsShaderDataType					DataType_;
	BcU32								Format_;
};

#endif
