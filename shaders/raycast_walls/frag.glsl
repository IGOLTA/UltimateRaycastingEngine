#version 410 core

#define XP 0
#define XN 1
#define YP 2
#define YN 3

in vec2 RenderCoord;
out vec4 FragColor;

uniform float HEIGHT_FACTOR;
uniform float TILE_PER_SPRITE;

uniform sampler1DArray raycastData;
uniform sampler2D baseColor;

uniform int currentTile;

uniform vec3 ambiant;
uniform sampler1DArray lightsData;
uniform int lightsCount;

void main()
{
    float lineHeight = HEIGHT_FACTOR / texture(raycastData, vec2(RenderCoord.x, 0)).x;
    vec3 position = vec3(
        texture(raycastData, vec2(RenderCoord.x, 2)).x,
        texture(raycastData, vec2(RenderCoord.x, 3)).x,
        (RenderCoord.y + lineHeight) / (lineHeight * 2)
    );
    int side = int(round(texture(raycastData, vec2(RenderCoord.x, 4)).x));
    int tileId = int(round(texture(raycastData, vec2(RenderCoord.x, 1)).x));

    //Texture coords
    float offset = 0;
    if (side <= 1) offset = position.y;
    else offset = position.x;

    vec2 TexCoord = vec2(
        mod(offset, TILE_PER_SPRITE) / TILE_PER_SPRITE,
        (RenderCoord.y/ lineHeight + 1) / 2.0f
    );

    //BaseColor
    if (
        position.z >= 0 &&
        position.z <= 1 &&
        tileId == currentTile
    ) {
        FragColor = texture(baseColor, TexCoord);
    }
    else {
        FragColor = vec4(0, 0, 0, 0);
    }

    //Lightning
    vec3 lightFinalColor = ambiant;

    vec3 normal = vec3(0, 0, 0);
    /*
    switch (side) {
    case XP:
        normal = vec3(1, 0, 0);
        break;
    case XN:
        normal = vec3(-1, 0, 0);
        break;
    case YP:
        normal = vec3(0, 1, 0);
        break;
    case YN:
        normal = vec3(0, -1, 0);
        break;
    }


    for (int i = 0; i < lightsCount; i++) {
        
        vec3 lightRelativePos = texelFetch(lightsData, ivec2(i, 0), 0).xyz - position;
        FragColor.xyz = texture(lightsData, vec2(i / lightsCount, 0), 0).xyz;
        vec3 lightDir = normalize(lightRelativePos);
        float distance = length(lightRelativePos) / 50;
        lightFinalColor += max(dot(normal, lightDir), 0.0) * texelFetch(lightsData, ivec2(i, 0), 0).xyz * distance;
    }*/

    FragColor.x = lightFinalColor.x * FragColor.x;
    FragColor.y = lightFinalColor.y * FragColor.y;
    FragColor.z = lightFinalColor.z * FragColor.z;
}