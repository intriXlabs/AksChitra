# main architecture

## best explaination
so fuck the buzzwords and let me cook now
- terminal is just a simple insert() based display that's it
- now ansii code gets to do color blah blah
- now what i going to do is adding a damn display to it

note: display is not enough - i have to do two things to manage the architecture
1. clean and logical code saperation to understand it
2. making it damn immersive

now let's make teminal devs cry their soul out.

# logical reasoning

## what even the damn inside terminal is?
- terminal is just characters that's it
- ansii codes give them foreground and background colors
- ansii let them move around and do some fancy stuff like cursor movement, clearing the screen, etc

now there are total 256 single byte characters which can work without a damn issue of width unlike utf and emoji stuff

## how the damn we going to do display?
ask your soul what the hell is a display:
- it's a damn pixel grid having colors
- but terminal is not pixel grid - it;s the damn character grid

** stamp this shit in your mind - terminal is a fucking character grid simulated by display pixels itself the hardware provide" **

# mental model:
- terminal is stateless shit
- there is no memory of whatever you draw on it - it just shows the damn stuff and that's it

## what i going to do with this girlie:
- i am going to make a damn display on it but how?
what is display again? **character grid** but it's not just a terminal grid.

it is character grid - but only if you see terminal as characters

## twisted model of terminal engineering:
add ansii code and colors to terminal characters - now what it is?
- each character having foreground and background color - now what it is?
- now it's a damn pixel grid - but not a pixel grid - it's a damn character grid
- each character holds {character, foreground color, background color} but now here's twist

## the hidden curse of terminal:
- terminal is not buffer like x11 or wayland or even windows
- but it have ansii codes let it move - yeah this mf "\033[H"
- this movement gives far more frames per second potential: fuck ncurses, notcurses and ratattui - i am leaking this shit here

## now what? 
there are few things to understand:
- if your terminal is not having diff rendering then either you are stupid as fuck or something wrong with you

## now what terminal is, with colors bg fg character and a display 2D array?
- right it is a display simulationL: it. is. a. fucking. display. simulation.

# let's play with sould of terminal now

## how we going to do this:
best way to see it by understand bandwith of terminal which is fucking low - so we have to do some tricks to make it immersive and interactive
- saperate display with pixel(simulated by character) and character grid
trick: colored bg + space = pixel

- pixel going to render the damn scenes etc on pixel manner
- while character grid going to render the damn text and other stuff on character manner

means two displays are going to be there - one for pixel and one for character grid to do the immersive stuff

## now what is the damn display?
imagine a grid but of what? what a grid can have? primitive - this primitive is what makes and architecture workable and better. now i categorise it logicaly

### primitive of pixelDisplay:
- pixel: a single pixel having color and position
pixel {
    uint8_t r, g, b; // color values
}

### priomitive of characterDisplay:
- character: a single character having color and position
character {
    char c; // character value
    uint8_t fgr, fgg, fgb; // foreground color values
    uint8_t bgr, bgg, bgb; // background color values
}

## reasons:
- pixelDisplay is going to be used for rendering the damn scenes and other stuff and not having fg because colored space does not need it - yeah it's the tricky stuff
- characrterDisplay is going to be used for rendering the text and other stuff and having fg and bg because it's the tricky stuff

note: fun stops now - now real and complex stuff is going to start - now i am going to make a damn architecture for this shit

# display architecture:

## basic primitives:

for pixelDisplay:

```
pixel {
    uint8_t r, g, b; // color values
}
```

for characterDisplay:

```
character {
    char c; // character value
    uint8_t fg_r, fg_g, fg_b; // foreground color values
}
```

NOTE: they're only primitives - not the display or someting

## display architecture:
display is going to be a 1D array behaving as 1D mf to provide linear access to the dsiplay - tricky stuff

```
pixel* pixelDisplay; // 1D array of pixels
character* characterDisplay; // 1D array of characters
```

## perimeter of display:
there are just two needed:
```
int displayRows, displayCols; // dimensions of the display
```
## a simple operation to turn up displays
initializeDisplay
```
- taking terminal size
- setting up pixelDisplay and characterDisplay arrays
- setting up buffer size of the display


pixelDisplay = new pixel[displayRows * displayCols];
characterDisplay = new character[displayRows * displayCols];

// i assume you knowing it already - for the sake of clarity.
isDisplayInitialized = true;

```

done

**remember one thing**: each requested vector using this struct internaly combine with main strcut of primitive
```
struct coord {
    int row, col;
};
```

# diff rendering trick i going to use:

## vectorisation of queue:
- i am going to use a vector to store the pixels or characters requested from engine.

now thing is there are going to be four different types of vectors
- requestedPixels: vector of pixels requested to be drawn on pixelDisplay
- requestedCharacters: vector of characters requested to be drawn on characterDisplay
- requestedStatelessPixels: vector of pixels requested to be drawn on pixelDisplay but not stored in pixelDisplay
- requestedStatelessCharacters: vector of characters requested to be drawn on characterDisplay but not stored

WHY? simple - not every character has to be in memory.

## logic:
it bypass the need to check new and old display and just render the requested pixels and characters on the display
later we just open the pixels from vector and going to do this
- adding coordaintes
- adding color values
- buffer addressing
- buffer push

# basic stupid API:
aPI is going to be simple and easy to use - just like a damn canvas but intuitive

## API:
### setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) and also setPixel(int row, int col, std::string colorCode)
remember it alwaays - i not goint to tell a freaking helper function - that is your job not mine
color code just do this: check what it is hexcode, rgb() or "red" etc and then call uint function with defined colors

it adds it to requestedPixels vector
**clarification**: we not adding it to display coz we going to do it later in rendering phase.

READ IT: keep all function secure is your shit

### setStatelessPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) and also setStatelessPixel(int row, int col, std::string colorCode)
which add it to requestedStatelessPixels vector
**clarification**: we not adding it to display coz we going to do it later in rendering phase.

### setCharacter(int row, int col, char c, uint8_t fgr, uint8_t fgg, uint8_t fgb, uint8_t bgr, uint8_t bgg, uint8_t bgb) and also setCharacter(int row, int col, char c, std::string fgColorCode, std::string bgColorCode)
which add it to requestedCharacters vector
**clarification**: we not adding it to display coz we going to do it later in rendering phase.]
**stop and read first**: characters are going to read bg pixel then use same color as their bg means there is no concept of bg color for characters - they are going to use pixel color as their bg color and fg color is going to be used as their fg color - tricky stuff

### setStatelessCharacter(int row, int col, char c, uint8_t fgr, uint8_t fgg, uint8_t fgb, uint8_t bgr, uint8_t bgg, uint8_t bgb) and also setStatelessCharacter(int row, int col, char c, std::string fgColorCode, std::string bgColorCode)
which add it to requestedStatelessCharacters vector
**clarification**: we not adding it to display coz we going to do it later in rendering phase.

## get api:
is going to be simple - just like a damn canvas but intuitive
- getPixel going to give the whole data struct of pixel at that position from grid we set up earlier. not vector - a grid.
- getCharacter going to give the whole data struct of character at that position from grid we set up earlier. not vector - a grid.

# rendering phase:
- now we going to render the damn stuff on terminal - tricky stuff

see it is simple as hell render() but divided into four parts: each one does it's own work:
- renderRequestedPixels() - which renders the requested pixels on pixelDisplay and then on terminal
- renderRequestedCharacters() - which renders the requested characters on characterDisplay and then on terminal
- renderRequestedStatelessPixels() - which renders the requested stateless pixels on pixelDisplay and then on terminal
- renderRequestedStatelessCharacters() - which renders the requested stateless characters on characterDisplay and then on terminal

yeah all of them are going to helper functions before the main render() function - keeping it simplest yet portable and debugable.

idea is more basci

std::string buffer;
then passing it to each as pointer and later render this buffer to terminal using write() function.

# done
yeah - it's over. completed.

# yes it is exposed to global scope due to architecture i building.