#version 410 core
in vec2 RenderCoord;
out vec4 FragColor;

uniform sampler1DArray textureSampler;

uniform float HEIGHT_FACTOR;
uniform float TILE_PER_SPRITE;

void main()
{
    float lineHeight = HEIGHT_FACTOR / texture(textureSampler, vec2(RenderCoord.x, 0)).x;

    vec2 TexCoord = vec2(
        mod((texture(textureSampler, vec2(RenderCoord.x, 1)).x), TILE_PER_SPRITE),
        ((RenderCoord.y/ lineHeight + 1) / 2.0f)
    );
    


    if (abs(RenderCoord.y) < lineHeight) {
        FragColor = vec4(TexCoord.x, TexCoord.y, 0, 1);
    }
    else {
        FragColor = vec4(0, 0, 0, 1);
    }
}