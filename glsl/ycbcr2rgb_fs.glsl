// by Jan Eric Kyprianidis <www.kyprianidis.com>
uniform sampler2D edges;
uniform sampler2D color;
uniform int nbins;
uniform float phi_q;
uniform vec2 img_size;

vec3 ycbcr2rgb(vec3 c) {
    return c * mat3(
        1.000,  0.000,  1.402,
        1.000, -0.344, -0.714,
        1.000,  1.772,  0.000
    );
}

vec3 lab2xyz( vec3 c ) {
    float fy = ( c.x + 16.0 ) / 116.0;
    float fx = c.y / 500.0 + fy;
    float fz = fy - c.z / 200.0;
    return vec3(
         95.047 * (( fx > 0.206897 ) ? fx * fx * fx : ( fx - 16.0 / 116.0 ) / 7.787),
        100.000 * (( fy > 0.206897 ) ? fy * fy * fy : ( fy - 16.0 / 116.0 ) / 7.787),
        108.883 * (( fz > 0.206897 ) ? fz * fz * fz : ( fz - 16.0 / 116.0 ) / 7.787)
    );
}

vec3 xyz2rgb( vec3 c ) {
    vec3 v =  c / 100.0 * mat3( 
        3.2406, -1.5372, -0.4986,
        -0.9689, 1.8758, 0.0415,
        0.0557, -0.2040, 1.0570
    );
    return vec3(
        ( v.r > 0.0031308 ) ? (( 1.055 * pow( v.r, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.r,
        ( v.g > 0.0031308 ) ? (( 1.055 * pow( v.g, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.g,
        ( v.b > 0.0031308 ) ? (( 1.055 * pow( v.b, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.b
    );
}

vec3 lab2rgb(vec3 c) {
    return xyz2rgb( lab2xyz( vec3(100.0 * c.x, 2.0 * 127.0 * (c.y - 0.5), 2.0 * 127.0 * (c.z - 0.5)) ) );
}

void main (void) {
    vec2 uv = gl_FragCoord.xy / img_size;
    vec2 d = 1.0 / img_size;
    vec3 c = texture2D(color, uv).xyz;

    float s = ( 1.0 * texture2D(edges, uv + vec2(-d.x, -d.x)).x +
                4.0 * texture2D(edges, uv + vec2( 0.0, -d.x)).x +
                1.0 * texture2D(edges, uv + vec2( d.x, -d.x)).x +
                4.0 * texture2D(edges, uv + vec2(-d.x,  0.0)).x +
               16.0 * texture2D(edges, uv + vec2( 0.0,  0.0)).x +
                4.0 * texture2D(edges, uv + vec2( d.x,  0.0)).x +
                1.0 * texture2D(edges, uv + vec2(-d.x,  d.x)).x +
                4.0 * texture2D(edges, uv + vec2( 0.0,  d.x)).x +
                1.0 * texture2D(edges, uv + vec2( d.x,  d.x)).x
              ) / 36.0;

    float qn = floor(c.x * float(nbins) + 0.5) / float(nbins);
    float qs = smoothstep(-2.0, 2.0, phi_q * (c.x - qn) * 100.0) - 0.5;
    float qc = qn + qs / float(nbins);

    float y = s * qc;
    gl_FragColor = vec4( lab2rgb(vec3(y, c.yz)), 1.0 );
}
