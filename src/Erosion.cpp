#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>

#include "../include/perlin.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

double clamp(double a, double b, double c) {
    return std::max(b, std::min(a, c));
}

struct Position {
    int x;
    int y;
    Position(int x, int y) : x(x), y(y) {}
};

struct Droplet {
    Position pos;
    double vol;
    double sediment;
    double vel;

    Droplet(int x, int y, double w = 1.0)
        : pos(x, y), vol(w), sediment(0.0), vel(1.0) {}
};

class Terrain {
public:
    std::vector<std::vector<double>> heightmap;
    int width;
    int length;

    Terrain(const std::vector<std::vector<double>>& map)
        : heightmap(map), length(map.size()), width(map.empty() ? 0 : map[0].size()) {}

    // Get height at position (with bounds checking)
    double getHeight(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= length) return 0.0;
        return heightmap[y][x];
    }

    // Set height (for erosion/deposition)
    void setHeight(int x, int y, double value) {
        if (x >= 0 && x < width && y >= 0 && y < length) {
            heightmap[y][x] = value;
        }
    }

    // Find lowest neighbor(4 - directional)
    Position findLowestNeighbor(int x, int y) const {
        double minHeight = getHeight(x, y);
        Position best(x, y);
        int count = 0;

        // Check 4 neighbors: up, down, left, right
        int dx[] = { 0, 0, -1, 1 };
        int dy[] = { -1, 1, 0, 0 };

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            double h = getHeight(nx, ny);

            if (h - minHeight < 1e-9) {
                count++;
                if (std::rand() % count == 0) { // 1/count probability to replace
                    best = Position(nx, ny);
                }
            }else if (h < minHeight) {
                minHeight = h;
                best = Position(nx, ny);
                count = 1;
            }
        }
        return best;
    }

    void print(const std::string& filename) {
        // Convert heightmap to grayscale image (0–255)
        std::vector<unsigned char> image(width * length);

        // Find min and max height for normalization
        double minH = 1e9, maxH = -1e9;
        for (int y = 0; y < length; ++y) {
            for (int x = 0; x < width; ++x) {
                double h = heightmap[y][x];
                if (h < minH) minH = h;
                if (h > maxH) maxH = h;
            }
        }

        // Normalize and fill image
        double range = maxH - minH;
        if (range == 0) range = 1; // Avoid division by zero

        for (int y = 0; y < length; ++y) {
            for (int x = 0; x < width; ++x) {
                double h = heightmap[y][x];
                int val = static_cast<int>(255 * (h - minH) / range);
                val = clamp(val, 0, 255);
                image[y * width + x] = static_cast<unsigned char>(val);
            }
        }

        // Save as grayscale PNG
        stbi_write_png(filename.c_str(), width, length, 1, image.data(), width);
        std::cout << "Saved terrain to: " << filename << "\n";
    }
};

void erodeDroplet(Terrain& terrain, Droplet& droplet, int steps = 1000) {
    const double EROSION_RATE = 0.3;
    const double DEPOSITION_RATE = 0.1;
    const double MAX_SEDIMENT = 5.0;
    const double MAX_VEL = 6.5;

    for (int i = 0; i < steps && droplet.vol > 0.01; ++i) {
        Position next = terrain.findLowestNeighbor(droplet.pos.x, droplet.pos.y);

        if (next.x == droplet.pos.x && next.y == droplet.pos.y) {
            break; // Nowhere to go
        }

        double currentHeight = terrain.getHeight(droplet.pos.x, droplet.pos.y);
        double nextHeight = terrain.getHeight(next.x, next.y);
        double slope = currentHeight - nextHeight;

        droplet.vel = std::sqrt(std::max(0.0, droplet.vel + slope * 0.1));

        if (slope > 0.01) {
            double available = MAX_SEDIMENT - droplet.sediment; // How much can be eroded?
            double erodeAmount = std::min(EROSION_RATE * slope * droplet.vel, available);
            terrain.setHeight(droplet.pos.x, droplet.pos.y, currentHeight - erodeAmount);
            droplet.sediment += erodeAmount;
        }
        else {
            double depositAmount = droplet.sediment * DEPOSITION_RATE * (1.0 - droplet.vel / MAX_VEL);
            depositAmount = clamp(depositAmount, 0, droplet.sediment);
            terrain.setHeight(next.x, next.y, terrain.getHeight(next.x, next.y) + depositAmount);
            droplet.sediment -= depositAmount;
        }

        droplet.pos = next;
        droplet.vol *= 0.99; // Evaporation
    }
}

void simulateErosion(Terrain& terrain, int numDroplets = 10000) {
    const int RAIN_HEIGHT = 5;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, terrain.width - 1);
    std::uniform_int_distribution<> disY(0, terrain.length - 1);


    for (int i = 0; i < numDroplets; ++i) {
        int x, y;
        //do {
        //    x = disX(gen);
        //    y = disY(gen);
        //} while (terrain.getHeight(x, y) < RAIN_HEIGHT); // Only start at certain elevation
        
        x = disX(gen);
        y = disY(gen);

        Droplet droplet(x, y);

        std::cout << i;
        erodeDroplet(terrain, droplet);
    }
}

// Generate a 100x100 heightmap with values between 0 and 10
std::vector<std::vector<double>> generateHeightmap(int width = 100, int length = 100) {
    std::vector<std::vector<double>> map(length, std::vector<double>(width, 0.0));

    int centerX = width / 2;
    int centerY = length / 2;
    double radius = std::min(width, length) / 3.0; // Hill covers ~1/3 of the map

    for (int y = 0; y < length; ++y) {
        for (int x = 0; x < width; ++x) {
            // Distance from center
            double dx = x - centerX;
            double dy = y - centerY;
            double distance = std::sqrt(dx * dx + dy * dy);

            // Smooth hill: Gaussian-like falloff
            if (distance <= radius) {
                double normalized = distance / radius; // 0 at center, 1 at edge
                map[y][x] = 10 * (1.0 - normalized * normalized); // Parabolic shape
            }
            else {
                map[y][x] = 0.0; // Flat outside the hill
            }
        }
    }

    return map;
}

std::vector<std::vector<double>> generateHeightmap2(int w, int l) {
    std::vector<std::vector<double>> heightmap(l, std::vector<double>(w, 0.0));
    
    const float freq = 0.02;
    Perlin p(w, l);

    for (int y = 0; y < l; ++y) {
        for (int x = 0; x < w; ++x) {
            heightmap[y][x] = p.noise(x*freq, y*freq);
        }
    }

    return heightmap;
}

int main() {
    std::vector<std::vector<double>> map = generateHeightmap2(100,100);
    
    Terrain t(map);

    t.print("before.png");
    simulateErosion(t, 10000);
    t.print("after.png");

    return 0;
}

