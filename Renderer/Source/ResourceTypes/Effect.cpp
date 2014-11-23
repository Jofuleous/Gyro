#include "Effect.h"
#include "../ResourceManager.h"

#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <vector>
#include <malloc.h>
#include <assert.h>
#include <string>

EffectReference* EffectLoader::Load( const char* i_filename )
{
	char filepath[128];
	sprintf( filepath, "data//Effects//%s", i_filename );

	std::ifstream file( filepath );

	if( !file )
	{
		std::cerr << "Could not find file: \"" << filepath << "\"";
		return false;
	}

	EffectReference* reference = new EffectReference();
	reference->rm_Reference.m_effectId = ResourceManager<EffectReference>::m_nextId;

	std::string data;

	file >> data;

	reference->rm_Reference.m_effectId = ResourceManager<EffectReference>::m_resourceMap.size();

	while( data.compare( "<Technique" ) == 0 )
	{
		//get the type...
		file >> data;
		Technique** t;
		Technique*  tech = new Technique();
		if( data.compare( "Depth>" ) == 0 )
		{
			t = &reference->rm_Reference.m_depth;
			tech->m_pass = 1;
		}
		else if( data.compare( "Opaque>" ) == 0 )
		{
			t = &reference->rm_Reference.m_opaque;
			tech->m_pass = 2;
		}
		else if( data.compare( "PostOpaque>" ) == 0 )
		{
			t = &reference->rm_Reference.m_postOpaque;
			tech->m_pass = 3;
		}
		else if( data.compare( "Alpha>" ) == 0 )
		{
			t = &reference->rm_Reference.m_alpha;
			tech->m_pass = 4;
		}
		else if( data.compare( "PostProcess>" ) == 0 )
		{
			t = &reference->rm_Reference.m_postProcess;
			tech->m_pass = 5;
		}
		else if( data.compare( "UI>" ) == 0 )
		{
			t = &reference->rm_Reference.m_UI;
			tech->m_pass = 6;
		}

		file >> data;

		
		if( data.compare( "VERTEXSHADER" ) == 0 )
		{
			file >> data;
			VertexShaderLoader loader;

			ResourcePtr<VertexShaderReference> vReference = ResourceManager<VertexShaderReference>::Load( data.c_str(), &loader, false );
			tech->m_vertexShader = vReference;
			file >> data;
		}
		else
			assert( 0 );

		if( data.compare( "PIXELSHADER" ) == 0 )
		{
			file >> data;
			PixelShaderLoader loader;
			ResourcePtr<PixelShaderReference> pReference = ResourceManager<PixelShaderReference>::Load( data.c_str(), &loader, false );
			tech->m_pixelShader = pReference;
			file >> data;
		}
		else
			assert( 0 );

		if( data.compare( "<RENDERSTATE>" ) == 0 )
		{
			while( data.compare("</RENDERSTATE>") != 0 )
			{
				file >> data;
				if( data.compare( "ALPHA" ) == 0 )
				{
					file >> data;
					if( data.compare( "Test" ) == 0 )
						tech->m_type = 3;
					else if( data.compare( "Additive" ) == 0 )
						tech->m_type = 1;
					else if( data.compare( "Partial" ) == 0 )
						tech->m_type = 2;
					else if( data.compare( "SceneBlend" ) == 0 )
						tech->m_type = 4;
					else
						tech->m_type = 1; // safe guard
				}
			}

			file >> data;
		}

		*t = tech;

		if( data.compare( "</Technique>" ) == 0 )
			file >> data;
	}

	file.close();

	return reference;

}