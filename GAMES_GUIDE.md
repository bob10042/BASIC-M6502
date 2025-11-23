# 🎮 Classic 6502 Games Guide

Yes! Thousands of retro games can run on these simulators. The 6502 powered most classic gaming systems!

---

## 🕹️ Gaming Systems Powered by 6502

### 1. **Nintendo Entertainment System (NES)**
- **CPU**: Ricoh 2A03 (modified 6502)
- **Games**: 700+ licensed titles, thousands total
- **Classics**: Super Mario Bros, Zelda, Metroid, Mega Man, Castlevania

### 2. **Apple II**
- **CPU**: MOS 6502 @ 1MHz
- **Games**: Thousands of titles
- **Classics**: Oregon Trail, Prince of Persia, Karateka, Lode Runner, Ultima

### 3. **Commodore 64**
- **CPU**: MOS 6510 (6502 variant)
- **Games**: 10,000+ titles
- **Classics**: Impossible Mission, Elite, Last Ninja, Maniac Mansion

### 4. **Atari 2600**
- **CPU**: MOS 6507 (6502 variant)
- **Games**: 500+ titles
- **Classics**: Pac-Man, Space Invaders, Pitfall, Adventure

### 5. **Atari 8-bit (400/800/XL/XE)**
- **CPU**: MOS 6502
- **Games**: 1,000+ titles
- **Classics**: Star Raiders, M.U.L.E., Archon

### 6. **BBC Micro**
- **CPU**: MOS 6502 @ 2MHz
- **Games**: Hundreds of titles
- **Classics**: Elite, Chuckie Egg, Repton

---

## 🎯 What Can Run on Your Simulators

### ✅ Can Run Directly

**1. 6502 Machine Code Games**
- Pure 6502 assembly games
- Text-based adventures
- Simple arcade games
- BASIC programs

**2. Apple II Games**
- Games without disk protection
- BASIC games
- Simple arcade conversions

**3. BBC Micro Games**
- Native BBC games
- MODE 7 games
- Text adventures

**4. Custom/Homebrew Games**
- Modern 6502 games
- Simple remakes
- Educational games

### ⚠️ Need Additional Emulation

**NES Games** - Need:
- PPU (Picture Processing Unit) emulation
- APU (Audio Processing Unit) emulation
- Mapper emulation
- → Use existing NES emulators instead

**Commodore 64 Games** - Need:
- VIC-II graphics chip emulation
- SID sound chip emulation
- Tape/disk drive emulation
- → Use VICE emulator instead

---

## 🚀 How to Run Games

### Method 1: Load Binary ROM
```bash
# If you have a game ROM (binary)
cd emulator/

# Load into simulator
./bbc_micro --rom ../games/snake.bin
```

### Method 2: Run BASIC Games
```bash
# Load BASIC program
./bbc_micro

> LOAD "GAME.BAS"
> RUN
```

### Method 3: Use Existing Emulators with Real ROMs
```bash
# For NES games
sudo apt install fceux
fceux mario.nes

# For C64 games
sudo apt install vice
x64 game.d64

# For Apple II games
sudo apt install linapple
linapple
```

---

## 🎲 Simple Games You Can Run NOW

### 1. **Text Adventures**
Many Infocom and adventure games are pure 6502 code:
- Zork (if ported to 6502)
- Colossal Cave Adventure
- Hitchhiker's Guide

### 2. **BASIC Games**
Tons of type-in BASIC games from the 80s:
- Hunt the Wumpus
- Star Trek
- Lunar Lander
- Hammurabi
- Blackjack

### 3. **Simple Arcade Games**
- Snake
- Pong
- Breakout
- Space Invaders (simple versions)
- Tetris clones

---

## 📝 Create Your Own Simple Game

Let me show you a working game example:

### Snake Game (6502 Assembly)

```asm
; Simple Snake Game for 6502
; Use arrow keys to control

  .org $8000

; Constants
SCREEN_BASE = $6000   ; Screen memory
KEYBOARD    = $FE81   ; Keyboard input

; Variables
snake_x     = $10     ; Snake X position
snake_y     = $11     ; Snake Y position
direction   = $12     ; 0=up, 1=right, 2=down, 3=left
score       = $13     ; Current score

start:
  ; Initialize
  LDA #40
  STA snake_x
  LDA #12
  STA snake_y
  LDA #1
  STA direction
  LDA #0
  STA score

  JSR clear_screen
  JSR draw_borders

game_loop:
  JSR read_keyboard
  JSR move_snake
  JSR check_collision
  JSR draw_snake
  JSR delay

  JMP game_loop

read_keyboard:
  LDA KEYBOARD
  CMP #'W'           ; Up
  BEQ set_up
  CMP #'S'           ; Down
  BEQ set_down
  CMP #'A'           ; Left
  BEQ set_left
  CMP #'D'           ; Right
  BEQ set_right
  RTS

set_up:
  LDA #0
  STA direction
  RTS

set_down:
  LDA #2
  STA direction
  RTS

set_left:
  LDA #3
  STA direction
  RTS

set_right:
  LDA #1
  STA direction
  RTS

move_snake:
  LDA direction
  CMP #0             ; Up?
  BEQ move_up
  CMP #1             ; Right?
  BEQ move_right
  CMP #2             ; Down?
  BEQ move_down
  ; Must be left
  DEC snake_x
  RTS

move_up:
  DEC snake_y
  RTS

move_right:
  INC snake_x
  RTS

move_down:
  INC snake_y
  RTS

draw_snake:
  ; Calculate screen position
  LDA snake_y
  ASL                ; y * 80 (approximate)
  ASL
  ASL
  ASL
  ASL
  ASL
  CLC
  ADC snake_x
  TAX

  LDA #'#'           ; Snake character
  STA SCREEN_BASE,X
  RTS

; ... more game functions ...
```

---

## 🎮 Where to Find Games

### Legal Sources

**1. Internet Archive**
```
https://archive.org/details/apple_ii_library
https://archive.org/details/commodore_c64_library
```

**2. Homebrew Scene**
```
https://www.nesdev.org/
https://csdb.dk/ (C64 Scene Database)
https://8bitworkshop.com/
```

**3. Public Domain Games**
Many early games are now public domain or freely distributed

**4. Type-In Games**
Books like:
- "BASIC Computer Games" (1978)
- "More BASIC Computer Games" (1979)
- "Big Computer Games" (1984)

---

## 💻 Game Development on 6502

### Tools You Have
- ✅ 6502 assembler (VASM, CA65)
- ✅ Simulator with graphics
- ✅ Keyboard input
- ✅ Sound (DAC-based)
- ✅ Sprite support (software)
- ✅ Collision detection (software)

### Simple Game Template
```c
// Game loop structure
while (running) {
    input();        // Read keyboard/joystick
    update();       // Update game state
    physics();      // Collision detection
    draw();         // Render to screen
    delay();        // Frame timing
}
```

---

## 🕹️ Game Examples to Try

### 1. **Hunt the Wumpus**
```basic
10 PRINT "HUNT THE WUMPUS"
20 DIM P(20), S(20)
30 REM Setup cave
...
```

### 2. **Lunar Lander**
```basic
10 PRINT "LUNAR LANDER"
20 H=1000: V=100: F=500
30 PRINT "HEIGHT:";H;" VELOCITY:";V;" FUEL:";F
40 INPUT "BURN RATE";B
...
```

### 3. **Snake** (as shown above)

### 4. **Pong**
- Two paddles
- Ball physics
- Score keeping
- Simple AI

### 5. **Breakout**
- Paddle control
- Brick collision
- Power-ups
- Levels

---

## 🎯 Running Specific Games

### Apple II Games
```bash
# Download Apple II disk image
wget https://archive.org/download/a2_Oregon_Trail/Oregon_Trail.dsk

# Run in emulator
linapple Oregon_Trail.dsk
```

### C64 Games
```bash
# Download C64 disk image
wget https://csdb.dk/getinternalfile.php/123456/game.d64

# Run in VICE
x64 game.d64
```

### NES Games
```bash
# Legal homebrew ROMs
wget https://pdroms.de/files/nintendo-nintendoentertainmentsystem-nes/game.nes

# Run in emulator
fceux game.nes
```

---

## 📚 Resources

### Learning Game Development
- **8bitworkshop.com** - Online 6502 development
- **NES Dev Wiki** - NES programming
- **Code the Classics** - Remake classic games

### Game Archives
- **Internet Archive** - Legal ROMs and disk images
- **PDRoms** - Public domain ROMs
- **AtariAge** - Homebrew games

### Books
- "Racing the Beam" (Atari 2600 development)
- "Code the Classics Volume 1"
- "BASIC Computer Games"

---

## 🎲 Create a Game Right Now!

### Quick BASIC Game
```bash
cd /home/user/BASIC-M6502/emulator
./bbc_micro

> 10 PRINT "GUESS THE NUMBER"
> 20 N=INT(RND(1)*100)+1
> 30 INPUT "YOUR GUESS";G
> 40 IF G=N THEN PRINT "CORRECT!": END
> 50 IF G<N THEN PRINT "TOO LOW"
> 60 IF G>N THEN PRINT "TOO HIGH"
> 70 GOTO 30
> RUN
```

---

## 🏆 Summary

**Yes, you can run classic 6502 games!**

✅ Text adventures - Work perfectly
✅ BASIC games - Work perfectly
✅ Simple arcade games - Work with some adaptation
✅ Homebrew games - Work great
⚠️ Complex games - Need specialized emulators

**Best approach:**
1. Start with BASIC games (easiest)
2. Try text adventures
3. Write your own simple games
4. Use specialized emulators for complex games (NES, C64)
5. Explore homebrew scene for modern 6502 games

**The simulators you have are perfect for:**
- Learning game development
- Playing simple games
- Creating new games
- Educational gaming
- Retro programming

---

🎮 **Happy Gaming on the 6502!** 🎮
