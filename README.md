Hand rolled Space invaders clone

# Goals:
Implement an ECS (Entity-Component System).
Learn about game-engine architecture.
Learn about game scheduling. (as in flow through.)
Focus on Clean Code decisions.
Focus on writing code that follows SRP (Single Responsibility Principle), and DRY (Don't Repeat Yourself).

# Credit
https://github.com/leerob/space-invaders/blob/master/fonts/space_invaders.ttf

For the space invaders font

# Issues
## Should Input update components or only emit events?
-  Both?
- The input system should both update components as well emit events. While working, it occurred to me that the input system having direct value updates to the entity components was a problem. My assumption was that the right move was to then the input system to an event emitting system only. After continuing to research the question the right answer is actually a hybrid approach where the input system will update input components, that only need to reflect state, on the entities and to leave other inputs to emit an event. 
## Understanding the Event queue's place
- While continuous state belongs in the components (moving, aiming, charging a weapon). The event queue is perfect for discrete one-off events (UI/State changes, Weapon fired event, Open Inventory Event).
## Coordinate Space issue:
1. I would like my system to be independent of resolution. While it being the "correct" thing to do, I also work on this using my laptop and my monitor.
	- SDL3 handles all of the window creation for you, however it is not acting quite as robustly out of the box as I was hoping. So I'm going to investigate how I can configure windows in SDL3.
	- It looks like SDL3 supports a Logical representation of space, but this doesn't supercede getWindowSize() calls. I think this is the key I'm missing. The SDL_SetRenderLogicalPresentation has options for setting the logical width, height, presentation style. It used to support SCALEMODE, this doesn't look like it's in the api the same way anymore, so. Looking into it it seems like SDL3 will automatically set the scale for textures to linear, it CAN be changed globally still, but it was removed from that part.
## Ray casting
- There are couple of Ray casting algorithms that come up quickly. DDA or Digital Differential Analysis, Bresenham's Algorithm. However, both of these algorithms require a grid based representation of the plane. A grid based system I don't have on this project. So instead of either of these options, I'm going with Slab method:

> [!quote] Wikipedia 
>In computer graphics, the slab method is an algorithm used to solve the ray-box intersection problem in case of an axis-aligned bounding box (AABB), i.e. to determine the intersection points between a ray and the box. Due to its efficient nature, that can allow for a branch-free implementation, it is widely used in computer graphics applications.
## Misstep with entity typing.

>[!error] Problem
> I made an oopsie with regards to the entities. I created type components that defined the kinds there were. At first this doesn't seem like such an issue, however the true cost of this mistake came in the systems that reads the entities (which is all of them*). It caused a problem where $N \times N$ collision types had to be accounted for in the Collision System. 
> It had the second issue of coupling gameplay logic with the Collision System. This violated the single responsibility principle that I was trying to follow. The issue being that as the project grows to contain more and more of the gameplay that the Collision System goes from something that only focuses on the telling us whether or not there was a collision, to something that was also defining gameplay. It also meant that as I implemented more features, the case where I noticed this flaw was as I was adding enemies.
- 
# The movement system was unfortunately too rigid and couldn't be decoupled from game logic because of the much earlier decision to use an event driven system. 
1. The modern entity component system tends to prefer intent and response components from what I can tell about the general landscape of game development.
1. I wasn't quite aware of that when I was building the architecture. I instead jumped into what felt the most natural which was an event emitting system, with an event processor. 
  1. This caused a few issues later on as creating robust handlers that could account for all possible actions or events.

# The scoring system.
1. Event Driven actions has caused another issue, scoring.
  - It can be done two ways:
  1. The first way is bullet emits a score event, then the event processor (or separate scoring processor) handles the scoring updates.

# Space Invaders Movement
- Space invaders originally moved one alien per frame, at 60 frames a second they begin very slow. Playing the game killed off the aliens and as each alien was killed off the cpu was freed up few more cycles. Cycling through the aliens took shorter and shorter amounts of time until the last litle guy was practically sprinting across the monitor.
- I will get to this issue when I get to it. I have more pressing issues to figure out however. Firstly I need to get the game loop going.

# Gameplay
## Not completed
- I need to add the little green barriers.

## Completed
- I need to have random alien spaceship go by.
  - I created a system that checks manages the mystery ships. 
  - I decided to go with atari 2600 style of ufo behavior.
- I need to have it loop when the last invader is killed.
  - There is a system that monitors the number of formation aliens still alive in a round. Once it detects a round has ended it calls the renderer to clean up the animations. Pauses the game for a moment and then spawns in a new round of aliens. 
    - It doesn't have an issue where it creates a separate instance of entity factory. This means that the rounds don't keep descending, but more importantly it's a waste of resources.
- The invaders move across the screen properly and descend (slightly incorrectly).
- I need to add the explosion animation
- The score is on the screen and updates.
- I need to add the little green line at the bottom.
  - Added the green line, by drawing the bottom wall entity in green. This is probably a little jank, but at the same time, I feel as though it's fine.
- I need to add the player lives at the side.
  - I could probably do this a couple of different ways. I could add the texture directly in the render system and run it by the number of lives. I don't like this idea though because the render system should in theory should only draw entities and UI components.
    - Instead, I will be creating a texture with the number of sprites equal to the lives left. When a life is lost or added, an update flag is set and the texture recreated.
    - Players lives are being drawn
    - I need to still add the actual logic for losing a life, but theoretically the texture should update.

# Aliens shooting at the player
- The aliens have an attack besides inching downwards toward the player, they can shoot!
- Depending on which space invaders version you are playing, the rules for what aliens and when they can shoot differ. The one that I liked was that only the bottom row of any column can shoot. This means that the alien on the bottom has to know there is nothing blocking their shot.
  - These projectiles are only every shot from the last alien in a column, to achieve this you have to understand what the aliens need to know.
  1. Aliens need to be able to see what's below them (Raycasting).
  1. Aliens need to know their place in line and know the status of all the aliens around them (some convoluted tracker or algorithm).
  - I had already built a raycasting system that should act as a perfect drop in system that could be used!
  - I was able to apply my raycasting system to the aliens, giving them vision and by proxy a very rudimentary AI!

# Example [:^1]
## The "Firing a Weapon" Pipeline
1. The Input System (The Messenger)
  Your Input System reads the mouse/keyboard or controller. It finds the player entity and sets isFiring = true on their PlayerControllerComponent.
  Crucial detail: The Input System does not emit the WeaponFiredEvent. It simply updates the state.
2. The Combat/Weapon System (The Brains)
  You have a dedicated system (e.g., WeaponSystem) that iterates over entities with both a PlayerControllerComponent and a WeaponComponent (which holds data like ammo, fireRateCooldown, projectileType).
  It checks: Is isFiring true? If yes, is the cooldown timer at zero? Do we have ammo?
  If all conditions are met, this system handles the logic.
  It subtracts ammo, resets the cooldown timer, and directly spawns the bullet entity.
  Why not an event for spawning? In ECS, creating an entity is just adding data to arrays. The WeaponSystem has the context (barrel position, aim direction) to just create the projectile right there. Routing that through an event queue adds unnecessary overhead for a core gameplay mechanic.
3. Emitting the Event (The Broadcast)
  Immediately after spawning the bullet, the WeaponSystem emits the WeaponFiredEvent.
  4. Event Processing (The "Juice")
  Now your event queue shines. The WeaponFiredEvent is picked up by secondary systems that don't need to know the heavy logic of how a gun works, but need to react to the fact that it fired:
  Audio System: Hears the event, plays a "bang" sound at the player's location.
  Particle System: Hears the event, spawns a muzzle flash entity.
  Camera System: Hears the event, applies a small screen shake.
5. The Animation System (The Visual State)
  You mentioned the renderer showing the state. To keep things decoupled, your RenderSystem should be incredibly "dumb"—it should only care about drawing shapes, sprites, or meshes to the screen.
  Instead, an Animation System runs before the Renderer. It looks at the PlayerControllerComponent (isFiring == true) and updates the player's SpriteComponent or MeshComponent to the "shooting" frame. The Renderer then just blindly draws whatever the Sprite/Mesh component tells it to.


# Footnotes:
[:^1] This example was made by Google Gemini 3.1 on 2026-04-23.

