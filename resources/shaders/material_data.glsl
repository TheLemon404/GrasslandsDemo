struct MaterialData {
    vec3 albedo;
    vec3 shadowColor;
    float roughness;
    sampler2D baseTexture;
    int hasBaseTexture;
};
