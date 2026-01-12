#version 300 es
precision highp float;
precision highp int;

struct Camera {
    vec3 pos;
    vec3 forward;
    vec3 right;
    vec3 up;
};

struct Plane {
    vec3 point;
    vec3 normal;
    vec3 color;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    int type; // 0: opaque, 1: reflective, 2: refractive
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float shininess;
    float cutoff; // if > 0.0 then spotlight else directional light
};

struct HitInfo {
    vec3 rayOrigin;
    vec3 rayDir;
    float t;
    vec3 baseColor;
    int inside; // 1 if inside the sphere, 0 otherwise
    vec3 hitPoint;
    vec3 normal;
    int type; // 0: diffuse, 1: reflective
};

const int TYPE_DIFFUSE = 0;
const int TYPE_REFLECTIVE = 1;
const int TYPE_REFRACTIVE = 2;

const int MAX_SPHERES = 16;
const int MAX_LIGHTS = 4;
const int MAX_DEPTH = 5;

in vec2 vUV;
out vec4 FragColor;

uniform float uTime;
uniform ivec2 uResolution; // width and height of canvas


uniform Camera cam;
uniform Sphere uSpheres[MAX_SPHERES];
uniform int uNumSpheres;

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;

uniform Plane uPlane;

vec3 checkerboardColor(vec3 rgbColor, vec3 hitPoint) {
    // Checkerboard pattern
    float scaleParameter = 2.0;
    float checkerboard = 0.0;
    if (hitPoint.x < 0.0) {
    checkerboard += floor((0.5 - hitPoint.x) / scaleParameter);
    }
    else {
    checkerboard += floor(hitPoint.x / scaleParameter);
    }
    if (hitPoint.z < 0.0) {
    checkerboard += floor((0.5 - hitPoint.z) / scaleParameter);
    }
    else {
    checkerboard += floor(hitPoint.z / scaleParameter);
    }
    checkerboard = (checkerboard * 0.5) - float(int(checkerboard * 0.5));
    checkerboard *= 2.0;
    if (checkerboard > 0.5) {
    return 0.5 * rgbColor;
    }
    return rgbColor;
}

bool intersectSphere(vec3 rayOrigin, vec3 rayDirection, Sphere s, out float tHit, out vec3 normal, out int inside) {
    // find if the ray intersects the sphere
    vec3 originToCenter = rayOrigin - s.center;
    float b = dot(originToCenter, rayDirection);
    float c = dot(originToCenter, originToCenter) - s.radius * s.radius;
    float h = b*b - c;
    if (h < 0.0) return false;
    // compute the t values and find the nearest positive t
    h = sqrt(h);
    float t1 = -b - h;
    float t2 = -b + h;
    float t = t1 > 0.001 ? t1 : t2;
    if (t < 0.001) return false;
    // call output parameters
    tHit = t;
    vec3 hitPoint = rayOrigin + t * rayDirection;
    normal = normalize(hitPoint - s.center);
    inside = 0;
    // check if the ray is inside the sphere
    if (dot(rayDirection, normal) > 0.0) {
        normal = -normal;
        inside = 1;
    }
    return true;
}

bool intersectPlane(vec3 rayOrigin, vec3 rayDirection, Plane p, out float tHit, out vec3 normal) {
    // find if the ray intersects the plane
    float denom = dot(p.normal, rayDirection);
    if (abs(denom) < 1e-4) return false;
    // compute the t value
    float t = dot(p.point - rayOrigin, p.normal) / denom;
    if (t < 0.001) return false;
    tHit = t;
    normal = normalize(p.normal);
    return true;
}

HitInfo intersectScene(vec3 rayOrigin, vec3 rayDir) {
    HitInfo hit;
    // set to a large value to represent no hit
    hit.t = 1e20;
    hit.type = -1;
    // check intersections with all objects in the scene and keep the closest hit
    // Spheres
    for (int i = 0; i < uNumSpheres; i++) {
        float t;
        vec3 n;
        int inside;
        if (intersectSphere(rayOrigin, rayDir, uSpheres[i], t, n, inside)) {
            if (t < hit.t) {
                hit.t = t;
                hit.hitPoint = rayOrigin + t * rayDir;
                hit.normal = n;
                hit.baseColor = uSpheres[i].color;
                hit.type = uSpheres[i].type;
                hit.inside = inside;
            }
        }
    }
    // Plane
    float tPlane;
    vec3 nPlane;
    if (intersectPlane(rayOrigin, rayDir, uPlane, tPlane, nPlane)) {
        if (tPlane < hit.t) {
            hit.t = tPlane;
            hit.hitPoint = rayOrigin + tPlane * rayDir;
            hit.normal = nPlane;
            hit.baseColor = checkerboardColor(uPlane.color, hit.hitPoint);
            hit.type = TYPE_DIFFUSE;
            hit.inside = 0;
        }
    }
    hit.rayOrigin = rayOrigin;
    hit.rayDir = rayDir;
    return hit;
}

vec3 computeLighting(HitInfo hit) {
    vec3 color = 0.1 * hit.baseColor; // Global ambient
    for (int i = 0; i < uNumLights; i++) {
        Light L = uLights[i];
        vec3 lightDir;
        float attenuation = 1.0;
        if (L.cutoff > 0.0) { // spotlight
            lightDir = normalize(L.position - hit.hitPoint);
            float theta = dot(-lightDir, normalize(L.direction));
            if (theta < L.cutoff) continue;
        } else { // directional
            lightDir = normalize(-L.direction);
        }
        // Shadow ray (hard shadows)
        HitInfo shadow = intersectScene(hit.hitPoint + hit.normal * 0.01, lightDir);
        if (shadow.t < 1e19) continue;
        // Diffuse
        float diff = max(dot(hit.normal, lightDir), 0.0);
        vec3 diffuse = diff * hit.baseColor * L.color;
        // Specular
        vec3 viewDir = normalize(cam.pos - hit.hitPoint);
        vec3 reflectDir = reflect(-lightDir, hit.normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), L.shininess);
        vec3 specular = spec * L.color;
        color += diffuse + specular;
    }
    return color;
}

/* calculates color based on hit data and uv coordinates */
vec3 calcColor(HitInfo hit) {
    vec3 color = vec3(0);
    vec3 attenuation = vec3(1);

    // iterative approach to handle reflections/refractions
    for (int depth = 0; depth < MAX_DEPTH; depth++) {
        if (hit.t > 1e19) {
            color += attenuation * vec3(0.2, 0.3, 0.5); // background
            break;
        }
        if (hit.type == TYPE_DIFFUSE) {
            color += attenuation * computeLighting(hit);
            break;
        }
        if (hit.type == TYPE_REFLECTIVE) {
            attenuation *= hit.baseColor;
            // reflect - built in function returns the reflection direction
            vec3 reflectedRayDir = reflect(hit.rayDir, hit.normal);
            hit = intersectScene(hit.hitPoint + hit.normal * 0.01, reflectedRayDir);
            continue;
        }
        if (hit.type == TYPE_REFRACTIVE) {
            float eta = hit.inside == 1 ? 1.5 : 1.0 / 1.5;
            // refract - built in function returns the refraction direction
            vec3 refractedRayDir = refract(hit.rayDir, hit.normal, eta);
            attenuation *= hit.baseColor;
            // add 0.01 offset to avoid self-intersection
            hit = intersectScene(hit.hitPoint - hit.normal * 0.01, refractedRayDir);
            continue;
        }
    }
    return color;
}

/* scales UV coordinates based on resolution
 * uv given uv are [0, 1] range
 * returns new coordinates where y range [-1, 1] and x scales according to window resolution
 */
vec2 scaleUV(vec2 uv) {
    float aspect = float(uResolution.x) / float(uResolution.y);
    vec2 p = uv * 2.0 - 1.0;
    p.x *= aspect;
    return p;
}

void main() {
    vec2 uv = scaleUV(vUV);
    vec3 rayDir = normalize(cam.forward + uv.x * cam.right + uv.y * cam.up);

    HitInfo hit = intersectScene(cam.pos, rayDir);
    vec3 color = calcColor(hit);

    FragColor = vec4(color, 1.0);
}


