#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <vector>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <filesystem>
#include <fstream>

#include "Camera1.h"
#include "Ray.h"
#include "GeoPrims.h"
#include "LightSource.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include "Scene.h"
#include "Hit.h"

const int MAX_LEVELS = 5;
const glm::vec3 BACKROUND_COLOR(0.f);
const glm::vec3 MATERIAL_EMISSION(0.f);
const glm::vec3 MATERIAL_SPECULAR(0.7f, 0.7f, 0.7f);

const std::string DEBUG_SCENE_PATH = "res/debug_scene.txt";
std::ofstream DEBUG_OUTPUT_FILE(DEBUG_SCENE_PATH);

glm::vec3 rayTrace(const std::shared_ptr<Hit> &hit, 
                   const Ray &ray, int level,
                   const std::vector<std::shared_ptr<GeoPrim>> &objects,
                   const std::vector<std::shared_ptr<LightSource>> &lights,
                   const glm::vec3 &ambientIntensity) {
    if (level == MAX_LEVELS) {
        return glm::vec3(0.f); // Base case for recursion
    }
    
    // Compute local illumination by Phong model
    const std::shared_ptr<GeoPrim> hitObject = hit->getObject(); // Cache object pointer
    const glm::vec3 hitPoint = hit->getPoint();
    const glm::vec3 objectColor = hitObject->getColorByPoint(hitPoint);
    glm::vec3 color = MATERIAL_EMISSION + ambientIntensity * objectColor; //Ie + Ka * Ia
    // Per-light Phong shading (compute L per light; avoid relying on light->intensityAt for consistency)
    const glm::vec3 N = hit->getNormal();
    const glm::vec3 viewDir = glm::normalize(ray.getOrigin() - hitPoint);
    for (const std::shared_ptr<LightSource>& light : lights) {
        if (!light->isIlluminated(objects, hit)) continue;

        glm::vec3 L; // direction from point to light
        glm::vec3 lightIntensity = light->getIntensity();
        float spotFactor = 1.0f;

        if (DirectionalLight* dl = dynamic_cast<DirectionalLight*>(light.get())) {
            L = -dl->getDirection();
        } else if (Spotlight* sl = dynamic_cast<Spotlight*>(light.get())) {
            // vector from light to point
            glm::vec3 lightToPoint = hitPoint - sl->getPosition();
            float len = glm::length(lightToPoint);
            if (len <= 0.0f) continue;
            glm::vec3 lightToPointN = lightToPoint * (1.0f / len);
            // spot factor: dot between spotlight beam direction and vector from light to point
            spotFactor = glm::max(0.0f, glm::dot(sl->getDirection(), lightToPointN));
            // L is direction from point to light
            L = -lightToPointN;
        } else {
            continue; // unknown light type
        }

        float lambert = glm::max(0.0f, glm::dot(N, L));
        glm::vec3 diffuse = objectColor * lightIntensity * lambert * spotFactor;
        glm::vec3 R = 2.0f * glm::dot(N, L) * N - L;
        float RdotV = glm::max(0.0f, glm::dot(R, viewDir));
        float specFactor = std::pow(RdotV, static_cast<float>(hitObject->getN()));
        glm::vec3 specular = MATERIAL_SPECULAR * lightIntensity * specFactor * spotFactor;

        color += diffuse + specular;
    }

    // Reflection logic
    if(hitObject->isReflective()) {
        color = glm::vec3(0.f); // Reset color to accumulate reflection only
        Ray outRay(hitPoint, hit->getReflectionDir());
        std::shared_ptr<Hit> reflectionHit = outRay.findClosestIntersection(objects);
        if (reflectionHit != nullptr) {
            // Pass Hit result directly to avoid recalculation
            color += hitObject->getReflectionIntensity() * rayTrace(reflectionHit, outRay, level + 1, objects, lights, ambientIntensity);
        }
    }
    
    // Refrafction logic
    if(hitObject->isTransparent()) {
        color = glm::vec3(0.f); // Reset color to accumulate refraction only
        Sphere* spherePtr = dynamic_cast<Sphere*>(hitObject.get());
        Ray exitRay = spherePtr->getRefractionExitRay(hit, ray);
        // glm::vec3 L = ray.getDirection();
        // glm::vec3 N = hit->getNormal();
        // float cosL = glm::dot(-L, N);
        // float eta = 1.0f / 1.5f; // index of refraction ratio
        // glm::vec3 Rr = (eta * cosL - eta * glm::sqrt(1 - cosL * cosL)) * N - eta * L;
        // Ray refractedRay(hitPoint - N * 0.001f, Rr);
        // std::shared_ptr<Hit> refractionHit = refractedRay.findClosestIntersection(objects);
        // if(refractionHit == nullptr) {
        //     return color; // No further refraction
        // }
        // glm::vec3 exitN = refractionHit->getNormal();
        // float exitEta = 1.5f / 1.0f;
        // float cosRr = glm::dot(refractedRay.getDirection(), exitN);
        // glm::vec3 exitDirection = (exitEta * cosRr - exitEta * glm::sqrt(1 - cosRr * cosRr)) * exitN - exitEta * refractedRay.getDirection();
        // Ray exitRay(refractionHit->getPoint() + exitN * 0.001f, exitDirection);
        std::shared_ptr<Hit> exitHit = exitRay.findClosestIntersection(objects);
        if (exitHit != nullptr) {
            // Pass Hit result directly to avoid recalculation
            color += hitObject->getReflectionIntensity() * rayTrace(exitHit, exitRay, level + 1, objects, lights, ambientIntensity);
        }
    }

    return color;
}

int rayTraceScene(std::string sceneFilePath)
{
    std::shared_ptr<Scene> scene = Scene::parseToScene(sceneFilePath);
    if(scene == nullptr) {
        std::cout << "Error parsing scene." << std::endl;
        return -1;
    }

    DEBUG_OUTPUT_FILE << "Scene parsed successfully from: " << sceneFilePath << "the scene: " << scene->toString() << "\n";
    std:: cout << "Scene parsed successfully from: " << sceneFilePath << std::endl;
    // Cache frequently accessed scene data
    const std::vector<std::shared_ptr<GeoPrim>>& objects = scene->getObjects();
    const std::vector<std::shared_ptr<LightSource>>& lights = scene->getLights();
    const glm::vec3 ambientIntensity = scene->getAmbientLight().getIntensity();
    const Camera& camera = scene->getCamera();
    
    // Ray tracer
    const int PIXEL_WIDTH = 1000;
    const int PIXEL_HEIGHT = 1000;
    unsigned char* image = new unsigned char[PIXEL_WIDTH * PIXEL_HEIGHT * 3]{0}; // 1000x1000 rgb image initialized to background color
    
    for (int y = 0; y < PIXEL_HEIGHT; y++) {
        for (int x = 0; x < PIXEL_WIDTH; x++) {
            std::vector<Ray> rays = camera.castRaysThroughPixel(x, y);
            glm::vec3 color = BACKROUND_COLOR;
            int rayCount = rays.size();
            
            for (const Ray &ray : rays) {
                std::shared_ptr<Hit> hit = ray.findClosestIntersection(objects);
                if (hit != nullptr) {
                    // Pass Hit result directly to avoid recalculation
                    color += rayTrace(hit, ray, 0, objects, lights, ambientIntensity);
                }
            }
            // Use multiplication instead of division (faster)
            if (rayCount > 1) {
                color *= (1.0f / static_cast<float>(rayCount));
            }
            int idx = (y * PIXEL_WIDTH + x) * 3;
            image[idx] = static_cast<unsigned char>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
            image[idx + 1] = static_cast<unsigned char>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
            image[idx + 2] = static_cast<unsigned char>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));
            // std::cout << "Written Pixel (" << x << ", " << y << ") with color (" << glm::clamp(color.r * 255.0f, 0.0f, 255.0f) << ", " << glm::clamp(color.g * 255.0f, 0.0f, 255.0f) << ", " << glm::clamp(color.b * 255.0f, 0.0f, 255.0f) << ") to image buffer.\n";
        }
    }
    std::string outputPath = std::string("res/results/image1.png");
    int result = stbi_write_png(outputPath.c_str(), PIXEL_WIDTH, PIXEL_HEIGHT, 3, image, PIXEL_WIDTH * 3);
    delete[] image;
    if (!result){
    std::cout << "Failed to write image to: " << outputPath << std::endl;
    }
    return result;
}


int main() {
    auto start = std::chrono::steady_clock::now();
    std::string sceneFilePath = "../files/scene6.txt"; //placeholder
    rayTraceScene(sceneFilePath);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Rendering time (ms): " << duration.count()  << std::endl;
    return 0;
}