# Procedural 2D Caves using cellular automata

This is part of the exam for gameplayprogramming at Howest Digital Arts and entertainment. For the exam we were tasked to research a topic and implement it. I choose the procedural creation of cavesystems using cellular automata. The project initially seemed simple but proofed to be more of a challenge then I expected, but the result was worth the struggle. The program is using the gameplay programming framework and is written in C++.

![Result](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/ae0b7c32-5eb4-443f-bce3-8bd16d3ab37d)

## What is a cellular automaton 
A cellular automaton is an algorithm thats run on a grid. The grid is just a normal square grid but every cell has to have a finite amount of states for example in boolean automaton true/false. In this case it is only split into wall or cave. Each cell has a neighbourhood of the other cells surrounding it. You start by assigning every cell of the grid a state. The cellular automaton runs in iterations, every tick is one iteration. When the tick advances every cell changes according to a set mathematical rule that sets the new state in relation to the neighborhood. 

## Implementation 
As mentioned earlier for a cellular automaton to work there needs to be a finite number of states in my case I distinguish between Wall, Floor and Cave tiles. The cave tiles are not used for the automaton and are used to destinguish different caves. The grid size I chose for testing was 25x25 but it is scalable according to needs. I found the most natural look is about 80x80 cells. 

### 1. Initialization
After selecting the size of the grid all of cells are set to be wall tiles. To create interesting shapes with this algorithm the start state of the grid cannot be only type of tile since that would lead to nothing changing per iteration. For that we just randomly choose tiles and set them to be floor. Usually changing 40% of the tiles to whitespace leads to the best looking results but this can just be changed if a different result is required.

![image](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/e988b425-2a6a-44ce-83d3-7657ea611239)

### 2. Cellular Automata
Now the grid is ready to be shaped anew and create the first structures. The rule I use to change the tiles is the following:

If the amount of wall tiles surrounding in the neighborhood of each cell is:

    more than 5 (gain Cutoff) wall tiles, the tile becomes a wall tile itself.
    
    less than 3 (starve Cutoff) wall tiles, the tile becomes a floor tile.
    
    If none of this is accurate for the cell it just stays the same tile it is currently

I repeat this step 4 times but this is not set in stone and can be changed according to the users wish. Less iterations make it more rocky while more iterations can smooth more edges and open up more spaces.

![image](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/d8967e65-84e4-4129-aec5-742eb1bb60ca)


After this I run the cellular automata again this time it I set the starve cutoff to -1 this means this step cannot generate more floor tiles. This is specifically to close smaller holes so that theres not too many caves that are just tiny.

### 3. Connecting Caves

This step was more complicated than I first imagined. So I will split it up into smaller sections.

### 3.1 What is a cave and how do we find it?

To find a cave we just start every tile of the map. If it hits a floor tile it starts a floodfill algorithm, which checks all surrounding tiles and adds them to an open list it repeats the process until there are no longer any tiles that we have not checked. In this step we need the third cell state which is caves. We mark all found floor tiles as part of a cave and therefore they dont show up when checking for the floor tiles. When we have all the caves the algorithm stops and adds them to a list.

![image](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/91f7590c-72cd-4a84-be92-ad307126d58f)

### 3.2 Creating a connection

Now that we have all the caves we can determine their center by taking the average off the position from every Cell in the cave. This can technically go wrong if the shape of the cave is a U in which case the center is outside of the cave. In reality this has never come up in testing ever. 
With the centers of all caves we can simply run a pathfinding algorithm to connect them. I choose an implementation of A* but others will lead to a similiar result. After that just turn every wall tile on the path into a Floor tile and there now is a connection between the caves. But just connecting random caves together doesn't work when the cave system gets bigger for example on a 100 x 100 grid. What I did was give every Cave a boolean attribute of "IsConnected" now we only connect unconnected caves with connected caves. This leads to a better overall Shape and generates a mostly contiguous cave.

![image](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/6055fb32-7a75-424a-aa36-7087264db096)

### 3.3 Smoothing

Making these connections especially on a bigger grid can lead to some unnatural looking lines since A* chooses the fastest way to the target. To counteract that we simply run the cellular automaton this time with a gain cutoff of 9 which means that it cannot add anymore walls. This way no paths get blocked again but the shape becomes less blocky.

![image](https://github.com/Howest-DAE-GD/gpp-researchtopic-Dezer-Ted/assets/85624966/4c87a58b-2b5f-4c18-9844-6af0b2a928c9)


## Conclusion

This was a really fun challenge, but also made me struggle a few times. I originally only wanted to do the cellular automaton but without the connected Caves it looked significantly less good and felt really unfinished Im glad I implemented the connections even though they took a lot of work to get them to look correct. 
I chose this topic because I really like Dungeons and Dragons and with this I finally have a tool to generate the maps I use for my players. It works really well and the players liked it. 

# Sources

Bronson Zgeb's Blog: [https://bronsonzgeb.com/index.php/2022/01/30/procedural-generation-with-cellular-automata/]

Kodeco's article about this technique:[https://www.kodeco.com/2425-procedural-level-generation-in-games-using-a-cellular-automaton-part-1]

Cellular Automata Wikipedia: [https://en.wikipedia.org/wiki/Cellular_automaton]

