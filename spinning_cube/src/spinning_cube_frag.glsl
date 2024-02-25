#version 330 core
uniform float iTime;
uniform vec2 iResolution;
out vec4 FragColor;

const float PI = acos(-1.0);

float sd_box(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sd_round_box(vec3 p, vec3 b, float r)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

vec3 op_tx(in vec3 p, in mat4 t)
{
    return (vec4(p, 1) * inverse(t)).xyz;
}

mat4 rot_x(in float t)
{
    return mat4(
        vec4(1,      0,       0, 0),
        vec4(0, cos(t), -sin(t), 0),
        vec4(0, sin(t),  cos(t), 0),
        vec4(0,      0,       0, 1)
    );
}

mat4 rot_y(in float t)
{
    return mat4(
        vec4( cos(t), 0, sin(t), 0),
        vec4(      0, 1,      0, 0),
        vec4(-sin(t), 0, cos(t), 0),
        vec4(      0, 0,      0, 1)
    );
}

float map( in vec3 pos )
{
    float t = mod(iTime / 2.0, PI);
    mat4 Ry = rot_y(t);
    mat4 Rx = rot_x(PI / 6.0);
    float d = sd_round_box(op_tx(pos, Ry * Rx), vec3(0.15, 0.15, 0.15), 0.05);
    return d;
}

vec3 calc_normal( in vec3 pos )
{
    // method of small differences
    vec2 e = vec2(0.0001, 0.0);
    return normalize(
        vec3(
            map(pos+e.xyy) - map(pos-e.xyy),
            map(pos+e.yxy) - map(pos-e.yxy),
            map(pos+e.yyx) - map(pos-e.yyx)
        )
    );
}

void main()
{
    float aspect = iResolution.x/iResolution.y;
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 p = uv - 1.0f;
    p.x *= aspect;

    vec3 col = vec3(0.0);

    // ray marching
    // ray origin
    vec3 ro = vec3(0.0, 0.0, 1.0);
    vec3 rd = normalize (vec3(p, -1.5)); // field of view controller -1.5 ( near view plane)

    float t = 0.0;
    for ( int i=0; i < 100; i++ )
    {
        // creates sampling points starting from camera origin
        vec3 pos = ro + t*rd;

        float h = map(pos);
        if ( h < 0.001) // distance is negative so "inside"
            break;
        t += h;
        if ( t > 20.0) // if we explore too much without intersection we break
            break;
    }

    if (t < 20.0 )
    {
        vec3 pos = ro + t*rd;
        vec3 nor = calc_normal(pos);

        // key light
        vec3 sun_dir = normalize(vec3(0.8, 0.4, 0.2));
        float sun_dif = clamp( dot(nor, sun_dir), 0.0, 1.0);

        // sky light
        vec3 sky_dir = normalize(vec3(0.0, 1.0, 0.0));
        // we bias with 0.5 + 0.5* to get a little light on the bottom side
        float sky_dif = clamp( 0.5 + 0.5*dot(nor, sky_dir), 0.0, 1.0) * 0.2;

        col = vec3(1.0, 0.7, 0.5)*sun_dif;
        col += vec3(0.0, 0.2, 0.3)*sky_dif;

        // gamma correction
        col = pow(col, vec3(1.0/2.2));
    }

    FragColor = vec4(col, 1.0);
} 