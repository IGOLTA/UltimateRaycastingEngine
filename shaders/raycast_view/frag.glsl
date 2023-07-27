#version 410 core
in vec2 RenderCoord;
out vec4 FragColor;

uniform float HEIGHT_FACTOR;
uniform float TILE_PER_SPRITE;

uniform sampler1DArray raycastData;
uniform sampler2D baseColor;

uniform int currentTile;


void main()
{
    float lineHeight = HEIGHT_FACTOR / texture(raycastData, vec2(RenderCoord.x, 0)).x;

    vec2 TexCoord = vec2(
        mod((texture(raycastData, vec2(RenderCoord.x, 1)).x), TILE_PER_SPRITE) / TILE_PER_SPRITE,
        ((RenderCoord.y/ lineHeight + 1) / 2.0f)
    );
    
    if (
        abs(RenderCoord.y) < lineHeight &&
        int(round(texture(raycastData, vec2(RenderCoord.x, 2)).x)) == currentTile
        ) {
        FragColor = texture(baseColor, TexCoord);
    }
    else {
        FragColor = vec4(0, 0, 0, 0);
    }
}