# RobCoGWeb
## Games for Robot Learning

 The idea behind the project is to develop a virtual environment in which the user can interact with various assets and be able to perform real-life tasks, in order to store the information and create a semantic map for the Robot Knowledge Base. The environment represents the actual kitchen from the laboratory and the end goal is to use the game engine as a way to accumulate a large amount of trials from multiple people, in order to generate episodic memory to help the robot react when given a certain situation. The project targets more of a high-level learning, meaning that it follows the steps of the workflow as a whole rather than trajectories and control process.
 
### C++ Classes

The user controls a first person character with behaviour set up in the C++ class 'MyCharacter'. The character can move around the virtual world, open and close doors or drawers, pick up items, drop items, adjust position and rotation of items held in hand as well as picking multiple items at once. It has two item slots, one coresponding to each hand, and a two-hand slot used when picking several items with both hands.

#### MyCharacter constructive logic

The HUD used has a canvas drawn in the middle of the screen. Since the perspective is first person, a ray trace is cast from the camera component to the direction the character is facing, returning the HitObject of the item in the world which the player is pointing at.
This HitObject is updated every frame and stored in the HighlightedActor variable. Depending on it's nature and properties, it responds to the inputs gived by the user differently. For example, when focusing on an item which can pe interacted, a highlight effect is added to help the user. By clicking, the highlighted item will be removed from the world and added to one of the hand slots. Alternatively if the user is allready holding an item and clicks on a surface, the action will be to drop it back in the world at the determined location.
The logic for picking up stacks works on the same principle, but instead of just manipulating one item, it manipulates a set of items all at once (with respect to having the same nature and properties and being placed in a 'stacked' manner).
Custom delegates have been implemented to help communicate between classes and send messages to the GameMode which is responsible with the level progress, user interface, as well as in game help messages.

#### RobCoGWebGameMode

As mentioned before, the game mode class handles messages to help the user. Whenever a function called is invalid or not permited, the game mode class prints a message to the screen indicating which exactly of his actions is not allowed. The display message is written in the MyCharacter class (depending on the case it popped in) and sent to the game mode to print to the screen.
The class handles ending game as well as updating the progress which the user has made

### Blueprints used

The Actors effectively used in the game are not C++ templates, but rather Blueprints derived from the written code class. Blueprints are an effective way to communicate inside the engine and therefore it complements the C++ code functionality.

MyCharacterHUD_BP:
- Draws canvas to the center of the screen
- Has message slots which are read from variables stored in GameMode and printed to the screen

MyCharacter_BP:
- Draws the HUD to the display screen
- Handles pausing the game

Widgets such as MainMenu_BP, EndGame_BP are also defined as blueprints.

### Assets and Models

Most of the assets used are .fbx and corespond to real life assets. They have been either created using Blender or 3d scanned and added to the engine and the world.
Currently there are 4 static assets: Oven area, Sink area, Fridge area, Island area, all of which have drawers or doors which can be opened and closed, and around 40 movable assets, varying from silverware (spoons, knives, etc.) to bowls and plates and consumables (milk, cereal, salt, etc.)

### Levels

- Tutorial Level:  helps the player get familiar with the controls and the environment
- Breakfast Level: where the player needs to set up the table for a couple of people to have breakfast
- Cleaning Level: where the player needs to clean the kitchen and place everything where he thinks they belong.

