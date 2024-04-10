Shadowgrove Game: 
* Full M4 Feature Updates and Creative Components list: *

(Story Elements – Advanced) Branching storyline that reacts to player crafting. Adaptive Cutscenes are different depending on which order the player crafts the potions in. Added 12 cutscenes total, including dialogue between player and dog to tell the players story through dialogue. If the player wins before the 7th night, they won’t see the warning cutscene or the out of time cutscene. If the player dies during the game, they won’t ever see the win cutscenes 
- 3 cutscenes right at the beginning of the game to introduce story. 
- 2 cutscenes after the 1st night cycle, to introduce what the night cycle is. 
- 3 cutscenes that appear after a healing potion is created, differing depending on player progress, explaining that the player created a healing potion. 
- 1 cutscene after the 7th night cycle, warning the player about running out of time. 
- 1 cutscene after the 9th night cycle, explaining that you’re out of time and the dog died.
- 2 cutscenes when the player wins, explaining how the player saves the dog. 

(Game Balancing – basic) Optimized our game to be fun! See separate game balancing PDF in repo.  
(Playability): our game has 9 levels, which equates to 9 different night cycles, each one different and harder than the last. This should allow at least 10 minutes of non-repetitive gameplay since each night cycle is 1 minute, and there is significant gameplay during the daytime as well.  

* (User Experience) *  
- Added in a tutorial at the beginning of the game. 
- At the end of the game once the player has made all the healing potions, sparks appear around the dog and potions and get larger as the player approaches, to indicate that this is what the player should go. In case this isn't immediately clear, the player isn’t able to enter night anymore, which should force them to wander around. 
- Added sound effects to torch lighting. 
- Added a sun/moon dial to track what time of day/night it is. 
- Added static goal potions to day screen to signify progress. Game goal potions are greyed out in crafted screen to illustrate unable to make more and show completion. 
- Added a “rooster call” sound effect and “good morning” text at end of each night cycle.  
- Added option to cycle through inventory with < and > , in addition to selecting 1,2,3,4,5. 

Graphics updates: 
- Dog has breathing effect via transform. 
- Animation rotation and scale of all plants.  
- Animated sparkles effects damage and invincibility, highlighting plants and story end signifying. 
- Added a Sun & Moon dial for time tracking.   

Minor updates: 
- Small tweaks to map generation to make forest seem more organic. 
- Game level cap and new game over / lose option. 
- Added new Keeper/Enemy types with different behavior and abilities. 
- 3 new potions with 3 new effects added. 
- Rolling credits on win screen 
- Game balancing  
- Bug fixes 

 







**Full M3 Feature Updates and Creative Components list:** 

(Graphics - Simple rendering effects - basic) Dynamic animated and interpolated day/night lighting system with and without flickering torchlight. Interpolating over a variety of textures depending on time of night or if a torch is used. If a torch is used the torchlight is animated with a flickering effect that uses slightly different colours and locations of the torchlight. 

(Graphics) All graphics are hand drawn and edited digitally. Visually coherent sprites are all drawn in the same style. As the game level progresses the biomes and plants change to provide a progressively more spooky atmosphere while keeping style consistent. 

(Advanced Procedural Generation) Organic Procedural Map Generation. Layer one a series of multiconnected map chunks. Each map chunk is a generated maze-like path of nodes. Each node then receives an appropriate shape for its relative place in connective nodes. There are a variety of noise levels. One patch based clustered noise algorithm for biome specific lookup of plant type and another white noise algorithm for sprite selection within the specific biome. As the game progresses the biomes update to more advanced plants.   

(Story Element) Inventory and collectible items for crafting potions to save dog and win. Crafting potions in the daytime using the UI component gives players tasks to accomplish in order to complete the game. The crafting UI also tells players which items to be on the lookout for during night and gives them purpose in their nighttime missions to search for items. 

(AI - basic) Improved A* AI with Patrol and Attack modes 

(Playability) 8-way movement increases ease of control. Added inventory item use. 

(Reloadability) Player restarts with inventory items.  

New game screens: 
- 	Title Screen 
- 	Day Screen 
- 	Night Screen ends after night timer  
- 	Winning Screen – with extra animated cuteness 
- 	Game Over Screen  

Level Progression (Playability):  
- increasing hardness with enemy spawn rates and numbers 
- with changing graphics for forest 

Major Bug fixes: 
-  Collision 
- Clipping through plants 
- Rendering player on top now instead of having bushes over player head 
- AI 
- Map generation  

