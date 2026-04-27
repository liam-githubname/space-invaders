/* Raycast.hpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. I have spent so long deciding to implement Slab method raycasting.
// It also took a look time to figure out.
// 2. I have to implement raycast for circle colliders as well.
// TODO:=======================================================================
// 1. Create a raycast that can be created and used in other library files.
// 2. Raycast should also probably have an inline
// 3. I have to create the circle collider flow.
// 4. I want to update the names to reflect the fact that they return the
// distance of the hit. But I can't think of any names right now.
// 5. I NEED TO HAVE NORMALIZED DIRECTION.
// ============================================================================
#pragma once

#include <algorithm>
#include <cmath>
#include <optional>

namespace Raycast {
struct Ray {
  float origin_x, origin_y;
  float direction_x, direction_y;
};

inline std::optional<float> RayAgainstAABB(Ray ray, float entity_min_x,
                                           float entity_min_y,
                                           float entity_max_x,
                                           float entity_max_y) {
  // 1. Get the minimum and maximum coordinates for the rectangle entity.
  // Passed to the function.
  // 2. Cast the Ray and find where ray crosses into the slab.
  // 3. Calculate the slabs.
  float time_x_min = (entity_min_x - ray.origin_x) / ray.direction_x;
  float time_x_max = (entity_max_x - ray.origin_x) / ray.direction_x;

  float time_y_min = (entity_min_y - ray.origin_y) / ray.direction_y;
  float time_y_max = (entity_max_y - ray.origin_y) / ray.direction_y;

  // 4. Combine the slabs.
  float latest_time_enter = std::max(time_x_min, time_y_min);
  float earliest_time_exit = std::min(time_x_max, time_y_max);
  // 5. Check if ray is inside slab intersection.
  return (latest_time_enter <= earliest_time_exit && earliest_time_exit >= 0)
             ? std::optional<float>(latest_time_enter)
             : std::nullopt;
}

inline std::optional<float> RayAgainstCircle(Ray ray, float center_x,
                                             float center_y, float radius) {
  float a =
      ray.direction_x * ray.direction_x + ray.direction_y * ray.direction_y;

  float b = 2 * (ray.direction_x * (ray.origin_x - center_x) +
                 ray.direction_y * (ray.origin_y - center_y));
  float c = (ray.origin_x - center_x) * (ray.origin_x - center_x) +
            (ray.origin_y - center_y) * (ray.origin_y - center_y) -
            radius * radius;

  float d = b * b - 4 * a * c;

  if (d == 0) {
    return -b / 2 * a;
  }

  if (d > 0) {
    float t1 = (-b + std::sqrt(d)) / 2 * (a);
    float t2 = (-b - std::sqrt(d)) / 2 * (a);

    return std::max(t1, t2);
  }

  return std::nullopt;
}

}; // namespace Raycast
