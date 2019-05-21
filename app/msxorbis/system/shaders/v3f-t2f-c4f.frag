#version 100
precision mediump float;
uniform sampler2D texture;

varying vec2 vTexCoord;
varying vec4 fragColor;

void main()
{
    float a = texture2D(texture, vTexCoord).a;
    gl_FragColor = vec4(fragColor.rgb, fragColor.a*a);
}
