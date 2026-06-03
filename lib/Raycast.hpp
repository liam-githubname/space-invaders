/* Raycast.hpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. I have spent so long deciding to implement Slab method raycasting.
// It also took a look time to figure out.
// TODO:=======================================================================
// 4. I want to update the names to reflect the fact that they return the
// distance of the hit. But I can't think of any names right now.
// ============================================================================
#pragma once

#include "Util.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cmath>
#include <optional>

namespace Raycast {
struct Ray {
  Vec2 origin;
  Vec2 direction;
};

// 4. I want to update the names to reflect their return value.
inline std::optional<float> RayAgainstAABB(Ray ray, float entity_min_x, float entity_min_y, float entity_max_x,
                                           float entity_max_y) {
  // SDL_Log("In RayAgainstAABB");
  // 1. Get the minimum and maximum coordinates for the rectangle entity.
  // Passed to the function.
  //    note: the minimum and maximum coordinates don't correspond inherently to
  //    the min and max time values. To get the floor and ceiling time you have
  //    to sort the items.
  // 2. Cast the Ray and find where ray crosses into the slab.
  // 3. Calculate the slabs.
  if (ray.direction.x == 0 && ray.direction.y == 0)
    return std::nullopt;

  // Use the Euclidean norm to transform the direction into a standard
  // comparable format. This lets me check the actual distance in game.
  float magnitude = std::hypot(ray.direction.x, ray.direction.y);
  ray.direction.x = ray.direction.x / magnitude;
  ray.direction.y = ray.direction.y / magnitude;

  // Calculate the inverse so we don't have to divide by 0.0.
  float inverse_direction_x = 1.0f / ray.direction.x;
  float inverse_direction_y = 1.0f / ray.direction.y;

  float time_x_1 = (entity_min_x - ray.origin.x) * inverse_direction_x;
  float time_x_2 = (entity_max_x - ray.origin.x) * inverse_direction_x;
  float time_x_floor = std::min(time_x_1, time_x_2);
  float time_x_ceiling = std::max(time_x_1, time_x_2);

  float time_y_1 = (entity_min_y - ray.origin.y) * inverse_direction_y;
  float time_y_2 = (entity_max_y - ray.origin.y) * inverse_direction_y;
  float time_y_floor = std::min(time_y_1, time_y_2);
  float time_y_ceiling = std::max(time_y_1, time_y_2);

  // 4. Combine the slabs.
  float latest_time_floor = std::max(time_x_floor, time_y_floor);
  float earliest_time_ceiling = std::min(time_x_ceiling, time_y_ceiling);

  // SDL_Log("%f, %f", latest_time_floor, earliest_time_ceiling);

  // 5. Check if ray is inside slab intersection.
  return (latest_time_floor <= earliest_time_ceiling && earliest_time_ceiling >= 0)
           ? std::optional<float>(latest_time_floor)
           : std::nullopt;
}

// Apply the same fixes to RayAgainstCircle, currently could divide by zero on
// axis aligned raycasting.
inline std::optional<float> RayAgainstCircle(Ray ray, float center_x, float center_y, float radius) {
  float magnitude = std::hypot(ray.direction.x, ray.direction.y);
  ray.direction.x = ray.direction.x / magnitude;
  ray.direction.y = ray.direction.y / magnitude;

  float a = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y;

  float b = 2 * (ray.direction.x * (ray.origin.x - center_x) + ray.direction.y * (ray.origin.y - center_y));
  float c = (ray.origin.x - center_x) * (ray.origin.x - center_x) +
            (ray.origin.y - center_y) * (ray.origin.y - center_y) - radius * radius;

  float d = (b * b) - (4 * a * c);

  if (d == 0) {
    // evaluation order whoopsies
    return -b / (2.0f * a);
  }

  if (d > 0) {
    float t1 = (-b + std::sqrt(d)) / (2.0f * a);
    float t2 = (-b - std::sqrt(d)) / (2.0f * a);

    return std::min(t1, t2);
  }

  return std::nullopt;
}

}; // namespace Raycast
