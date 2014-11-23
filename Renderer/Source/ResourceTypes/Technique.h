#ifndef __TECHNIQUE_H_
#define __TECHNIQUE_H_

#include "VertexShader.h"
#include "PixelShader.h"

class Technique
{
public:
	~Technique()
	{
		/*
		m_vertexShader->rm_Reference.m_vertexShader->Release();
		m_vertexShader->rm_Reference.m_vertexShaderConstantTable->Release();
		m_pixelShader->rm_Reference.m_pixelShader->Release();
		m_pixelShader->rm_Reference.m_pixelShaderConstantTable->Release();
		*/
	}
	ResourcePtr<VertexShaderReference>			m_vertexShader;
	ResourcePtr<PixelShaderReference>			m_pixelShader;
	unsigned __int8								m_pass; // 0 = depth, 1 = opaque, 2 = post opaque, 3 = alpha //todo: turn back to u8
	unsigned __int8								m_type; //0 = none, 1 = additive, 2 = partial blend, 3 = test
};

#endif