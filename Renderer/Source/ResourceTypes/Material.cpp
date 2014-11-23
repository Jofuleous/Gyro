#include "Material.h"
#include "Entity.h"

//all this file reading shit i need.
#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>
#include <vector>
#include <malloc.h>
//#include "Scene.h"
#include "../Camera.h"
#include <assert.h>
#include <string>

MaterialReference* MaterialLoader::Load( const char* i_filename )
{
	char filepath[ 128 ];
	sprintf( filepath, "data//Materials//%s", i_filename );

	std::ifstream file( filepath );

	if( !file )
	{
		std::cerr << "Could not find file: \"" << filepath << "\"";
		assert( 0 );
		return false;
	}

	MaterialReference* reference = new MaterialReference();

	reference->rm_Reference.m_environmentMap = false;

	std::string ignore;
	std::string data;

	file >> data;

	if( data.compare( "NAME" ) == 0 )
	{
		file >> data;

		char* name = (char*) malloc( 64 );
		strcpy( name, data.c_str() ); 
		//m_materialName = name;
		file >> data;
		free( name );
	}
	//else
	//	m_materialName = "NO_NAME";

	if( data.compare( "EFFECT" ) == 0 )
	{
		file >> data;
		char effectName[128];
		strcpy( effectName, data.c_str() );
		EffectLoader loader;
		reference->rm_Reference.m_effect = ResourceManager<EffectReference>::Load( effectName, &loader, false );
	}
	else
		assert( 0 );

	file >> data;
	if( data.compare( "<PARAMETERS>" ) == 0 )
	{
		file >> data;

		while( data.compare( "</PARAMETERS>" ) != 0 )
		{
			if( data.compare( "TEXTURE" ) == 0 )
			{
				file >> data;
				char* paramName = (char*) malloc( data.size() + 1 );
				strcpy( paramName, data.c_str() );

				file >> data;
				char* filename = (char*) malloc( data.size() +1 );
				strcpy( filename, data.c_str() );

				TextureLoader loader;
				//overrode the compare...
				ResourcePtr<TextureReference> textureReference = ResourceManager<TextureReference>::Load( filename, &loader, false );
				reference->rm_Reference.m_textures.push_back( textureReference );
				reference->rm_Reference.m_textureNames.push_back( paramName );
				free( filename );
			}
			if( data.compare( "SCENETEXTURE" ) == 0 )
			{
				file >> data;
			}
			else if( data.compare( "PFLOAT" ) == 0 )
			{
				file >> data;
				char* name = (char*) malloc( data.size() + 1 );
				strcpy( name, data.c_str() );
				float value;
				file >> value;
				reference->rm_Reference.m_floats.push_back( value );
				reference->rm_Reference.m_floatNames.push_back( name );
			}
			else if( data.compare( "CAMERA" ) == 0 )
			{
				file >> data;
				char* name = (char*) malloc( data.size() );
				reference->rm_Reference.m_requiresCamera = true;
			}
			else if( data.compare( "ALPHA" ) == 0 )
			{
				file >> data;
				reference->rm_Reference.m_alpha = (float) atof( data.c_str() );
			}
			else if( data.compare( "ENVIROMAP" ) == 0 )
			{
				file >> data;
				reference->rm_Reference.m_environmentMap = true;
			}

			file >> data;
		}
	}

	reference->rm_Reference.m_materialId = ResourceManager<MaterialReference>::m_nextId;

	file.close();

	return reference;
}


//void Material::SetMaterialData( Entity* i_entity )
//{
	/*
	EffectData effectData = m_effect->rm_Reference;
	VertexShaderData vData = m_effect->rm_Reference.m_vertexShader->rm_Reference;
	PixelShaderData pData = m_effect->rm_Reference.m_pixelShader->rm_Reference;

	// ALL MATERIALS USE THESE.  AT LEAST FOR NOW.
	HRESULT result;
	// Set the shader programs
	{
		result = cs6963::cRenderer::GetDevice()->SetVertexShader( vData.m_vertexShader );
		assert( SUCCEEDED( result ) );
		result = cs6963::cRenderer::GetDevice()->SetPixelShader( effectData.m_pixelShader->rm_Reference.m_pixelShader );
		assert( SUCCEEDED( result ) );

		// SECONDS ELAPSED..
		{
			float secondsElapsed = 0;
			D3DXHANDLE variable;
			{
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_secondsElapsed";
				variable = vData.m_vertexShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				vData.m_vertexShaderConstantTable->
			}
			if ( variable != NULL )
			{
				result = vData.m_vertexShaderConstantTable->SetFloat( cs6963::cRenderer::GetDevice(), variable, secondsElapsed );
				assert( SUCCEEDED( result ) );
			}
		}

		// WORLD MATRIX
		{
			D3DXHANDLE variable;
			{
				D3DXHANDLE topLevelVariable = NULL;
				const char* variableName = "g_world";
				variable = vData.m_vertexShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
			}
			if ( variable != NULL )
			{
				D3DXMATRIX worldRotation;
				D3DXMatrixRotationY( &worldRotation, i_entity->m_orientationY );

				D3DXMATRIX worldTranslation;
				D3DXMatrixTranslation( &worldTranslation, i_entity->m_position.x, i_entity->m_position.y, i_entity->m_position.z );

				D3DXMATRIX worldTransformation = worldRotation * worldTranslation;

				result = vData.m_vertexShaderConstantTable->SetMatrix( cs6963::cRenderer::GetDevice(), variable, &worldTransformation );
				assert( SUCCEEDED( result ) );
			}
		}

		// VIEW MATRIX
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_view";
			variable = vData.m_vertexShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				D3DXMATRIX view = Scene::GetCamera()->ViewTransform();
				result =  vData.m_vertexShaderConstantTable->SetMatrix( cs6963::cRenderer::GetDevice(), variable, &view );
				assert( SUCCEEDED( result ) );
			}
		}

		// PROJECTION MATRIX
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_projection";
			variable = vData.m_vertexShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				D3DXMATRIX projection = Scene::GetCamera()->ProjectionTransform();
				result =  vData.m_vertexShaderConstantTable->SetMatrix( cs6963::cRenderer::GetDevice(), variable, &projection );
				assert( SUCCEEDED( result ) );
			}
		}

		
		// LIGHT POSITION.  For now, let's make the assumption that all of our effects use a light...
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_lightPosition";
			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				D3DXVECTOR4 lightPos = cs6963::cRenderer::GetCurrentScene()->m_pointLight->m_position;
				result =  pData.m_pixelShaderConstantTable->SetVector( cs6963::cRenderer::GetDevice(), variable, &lightPos );
				assert( SUCCEEDED( result ) );
			}
		}

		// LIGHT PARMS
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_lightParms";
			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				D3DXVECTOR4 lightParms = cs6963::cRenderer::GetCurrentScene()->m_pointLight->m_color;
				lightParms.w = cs6963::cRenderer::GetCurrentScene()->m_pointLight->m_intensity;
				result =  pData.m_pixelShaderConstantTable->SetVector( cs6963::cRenderer::GetDevice(), variable, &lightParms );
				assert( SUCCEEDED( result ) );
			}
		}
		// AMBIENT LIGHT
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;
			const char* variableName = "g_ambientLight";
			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				D3DXVECTOR4 light = cs6963::cRenderer::GetCurrentScene()->m_ambientLight;
				result =  pData.m_pixelShaderConstantTable->SetVector( cs6963::cRenderer::GetDevice(), variable, &light );
				assert( SUCCEEDED( result ) );
			}
		}

		//Set the textures
		std::map<const char*, ResourcePtr<TextureReference>,compare>::iterator it = m_textureMap.begin();
		while( it != m_textureMap.end() )
		{
			const char* variableName = it->first;
			LPDIRECT3DTEXTURE9 texture = it->second->rm_Reference;

			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;

			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				UINT index =  pData.m_pixelShaderConstantTable->GetSamplerIndex( variable );
				cs6963::cRenderer::GetDevice()->SetTexture( index, texture);
				assert( SUCCEEDED( result ) );
			}

			it++;
		}

		// SET FLOATS..
		std::map<const char*, float,compare>::iterator itfloat = m_pixelFloatMap.begin();
		while( itfloat != m_pixelFloatMap.end() )
		{
			const char* variableName = itfloat->first;
			float value = itfloat->second;

			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;

			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, variableName );
				
			if ( variable != NULL )
			{
				result =  pData.m_pixelShaderConstantTable->SetFloat( cs6963::cRenderer::GetDevice(), variable, value );
				assert( SUCCEEDED( result ) );
			}

			itfloat++;
		}

		// SET PIXEL CAMERA
		if( m_requiresCamera )
		{
			D3DXHANDLE variable;
			D3DXHANDLE topLevelVariable = NULL;

			variable = pData.m_pixelShaderConstantTable->GetConstantByName( topLevelVariable, "g_cameraPosition" );
				
			if ( variable != NULL )
			{
				D3DXVECTOR4 cameraPos = cs6963::cRenderer::GetCurrentScene()->m_camera->m_position;
				result =  pData.m_pixelShaderConstantTable->SetVector( cs6963::cRenderer::GetDevice(), variable, &cameraPos );
				assert( SUCCEEDED( result ) );
			}
		}


		//Set the render state
		
	}
	*/
//}