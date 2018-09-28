# Snake - Prototype
A simple, extendable Snake Game for Windows, developed in C++ and Win32API.

# Type
Prototype : Some features are still in development, and certain test buttons are provided for the same.
The main task of the progam is to poll and keep doing a task until some interrupt is recieved.

## Description
A remake of the classic snake game in C++, using functions from Win32API to define a primitive user interface. The game also features a new concept of poweups, which allows one to bypass the difficulties encountered. The game is extendable, in the sense that more levels can be added to it anytime by adding a .map file.

## Rules
The main objective is to eat as much cherries as possible. When the number of cherries reaches a particular count, the snake 
is advanced to the next level. This process continues till the number of levels defined (currently 3). The player also needs to make
sure the snake stays alive. The snake dies, when : 
- It attempts to bite a part of itself off,
- It collides with an wall or boundary made of metal, wood, glass(not in all cases) or,
- It tries to move in a direction it cannot after the game is paused.

## Interface
- The current level, along with the number of cherries consumed, is displayed in the Console Window's title. 
- The snake moves on its own, but to change direction, the player needs to use the direction keys.
- To pause the game, press 'p' anytime.

## Powerups
There are 8 powerups available for use in the roster below the map. Currently, the game is a prototype program and so, powerup 
spawning has been disabled. To enable a powerup, simply click on its radio button. You can only use a single powerup at a time.
Some powerps have immediate effect, some are applied for as long as they are activated. All this mechanism will be linked to the spawn 
systme by maintaining an internal clock, and checking its respective tick count.
1. **Pass Through**
   - Allows the snake to pass through objects (destructible or not), as if it was thin air. 
   - The mechanism uses principles of quantum entanglement and tunneling, to ...
   - The maps' vulnerable objects are rendered every second the snake interacts with them.
2. **Radiation**
   - When enabled, the snake will destroy all destructible blocks in its path, determined by the direction it is facing.
   - Destructible Materials include Wood, Glass and Rubber.
3. **Shrink**
   - The snake shrinks back to the default size, and sheds off the rest of the skin.
4. **No Growth**
   - A secret formula now enables the snake to eat cherries without growing up! Helpful when the snake grows too long.
5. **Faster Growth**
   - The result when the secret formula development went wrong. Has inverse affects, increasing growth rate.
6. **Faster Speed**
   - The snake now moves faster. (As if the snake was crawling earlier...)
7. **Slower Speed**
   - The coefficient of friction of the surface suddenly increased! Oh wait...
8. **Magnet**
   - The snake turns into a magnet and attracts cherries from a 5 block radius.

## Materials 
In addition to powerps, there are obstacles of different materials in the level maps. These are represented by different characters, 
as the level map in general is a character matrix of size 102x42. These have different properties, and respective action is taken 
upon collision/interaction. Some of them are : 
- *Wood (BROWN)*   : Destructible; Brought from the Amazon rainforests.
- *Metal (GRAY)*   : Indestructible; High Grade Lead-Titanium Alloy, cast into sheets 10cm thick.
- *Glass (CYAN)*   : Destructible; Shatters when the snake it moving quite fast.
- *Rubber (GREEN)* : Destructible; Allows the snake to bounce off into the opposite direction. 

# Screenshots
