Kinda Asteroids
# Goals:
Implement an ECS (Entity-Component System).
Focus on Clean Code decisions.
Focus on writing code that follows SRP (Single Responsibility Principle), and DRY (Don't Repeat Yourself).

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

