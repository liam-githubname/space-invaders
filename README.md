# What I learned building "Not Space Invaders"

*A timeline of every good and bad decision I made on the way to a Space Invaders clone.*

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/5bac7bce-3955-480e-9508-9b8aedff4e9a" />


---
I wanted to work on a project that was going to help me learn more than just a language or a library. I wanted to learn about how implementing clean code decisions would ease the comprehension debt that occurs as a project scales up.

I had a few goals for this project: 
> - Implement an ECS (Entity-Component System). : I accidentally implemented a Composition system instead. More on that later!
> - Learn about game architecture. : Absolutely learned a lot about game architecture!
> - Focus on Clean Code decisions, SRP (Single Responsibility Principle), and DRY (Don't Repeat Yourself). : I did my best to build good clean code into the code base!
> - Dust off my low-level programming skills. : Got magnitudes better at knowing what the compiler wants.

About halfway through I decided that I was going to write up the timeline of the project, firstly so I didn't forget all those good lessons I learned along the way, and secondly as a better way to understand what code decisions I was making, and why I was making them. ~8 weeks, 46 commits, one C++/SDL3 project, and a lot of commits titled things like *"I think I fixed the movement actually."*

### Thoughts on AI.
I have very strict personal rules for LLM usage. 
- Write all code by hand.
- Never off-load thinking onto an LLM. 
- Look for the bug yourself first.
- Write the problem down on paper first.
What this means in practice: as a ***naive*** sound board for architectural decisions, as a natural language documentation parser, or as way to understand pitfalls of low-level programming.
---
## Credit
The arcade font is `space_invaders.ttf` from [leerob/space-invaders](https://github.com/leerob/space-invaders/blob/master/fonts/space_invaders.ttf).
The sprites are from https://www.spriters-resource.com/arcade/spaceinv/.

---

## Phase 0 — Pre-scaffolding (`3e974fc` → `4744750`, Apr 9)

> **Commits:** *"changing file structure"* → *"added more targets"* → *"added a module, ran into bug"*

I was started from basically zero, a few years ago I had taken a perusal through SDL2's window creation but hadn't done anything meaningful with it. This meant I had to learn how SDL liked to do things! I also had to learn the library's handles.

I started with everything in `main()`. Window creation, the SDL RAII wrapper, the timestep struct. Then created the first of the library systems, the GraphicsModule.

> *"In C++, when an object is moved (using the default move constructor), its smart pointers are transferred to the new object, and the temporary object's pointers are set to `nullptr`. If we unconditionally call `SDL_QuitSubSystem` in the destructor, the destruction of the temporary object will prematurely shut down the entire SDL Video Subsystem, destroying our window instantly!"*

This bug was pretty nasty, and a result of my pretty lacking understanding of how the clang optimizes things.

Because work needed to be done on the SDL structures that the GraphicsModule wrapped, I had to create an initializer. This returns a call to a constructor, which creates a temporary unnamed value that holds pointers for the SDL structures. `C++` compiler sees that we are returning an unnamed `GraphicsModule` constructor, so it decides to 'move' the contents of the object instead of copying them. When the unnamed GraphicsModule that had it's contents moved, falls out of scope the destructor is called. This is where the bug was. The destructor killed off the SDL systems no matter what. The fix? Add a simple guard clause to make sure the pointer's were filled.


---

## Phase 1 — The Entity System (`811e18c` → `0d02086`, Apr 15)

> **Commits:** *"Began developing a toy Entity Component System"* → *"added input system"* → *"added gitignore"* → *"removed cmake build files"*

I wanted to get familiar with modern game architecture, so I decided to try to implement an ECS.

```cpp
struct Velocity  { float dx; float dy; };
struct Transform { float x;  float y;  };
struct Sprite    { void *texture; float width, height; };
struct Entity    { uint32_t id; bool is_active;
                    std::optional<Velocity>  velocity;
                    std::optional<Transform> transform;
                    std::optional<Sprite>    sprite; };
```

A `GameState` class that owned a `std::vector<Entity>`. An `InputSystem` that wrote to a `PlayerInput` struct every frame. And a `.gitignore`, because I was committing my build files.

I did not yet realize the architectural question this commit was going to ask me: **ECS wants you to iterate *components*, not entities.** I was thinking in terms of *"the player"*, *"the walls"*, *"the bullet"*, and I didn't realize the mistake until a refactor would be unrealistic. The truth is that this mistake let me learn a lot of modern `C++` idiosyncrasies that I wouldn't have seen otherwise, if I had to find a silver lining!

==I flipped the ECS==, This meant that I was iterating through a central vector of entities and looking for components attached to it. This is a far more Object Oriented approach, which didn't give me the benefit of cache coherency. 

---

## Phase 2 — First three systems + cleanup (`a55c7fc` → `47370a8`, Apr 20–21)

> **Commits:** *"created Movement System, Render System, Collision System"* → *"My code styling was all over the place, updated it to snake_case and PascalCase for types"* → *"STATE: running, entity-component system, with AABB collision detection (SAT actually)"* [^2]
 
The first thing I did was iterate over the entities and update state components with intent components. Which is a fancy way of saying I had the positions of entities get updated with their velocity. I then wrote some skeleton code for a collision detection system. Then finally I added a very small skeleton rendering system that only drew things to screen for testing.

It was at this point where the project was straddling four or five files, and things we're starting to include each other. I had to make some decisions about making reproducible readability. I had heard adage somewhere "it doesn't matter what style guidelines you choose, only that you choose one" (I want to say Fireship or CodeAesthetic said that one).
- PascalCase for class methods
- snake_case for locals with trailing underscore on private members

The third was the first *visible thing* the project ever produced: a window with two rectangles that I could slide into each other and a console log telling me they were colliding.

[^2]: **Slab method detection not SAT**: for some reason I thought it was SAT, it's not.
# Add the gif of the collision detection working

---

## Phase 3 — The event system arrives (`5b3db0e` → `31bee41`, Apr 22–25)

> **Commits:** *"created an event system to maintain SRP, (using god event processor tho)"* → *"New: Event type header, EventQueue header"* → *"Linker Error I didn't catch"* → *"centered entity transforms in logic, fixed collision logic bugs, updated EventSystem processes, Input system supports multidirectional input"*

 I realized that if the collision system was supposed to know about types of collisions that a "robust" solution would have to have logic for all possible interactions even if they shouldn't happen. My discrete structures, told me that this was a combinatorics problem I didn't want to solve.
 The `CollisionSystem` had to know about every *kind* of entity, and every kind of collision between them, and what to do about each. This was an SRP violation. I decided to separate out detection from reaction: `CollisionSystem` would just push a `CollisionPayload` to an event queue, and a `ProcessEvents` function would visit the payloads and call the right handler. **What I didn't realize at the time, was an Event system combined with what I already had was sort of an antipattern**.

---

## Phase 4 — Raycasting + shooting (`79d8ff9` → `476165d`, Apr 25–27)

> **Commits:** *"The demo of the collision and world borders is fully functional now"* → *"Added Raycast algorithms for AABB colliders and Circle colliders, started ShootingSystem"* → *"ShootingSystem is pushing events into the event queue. There is a serious amount of extra work piling up."*

### Why slab method, not DDA or Bresenham

The two raycasting algorithms that come up first in any search are **DDA (Digital Differential Analysis)** and **Bresenham's line algorithm**. Both of them assume a **grid-based representation of the plane**. I don't have a grid. I have floating-point positions, integer pixel sizes, and a small set of axis-aligned colliders. So I went with the slab method:

> *In computer graphics, the slab method is an algorithm used to solve the ray-box intersection problem in case of an axis-aligned bounding box (AABB), i.e. to determine the intersection points between a ray and the box. Due to its efficient nature, that can allow for a branch-free implementation, it is widely used in computer graphics applications.* — Wikipedia

The slab method computes the "slab" — the interval on the ray's `t` parameter where the ray is inside the AABB on each axis — and intersects the two axis intervals. No grid, no integer stepping, just floating-point math. It is also the *easiest* of the three to read, once you stop trying to be clever about the degenerate case where the ray runs parallel to an axis.
<img width="800" alt="Screen Recording 2026-04-21 at 12 55 12 PM" src="https://github.com/user-attachments/assets/1ce7869d-5778-46b5-9c98-e933f069c0df" />

### Implementing raycasting
By the time I was implementing raycasting, I hadn't quite decided what I was going to do with it. I was building space invaders (or asteroids, I kind of hadn't decided yet). Which I couldn't tell a direct usage for yet, but I knew I wanted to write raycasting myself. It's the cornerstone of video game ai and computer graphics. All I knew is that I had to write one. This ended up being one of the best early decision I made as I eventually had a problem where I needed the aliens to able to see what was in front of them.
<img width="800" height="885" alt="Screen Recording 2026-06-02 at 4 13 27 PM" src="https://github.com/user-attachments/assets/b5a7751f-049f-4552-87e6-3bf87e122419" />

---

## Phase 5 — The bitmask refactor (`5372f65` → `907a006`, May 12–17)

> **Commits:** *"removed the collision typing system because it violates srp and like also was on the way to being spaghetti code"* → *"Removed the entity-type components — Created Bitmask layer system instead"* → *"Moved Timestep to it's own header file — Updated systems to use bitmask system — working on eventSystem"* → *"Using a C++ std::find_if search tool to learn more about the language in input system, added a new paradigm to event event handler"* → *"fixed bug in raycast.hpp"* → *"reducing debug logs"*

This is the phase the article is about, honestly.

I had built my entities with type components: `IsPlayer {}`, `IsWall {}`, `IsAlien {}` and an enum called `CollisionType` with values like `PlayerAndWall`,  `EnemyAndWall`, `PlayerAndEnemy`. When I wrote the PlayerAndEnemy type, I realized something was wrong.  Every time I added a new entity type, I had to add a new enum value and a new handler. The N×N explosion was very obviously a symptom of some earlier problem I had made. 
### Misstep with entity typing

> I made an oopsie with regards to the entities. I created type components that defined the kinds there were. At first this doesn't seem like such an issue, however the true cost of this mistake came in the systems that reads the entities (which is all of them). It caused a problem where N×N collision types had to be accounted for in the Collision System.
> It had the second issue of coupling gameplay logic with the Collision System. This violated the single responsibility principle that I was trying to follow. The issue being that as the project grows to contain more and more of the gameplay that the Collision System goes from something that only focuses on telling us whether or not there was a collision, to something that was also defining gameplay. It also meant that as I implemented more features, the case where I noticed this flaw was as I was adding enemies.

The fix: a `Bitmask { layer; mask; }` struct and a `GameLayer` enum (`Player = 1<<0`, `Wall = 1<<1`, `Enemy = 1<<2`, `Projectile = 1<<3`, `All = 0xFFFFFFF`). Every entity has a layer; every system declares the mask of layers it cares about. The collision routing collapses to a single `if ((a->bitmask->layer & b->bitmask->mask) && (b->bitmask->layer & a->bitmask->mask))`.

`5372f65`  removed entity typing entirely and a collision type enum. **The lines I removed were a class of bugs I no longer have to think about.** I also got to implement a bitmask by hand for the first time. On an older project I had discovered layers in Unity, not only where they useful, but bitmasks themselves feel like low-level magic. Which is one of the reasons I got into programming in the first place, how cool the little tricks we're for hiding big information in tiny formats. 

`Timestep` also got its own header file in this phase, which is boring and important: a fixed-step accumulator with a 250ms spiral-of-death cap, exposing a `consumeStep()` API.

I also added a `std::find_if` probe to the input system to learn what the standard algorithm looks like, then started using `Overload` (a cppreference pattern) to do type-safe dispatch on the `std::variant<CollisionPayload, DeathPayload, ScorePayload, HitPayload>` in the event queue — no virtual function, no `dynamic_cast`.

I think I also introduced the very common Vec2 struct in these commits, but I didn't refactor the codebase to use it until later.

---

## Phase 6 — The engine vs. game split, and the realization (`2411d95` → `7cada2e`, May 17–20)

> **Commits:** *"Finally getting around to game logic, added game subdir to store game specific logic"* → *"I have realized that the ECS makes it hard to create a toy engine, I also realized the event driven system I made makes decoupling hard to do."* → *"Removing the game specific movement system, using the movement_system instead"*

Here is where I realized the *anti-pattern* I was talking about earlier existed.

> *"I have realized that the ECS makes it hard to create a toy engine, I also realized the event driven system I made makes decoupling hard to do."* — `d71ad8b`

What happened: I created a `game/` subdirectory to hold Space Invaders–specific code (`Game.cpp`, `EntityFactory.cpp`, `GameRules.cpp`, `PlayerControlSystem.cpp`). The first instinct was to *also* fork the `MovementSystem` from `lib/` into a `SpaceInvadersMovementSystem` in `game/`, because the alien step tempo didn't fit the generic one. The fork lasted **less than two days** before I deleted it.

I was trying to build a reusable engine, but every "engine" decision I made was actually a Space Invaders rule wearing a trenchcoat. Here I had to make a decision about whether or not to try and fully decouple game logic from the library systems. 
- One, by now I knew that the backwards way I tried to implement ECS meant that I already couldn't operate over components separately, so component interactions had to be backed in the entire systems that had to iterate every entity to find the proper components to work on.
- Every new kind of system was at least a new $O(n)$ loop.

### Why the movement system was too rigid

> The movement system was unfortunately too rigid and couldn't be decoupled from game logic because of the much earlier decision to use an event driven system. The modern entity component system tends to prefer intent and response components from what I can tell about the general landscape of game development. I wasn't quite aware of that when I was building the architecture. I instead jumped into what felt the most natural which was an event emitting system, with an event processor. This caused a few issues later on as creating robust handlers that could account for all possible actions or events.

The fix was to introduce an `AlterMovement` *intent component* — the `EventSystem` writes the intent onto the entity, the `MovementSystem` reads it on the next pass and applies it. The generic engine owns the *mechanism* (advance, integrate, clamp); the game layer owns the *policy* (when, how far, in what direction). That's the right separation. The event system was the wrong tool for that job; the intent component is the right one.

The engine-vs-game split survived the moment of doubt, and the duplicate MovementSystem did not. **Sometimes the right move after a refactor is the one that deletes the new file.**

---

## Phase 7 — Vec2, the movement bug hunt, and the title arc (`6c7e1d1` → `4157c1b`, May 21–23)

> **Commits:** *"added a 2d vector struct, updated everything that should use it, trying to fix movement still"* → *"saving state, making alien formations, trying to figure out a couple of things"* → *"fixed alien movement bug, I found and fixed the wall interaction bug"* → *"added operator overloads for Vec2, I think I fixed the movement actually"* → *"I've got the basics of space-invaders working"*

I introduced a `Vec2` struct to replace the `transform->x`/`transform->y`/`velocity->dx`/`velocity->dy` quadruple across every system. 

The commit titles tell the whole arc:

> *"trying to fix movement still"* → *"trying to figure out a couple of things"* → *"I think I fixed the movement actually"* → *"I've got the basics of space-invaders working"*

Each title is a little less defeated.

# Add gif of the formation without the sprites

---

## Phase 8 — UI, infinite rounds, and aliens with vision (`f4345f6` → `d0c57f3`, May 27–30)

> **Commits:** *"Added a bit of ui and fixed sprites"* → *"Gotten UI working, Rendering textures dynamically for all of it"* → *"Game now loops through rounds of aliens infinitely, and animations are set by components"* → *"Aliens have vision now using raycasting. They can tell whether an alien is underneath them and know when they should fire."*

Three systems landed in this phase: `UISystem` (with a font loader and dynamic texture baking for the score and lives counters), the `RoundSystem` (which watches the formation die out, pauses for a moment, and spawns a new one), and the **alien vision AI** built on the `Raycast` module I shipped two weeks earlier.

### Aliens can shoot and they can see

The original Space Invaders had aliens that march downward and shoot. The version I wanted to build has aliens that march downward, *and* can shoot from only the bottom alien in each column. To do that, the bottom alien has to know whether anything is between it and the player.

> *"I had already built a raycasting system that should act as a perfect drop in system that could be used! I was able to apply my raycasting system to the aliens, giving them vision and by proxy a very rudimentary AI!"*

`d0c57f3` is the single commit in this project I'm most proud of, and it's the smallest. The `Raycast` module didn't change. The aliens didn't need to know they were being "seen" by a math primitive. The only thing I had to do was write a caller to the raycast module that grabbed the right information.

###  Mystery Ship System

Because I went to tackle the ufo's first. I wrote a system called the `MysteryShipSystem`, I quickly realized after implementing some more game logic that it should just be considered a part of a more encompassing RoundSystem. Which is another semi-god-file that handles all round logic for aliens and the player.

**By now the largest files are around 230-240 lines. I think they are just on the edge of being too big for my tastes.**

### Leaving behind the `ScorePayload`
At this point I had abandoned the idea of expanding the event system. The score was going to be updated if there was a score component attached to the entity during an event that might score i.e. a bullet hitting something. I knew that since this was the only way score could be increased, it wasn't really coupling another systems logic it was merely programmatically applying the proper effects. 
- One of the reasons I was reluctant to remove the Event system entirely was because of the useful `C++` lesson it taught me. An `event` is a type safe anonymous union called a `std::variant`. It held a single instance of Collision, Hit, Death, or ScorePayload at run time. Then using a call to `std::visit` you pass it a single callable. This is where the magic comes in: the `Overload()` helper derives all of the function operators, so when a call is made to it, `Overload()` tries to match the passed parameter to each of the () operators. **The main value of this, is in it's exhaustiveness**, because the compiler has to know what each path through the visit must do, it must be defined otherwise it will error. The other benefit, is because event is a variant the compiler can also tell you where you might be failing to meet this exhaustiveness.

---

## Phase 9 — Health, menus, and the magic-number cleanup (`bea20a8` → `82ad785`, Jun 1)

> **Commits:** *"Big refactor to EventSystem, added a health component to generalize bullet collision interactions"* → *"Start menu and Game over screens"* → *"small fixes"* → *"Removed magic numbers that I had left around the place"*

The final phase is the cleanup. Three things:

**1. The `Health` component generalizes bullet collision.** Before this commit, the bullet-collision code had to know that bullets are projectiles and projectiles collide with anything that takes damage. After this commit, anything with a `health.has_value()` takes damage from anything else with a `health.has_value()`. The player, the aliens, the mystery ship, and the barriers all get `Health`, and the same handler decrements it for all of them. The bullet doesn't know what it hit; the target doesn't know what hit it. They just meet in the middle. This is the same trick that made the raycast drop-in possible, but in the other direction: *drop the component in* instead of *drop the system in*.

**2. Start menu and game over screens.** The start menu and game over screens are nothing beautiful, but I am not a very good artists so it's going to have to be good enough.

**3. Removed magic numbers that I had left around the place.** The final commit introduces `lib/GameConfig.hpp`. Every literal in the codebase is now a name. I had been leaving myself notes to clean up the magic numbers when I was realizing I was putting them down, but as it turns out, I had a limited understanding of what a magic number was. 
<img width="1000" alt="image" src="https://github.com/user-attachments/assets/deb6e392-c198-458d-9c71-54df2e48d414" />


### A note on the alien march tempo

The original game's idiosyncratic movement is defined entirely by the hardware it was made on. It's actually a very interesting story that I didn't know about before starting this. Unfortunately creating a way to emulate the logic behind the specific movement, wasn't the goal of this project. 
The original arcade moved one alien per frame; at 60 frames a second, the formation starts glacially slow and accelerates as aliens die, freeing CPU. My implementation is different: `MovementSystem` gates the step on a timer, and the timer scales with `total - alive`. It's *not* faithful to the original, but it has the same arcade-feel: the last alien sprints across the screen.

---

## What I got wrong (the honest list)

If I were a reviewer reading this article cold, here is the list of things I would want me to own up to:

- **The event system should be deprecated.** Despite the bitmask refactor, the `EventSystem::ProcessEvents` dispatcher still calls both `WallCollisionHandler` and `BulletCollisionHandler` on every payload, and a `HitPayload` is emitted by the shooting system and *never consumed* (its handler is a commented-out log line). The architecture I am most proud of is also the one I most want to keep working on.
- **RenderSystem is doing logic.** It ticks the alien death animation and *destroys* the entity when the timer expires. The render pass should not be the place where entities go away. (I left a `// Render the system shouldn't know about entities.` comment in the source so I would not forget.)
- **InputSystem is doing both player input and alien random-fire seeding.** It should be two systems.
- **The barrier deterioration isn't implemented.** They have a health component and they get removed like all other entities, but I have no animations set up for it.
- **The UI does not scale with resolution.** The menu and game-over screens hard-code `2.0f` scale and `window_height_ / 3.0f` positioning, which works at 256×224 and breaks at any other size. The fix is straightforward — bake the menu textures at the right size and use SDL's logical presentation properly — but it isn't done.
- **I said "SAT" in three commit messages. It is AABB.** I'm correcting it here.

---

## What I got right (the parts the principles paid off on)

The list of things I am happy with is shorter than the list of things I am not, which is the right ratio for a portfolio:

- **The early SRP/DRY commitment paid for itself in Phase 5.** The `Entity` struct from `811e18c` (Phase 1) was the same shape I refactored 4 weeks later in `74ffd0a`. The cost of getting it roughly right early was small; the cost of retrofitting would have been enormous.
- **The bitmask layer is small, well-named, and a one-line change to extend.** Adding a new entity type is a constructor edit, not a switch statement.
- **The raycast module is a drop-in primitive.** Used for player bullets, then for alien line-of-sight, with no changes to the module itself. The best architectural decisions are the ones that pay off without anyone trying.
- **The `Health` component generalizes a class of interaction.** Player, aliens, mystery ship, and barriers all use the same `--hp` code path. 
- **`Timestep` is small and boring and does one thing.**
- **The naming commit (snake_case + PascalCase).** I do not need to think about it now.

## Closing

A portfolio project is supposed to be the thing that shows what you can do. This shows that I can take a jump right into problem space full of unknown unknowns and start making some of those unknowns known!

If you're reading this as a grad school admissions committee: I would like to learn how to be a good programmer, and I would love to do that by learning how to make good videogames!

If you're reading this as a developer: the most useful thing in this article is the bitmask refactor. The most useful thing in the codebase is `lib/Raycast.hpp`. The most useful thing in the commit log is `d71ad8b`.

Thanks for reading.

---

[^1]: This "Firing a Weapon" pipeline was drafted with the help of Google Gemini on 2026-04-23; I edited the version that lives here to match what my own code actually does. The shape is the gem; the specifics are mine.

[^2]: I did not implement SAT or Separate Axis Theorem, idk why I thought I did.
