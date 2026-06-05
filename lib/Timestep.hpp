#pragma once
#include <cstdint>

uint64_t SDL_GetTicksNS();

//--------------TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR---------------------
struct TimeStep {
  // TODO: I would like to consider using the chrono library instead, but that
  // is a later problem
  // target_deltatime_nanoseconds represents approximately 1/60th of second.
  static constexpr uint64_t target_deltatime_nanoseconds = 1000000000ULL / 60;
  static constexpr uint64_t max_allowable_frame_time = 250000000ULL;
  // accumulator is used to keep the process from spiraling.
  uint64_t accumulator = 0;
  // this is a cursor or time stamp of the previous call to tick()
  uint64_t last_time = 0;
  // Updates the accumulator and last time based on the elapsed time from the
  // start of the program.
  void Tick() {
    uint64_t current_time = SDL_GetTicksNS();
    uint64_t frame_time = current_time - last_time;

    const bool is_frame_time_greater_than_allowed = frame_time > max_allowable_frame_time;

    frame_time = is_frame_time_greater_than_allowed ? max_allowable_frame_time : frame_time;

    last_time = current_time;

    accumulator += frame_time;
  }
  // INFO: consumeStep is what allows for "substeps" These are steps that occur
  // within one frame. It is unlikely to ever have more than one substep, but it
  // is a safety precaution.
  bool consumeStep() {
    if (accumulator >= target_deltatime_nanoseconds) {
      accumulator -= target_deltatime_nanoseconds;
      return true;
    }
    return false;
  }
  float GetAlpha() { return (float)accumulator / (float)target_deltatime_nanoseconds; }
  static float GetCurrentTime() { return SDL_GetTicksNS(); }
};
//=============TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR======================
