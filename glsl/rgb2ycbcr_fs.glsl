// by Jan Eric Kyprianidis <www.kyprianidis.com>

uniform sampler2D img;
uniform vec2 img_size;
varying out vec4 result0;
varying out vec4 result1;

vec3 rgb2ycbcr(vec3 c) {
    return c * mat3(
        0.299,  0.587,  0.114,
       -0.169, -0.331,  0.500,
        0.500, -0.419, -0.081
    );
}

vec3 rgb2xyz( vec3 c ) {
    return 100.0 * 
        vec3(( c.r > 0.04045 ) ? pow( ( c.r + 0.055 ) / 1.055, 2.4 ) : c.r / 12.92,
             ( c.g > 0.04045 ) ? pow( ( c.g + 0.055 ) / 1.055, 2.4 ) : c.g / 12.92,
             ( c.b > 0.04045 ) ? pow( ( c.b + 0.055 ) / 1.055, 2.4 ) : c.b / 12.92 ) * 
        mat3( 0.4124, 0.3576, 0.1805,
              0.2126, 0.7152, 0.0722,
              0.0193, 0.1192, 0.9505 );
}

vec3 xyz2lab( vec3 c ) {
    vec3 n = c / vec3( 95.047, 100, 108.883 );
    vec3 v = vec3(
        ( n.x > 0.008856 ) ? pow( n.x, 1.0 / 3.0 ) : ( 7.787 * n.x ) + ( 16.0 / 116.0 ),
        ( n.y > 0.008856 ) ? pow( n.y, 1.0 / 3.0 ) : ( 7.787 * n.y ) + ( 16.0 / 116.0 ),
        ( n.z > 0.008856 ) ? pow( n.z, 1.0 / 3.0 ) : ( 7.787 * n.z ) + ( 16.0 / 116.0 )
    );
    return vec3(( 116.0 * v.y ) - 16.0, 500.0 * ( v.x - v.y ), 200.0 * ( v.y - v.z ));
}

vec3 rgb2lab(vec3 c) {
    vec3 lab = xyz2lab( rgb2xyz( c ) );
    return vec3( lab.x / 100.0, 0.5 + 0.5 * ( lab.y / 127.0 ), 0.5 + 0.5 * ( lab.z / 127.0 ));
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
    vec4 c = texture2D(img, uv);

    vec2 d = 1.0 / img_size;

    vec4 u = (
             -1.0 * texture2D(img, uv + vec2(-d.x, -d.y)) +
             -2.0 * texture2D(img, uv + vec2(-d.x,  0.0)) + 
             -1.0 * texture2D(img, uv + vec2(-d.x,  d.y)) +
             +1.0 * texture2D(img, uv + vec2( d.x, -d.y)) +
             +2.0 * texture2D(img, uv + vec2( d.x,  0.0)) + 
             +1.0 * texture2D(img, uv + vec2( d.x,  d.y))
             ) / 4.0;

    vec4 v = (
             -1.0 * texture2D(img, uv + vec2(-d.x, -d.y)) + 
             -2.0 * texture2D(img, uv + vec2( 0.0, -d.y)) + 
             -1.0 * texture2D(img, uv + vec2( d.x, -d.y)) +
             +1.0 * texture2D(img, uv + vec2(-d.x,  d.y)) +
             +2.0 * texture2D(img, uv + vec2( 0.0,  d.y)) + 
             +1.0 * texture2D(img, uv + vec2( d.x,  d.y))
             ) / 4.0;

    gl_FragData[0] = vec4(vec3(dot(u.xyz, u.xyz), 
                         dot(v.xyz, v.xyz), 
                         dot(u.xyz, v.xyz)), 1.0);

    gl_FragData[1] = vec4( rgb2lab(c.rgb), 1.0 );
}

