uniform sampler2D texture;
uniform int mode;

void main()
{
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);
    vec3 rgb = color.rgb;
    vec3 transformed = rgb;

    if (mode == 1) {
        mat3 matrix = mat3(
            0.567, 0.433, 0.0,
            0.558, 0.442, 0.0,
            0.0,   0.242, 0.758
        );
        transformed = matrix * rgb;
    }
    else if (mode == 2) {
        mat3 matrix = mat3(
            0.625, 0.375, 0.0,
            0.700, 0.300, 0.0,
            0.0,   0.300, 0.700
        );
        transformed = matrix * rgb;
    }
    else if (mode == 3) {
        mat3 matrix = mat3(
            0.950, 0.050, 0.0,
            0.0,   0.433, 0.567,
            0.0,   0.475, 0.525
        );
        transformed = matrix * rgb;
    }
    else if (mode == 4) {
        mat3 matrix = mat3(
            0.817, 0.183, 0.0,
            0.333, 0.667, 0.0,
            0.0,   0.125, 0.875
        );
        transformed = matrix * rgb;
    }
    else if (mode == 5) {
        mat3 matrix = mat3(
            0.800, 0.200, 0.0,
            0.258, 0.742, 0.0,
            0.0,   0.142, 0.858
        );
        transformed = matrix * rgb;
    }
    else if (mode == 6) {
        mat3 matrix = mat3(
            0.967, 0.033, 0.0,
            0.0,   0.733, 0.267,
            0.0,   0.183, 0.817
        );
        transformed = matrix * rgb;
    }

    gl_FragColor = vec4(transformed, color.a);
}
