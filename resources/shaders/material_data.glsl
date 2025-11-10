struct MaterialData {
    vec3 albedo;
    vec3 shadowColor;
    float roughness;
    float shininess;
    sampler2D baseTexture;
    int hasBaseTexture;
};
