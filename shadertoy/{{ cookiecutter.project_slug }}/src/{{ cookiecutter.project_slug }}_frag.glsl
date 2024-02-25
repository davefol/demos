#version 330 core
uniform float iTime;
uniform vec2 iResolution;
out vec4 FragColor;

void main()
{
    float aspect = iResolution.x/iResolution.y;
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 p = uv - 1.0f;
    p.x *= aspect;

    vec3 col = vec3((sin(iTime) + 1.0)/2.0, 0.5f, 0.2f);
    FragColor = vec4(col, 1.0f);
} 