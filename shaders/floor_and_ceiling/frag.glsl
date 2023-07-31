#version 410 core

in vec2 RenderCoord;
out vec4 FragColor;

uniform sampler1DArray texCoordData;
uniform sampler2D ceilingTexture;
uniform sampler2D floorTexture;

uniform vec3 ambiant;

void main()
{
	if (RenderCoord.y > 0) {
		vec2 sampleOrigin = vec2(
			texture(texCoordData, vec2(RenderCoord.y, 0)).x,
			texture(texCoordData, vec2(RenderCoord.y, 1)).x
		);
		vec2 sampleOffset = vec2(
			texture(texCoordData, vec2(RenderCoord.y, 2)).x,
			texture(texCoordData, vec2(RenderCoord.y, 3)).x
		);

		vec2 samplePos = mod(sampleOrigin + sampleOffset * RenderCoord.x, 0.5) / 2;

		FragColor = texture(ceilingTexture, samplePos);
	}
	else {
		vec2 sampleOrigin = vec2(
			texture(texCoordData, vec2(-RenderCoord.y, 0)).x,
			texture(texCoordData, vec2(-RenderCoord.y, 1)).x
		);
		vec2 sampleOffset = vec2(
			texture(texCoordData, vec2(-RenderCoord.y, 2)).x,
			texture(texCoordData, vec2(-RenderCoord.y, 3)).x
		);

		vec2 samplePos = mod(sampleOrigin + sampleOffset * RenderCoord.x, 0.5) / 2;

		FragColor = texture(floorTexture, samplePos);
	}

	//Lightning
	FragColor.x = FragColor.x * ambiant.x;
	FragColor.y = FragColor.y * ambiant.y;
	FragColor.z = FragColor.z * ambiant.z;

	FragColor.w = 0;
}