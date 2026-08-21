#include <cstdlib>  // For rand()
#include <glm/glm.hpp>
#include <cmath>    // For floor(), M_PI
#include <vector>

class Perlin {
public: 
    // Initialize static members (if declared in perlin.hpp)
    int width;
    int length;
    std::vector<std::vector<glm::vec2>> gradients;

    Perlin(int w = 256, int l = 256) : width(w), length(l) {
        width = w;
        length = l;

        gradients.resize(width, std::vector<glm::vec2>(length));
        srand(time(0));

        // Fill with random normalized vectors
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < length; y++) {
                float angle = 2.0f * 3.14159 * (float)rand() / RAND_MAX;
                gradients[x][y] = glm::vec2(cos(angle), sin(angle));
            }
        }
    }

    float noise(float x, float y) {  // Added return type and class scope
        int x0 = (int)floor(x) % width;
        int y0 = (int)floor(y) % length;
        int x1 = (x0 + 1) % width;
        int y1 = (y0 + 1) % length;

        float sx = x - floor(x);
        float sy = y - floor(y);

        glm::vec2 g00 = gradients[x0][y0];
        glm::vec2 g10 = gradients[x1][y0];
        glm::vec2 g01 = gradients[x0][y1];
        glm::vec2 g11 = gradients[x1][y1];

        glm::vec2 d00(sx, sy);     // To bottom-left
        glm::vec2 d10(1 - sx, sy);     // To bottom-right
        glm::vec2 d01(sx, 1 - sy); // To top-left
        glm::vec2 d11(1 - sx, 1 - sy); // To top-right

        float a = glm::dot(g00, d00);
        float b = glm::dot(g10, d10);
        float c = glm::dot(g01, d01);
        float d = glm::dot(g11, d11);

        // Smooth interpolation (Perlin's fade function)
        float u = sx * sx * sx * (sx * (sx * 6 - 15) + 10);
        float v = sy * sy * sy * (sy * (sy * 6 - 15) + 10);

        // Interpolate
        float bottom = a + u * (b - a);
        float top = c + u * (d - c);
        float result = bottom + v * (top - bottom);

        return result;  // Added: Return the computed value!
    }

};
