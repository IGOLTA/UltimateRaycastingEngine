#version 410 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;
void main()
{

    float distance = texture(textureSampler, vec2(TexCoord.x, 0.0f)).x;

    float lineHeight = 1 / (10 * distance);

    if (abs(TexCoord.y) < lineHeight) {
        FragColor = vec4(1 * lineHeight, 1 * lineHeight, 1 * lineHeight, 1);
    }
    else {
        FragColor = vec4(0, 0, 0, 1);
    }


}