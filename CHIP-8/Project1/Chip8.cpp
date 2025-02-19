//#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <string.h>

class Chip8
{
public:
    uint8_t registers[16]{};        // Storage V0 - VF (all CPU oprations)
    uint8_t memory[4096]{};         // 4 bytes (interpreter, characters, intructions)
    uint16_t index{};               // Memory addresses
    uint16_t pc{};                  // Address of the next instruction to execute
    uint16_t stack[16]{};           // Tracks order of execution
    uint8_t sp{};                   // Tracks top of stack
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};           // 0 - F input keys
    uint32_t video[64 * 32]{};      // Pixels
    uint16_t opcode;



    const unsigned int START_ADDRESS = 0x200;           // Start address for instructions in memory
    const unsigned int FONTSET_START_ADDRESS = 0x50;    // Start address for characters in memory

    const unsigned int VIDEO_WIDTH = 64;
    const unsigned int VIDEO_HEIGHT = 32;



    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;



    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];



    Chip8() :randGen(std::chrono::system_clock::now().time_since_epoch().count())
    {
        // Initialize PC
        pc = START_ADDRESS;

        // Load fonts into memory
        for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
        {
            memory[FONTSET_START_ADDRESS + i] = fontset[i];
        }

        // Initialize Random Number Generator (RNG)
        randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

        // Array of function pointers for the first digits ($0 to $F) of the opcode
        table[0x0] = &Chip8::Table0;
        table[0x1] = &Chip8::OP_1nnn;
        table[0x2] = &Chip8::OP_2nnn;
        table[0x3] = &Chip8::OP_3xkk;
        table[0x4] = &Chip8::OP_4xkk;
        table[0x5] = &Chip8::OP_5xy0;
        table[0x6] = &Chip8::OP_6xkk;
        table[0x7] = &Chip8::OP_7xkk;
        table[0x8] = &Chip8::Table8;
        table[0x9] = &Chip8::OP_9xy0;
        table[0xA] = &Chip8::OP_Annn;
        table[0xB] = &Chip8::OP_Bnnn;
        table[0xC] = &Chip8::OP_Cxkk;
        table[0xD] = &Chip8::OP_Dxyn;
        table[0xE] = &Chip8::TableE;
        table[0xF] = &Chip8::TableF;


        for (size_t i = 0; i <= 0xE; i++)
        {
            table0[i] = &OP_NULL;
            table8[i] = &OP_NULL;
            tableE[i] = &OP_NULL;
        }


        // Tables for repeating digits
        table0[0x0] = &OP_00E0;
        table0[0xE] = &OP_00EE;

        // Functions pointers that indexes correctly
        table8[0x0] = &OP_8xy0;
        table8[0x1] = &OP_8xy1;
        table8[0x2] = &OP_8xy2;
        table8[0x3] = &OP_8xy3;
        table8[0x4] = &OP_8xy4;
        table8[0x5] = &OP_8xy5;
        table8[0x6] = &OP_8xy6;
        table8[0x7] = &OP_8xy7;
        table8[0xE] = &OP_8xyE;

        tableE[0x1] = &OP_ExA1;
        tableE[0xE] = &OP_Ex9E;


        for (size_t i = 0; i <= 0x65; i++)
        {
            tableF[i] = &OP_NULL;
        }

        // Function pointers that indexes correctly
        tableF[0x07] = &OP_Fx07;
        tableF[0x0A] = &OP_Fx0A;
        tableF[0x15] = &OP_Fx15;
        tableF[0x18] = &OP_Fx18;
        tableF[0x1E] = &OP_Fx1E;
        tableF[0x29] = &OP_Fx29;
        tableF[0x33] = &OP_Fx33;
        tableF[0x55] = &OP_Fx55;
        tableF[0x65] = &OP_Fx65;
    }



    // Loads instructions into memory in a ROM file.
    void LoadROM(char const* filename)
    {
        // Open the file as a stream of binary and move the file pointer to the end
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (file.is_open())
        {
            // Get size of file and allocate a buffer to hold the contents
            std::streampos size = file.tellg();
            char* buffer = new char[size];

            // Go back to the beginning of the file and fill the buffer
            file.seekg(0, std::ios::beg);
            file.read(buffer, size);
            file.close();

            // Load the ROM contents into the Chip8's memory, starting at 0x200
            for (long i = 0; i < size; ++i)
            {
                memory[START_ADDRESS + i] = buffer[i];
            }

            // Free the buffer
            delete[] buffer;
        }
    }



    // Characters / Sprites (5 bytes each)
    const static unsigned int FONTSET_SIZE = 80;

    uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };


    // ------- INSTRUCTIONS --------


    // CLS ~~ Clear the display
    void OP_00E0()
    {
        memset(video, 0, sizeof(video));
    }


    // RET ~~ Return from a subroutine
    void OP_00EE()
    {
        --sp;
        pc = stack[sp];
    }


    // JP addr ~~ Jump to location nnn (interpreter sets PC to nnn)
    void OP_1nnn()
    {
        uint16_t address = opcode & 0x0FFFu;

        pc = address;
    }


    // CALL addr ~~ Call subroutine at nnn
    void OP_2nnn()
    {
        uint16_t address = opcode & 0x0FFFul;

        stack[sp] = pc; // puts current PC on top of stack, current PC holds the next instruction after this CALL.
        ++sp;
        pc = address;
    }


    // SE Vx, byte ~~ Skip next instruction if Vx = kk
    void OP_3xkk()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFU;

        if (registers[Vx] == byte)
        {
            pc += 2;
        }
    }



    // SNE Vx, byte ~~ Skip next instruction if Vx != kk
    void OP_4xkk()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFU;

        if (registers[Vx] != byte)
        {
            pc += 2;
        }
    }



    // SE Vx, Vy ~~ Skip next instruction if Vx = Vy
    void OP_5xy0()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vx] == registers[Vy])
        {
            pc += 2;
        }
    }


    // LD Vx, byte ~~ Set Vx = kk
    void OP_6xkk()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFU;

        registers[Vx] == byte;
    }


    // ADD Vx, byte ~~ Set Vx = Vx + kk
    void OP_7xkk()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00FFU;

        registers[Vx] += byte;
    }


    // LD Vx, Vy ~~ Set Vx = Vy
    void OP_8xy0()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vy];
    }


    // OR Vx, Vy ~~ Set Vx = Vx OR Vy
    void OP_8xy1()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] |= registers[Vy];
    }


    // AND Vx, Vy ~~ Set Vx = Vx AND Vy
    void OP_8xy2()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] &= registers[Vy];
    }


    // XOR Vx, Vy ~~ Set Vx = Vx XOR Vy
    void OP_8xy3()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] ^= registers[Vy];
    }


    // ADD Vx, Vy ~~ Set Vx = Vx + Vy and Set VF = carry (ie. ADD but with a flag for overflow)
    void OP_8xy4()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        uint16_t sum = registers[Vx] + registers[Vy];

        if (sum > 255U) // ie. > 1 byte
        {
            registers[0xF] = 1;
        }
        else
        {
            registers[0xF] = 0;
        }

        registers[Vx] = sum & 0xFFu;
    }


    // SUB Vx, Vy ~~ Set Vx = Vx - Vy and Set VF = NOT borrow (ie. Substraction with a flag for negative sign)
    void OP_8xy5()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vx] > registers[Vy])
        {
            registers[0xF] = 1;
        }
        else
        {
            registers[0xF] = 0;
        }

        registers[Vx] -= registers[Vy];
    }


    // SHR Vx ~~ Set Vx = Vx SHR 1 (ie. right-shift), least significant bit is saved in VF.
    void OP_8xy6()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        registers[0xF] = (registers[Vx] & 0x1u); // Saves LSB in VF

        registers[Vx] >>= 1;
    }


    // SUBN Vx, Vy ~~ Set Vx = Vy - Vx, rest is same as OP_8xy5()
    void OP_8xy7()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;


        if (registers[Vy] > registers[Vx])
        {
            registers[0xF] = 1;
        }
        else
        {
            registers[0xF] = 0;
        }

        registers[Vx] = registers[Vy] - registers[Vx];
    }


    // SHL Vx {, Vy} ~~ Set Vx = Vy SHL 1 (ie. left shift), most significant bit is saved in Vf
    void OP_8xyE()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        registers[0xF] = (registers[Vx] & 0x80u) >> 7u; // Saves MSB in VF

        registers[Vx] <<= 1;
    }


    // SNE Vx, Vy
    void OP_8xyE()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        registers[0xF] = (registers[Vx] & 0x80u) >> 7u; // Saves MSB in VF

        registers[Vx] <<= 1;
    }


    // SNE Vx, Vy ~~ Skip next instruction if Vx != Vy
    void OP_9xy0()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vx] != registers[Vy])
        {
            pc += 2;
        }
    }


    // LD I, addr ~~ Set I = nnn
    void OP_Annn()
    {
        uint16_t address = opcode & 0x0FFFu;

        index = address;
    }


    // JP V0, addr ~~ Jump to location nnn + V0
    void OP_Bnnn()
    {
        uint16_t address = opcode & 0x0FFFu;

        pc = registers[0] + address;
    }


    // RND Vx, byte ~~ Set Vx = random byte and kk
    void OP_Cxkk()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t byte = opcode & 0x00F0u;

        registers[Vx] = randByte(randGen) & byte;
    }


    // DRW Vx, Vy, nibble ~~ Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    void OP_Dxyn()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;
        uint8_t height = opcode & 0x000Fu;

        // Wrap if going beyond screen boundaries
        uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
        uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

        registers[0xF] = 0;

        for (unsigned int row = 0; row < height; ++row)
        {
            uint8_t spriteByte = memory[index + row];

            for (unsigned int col = 0; col < 8; ++col)
            {
                uint8_t spritePixel = spriteByte & (0x80u >> col);
                uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

                // Sprite pixel is on
                if (spritePixel)
                {
                    //Screen pixel also on - collision
                    if (*screenPixel == 0xFFFFFFFF)
                    {
                        registers[0xF] = 1;
                    }

                    // XOR with sprite pixel
                    *screenPixel ^= 0xFFFFFFFF;
                }
            }
        }

    }


    // SKP Vx ~~ Skip next instruction if key with value Vx is pressed
    void OP_Ex9E()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t key = registers[Vx];

        if (keypad[key])
        {
            pc += 2;
        }
    }


    // SKNP Vx ~~ Skip next instruction if key with value Vx is NOT pressed
    void OP_ExA1()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t key = registers[Vx];

        if (!keypad[key])
        {
            pc += 2;
        }
    }


    // LD Vx, DT ~~ Set Vx = delay timer value
    void OP_Fx07()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        registers[Vx] - delayTimer;
    }



    // LD Vx, K ~~ Wait for a key press, and store value of key in Vx
    void OP_Fx0A()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;


        if (keypad[0])
        {
            registers[Vx] = 0;
        }
        else if (keypad[1])
        {
            registers[Vx] = 1;
        }
        else if (keypad[2])
        {
            registers[Vx] = 2;
        }
        else if (keypad[3])
        {
            registers[Vx] = 3;
        }
        else if (keypad[4])
        {
            registers[Vx] = 4;
        }
        else if (keypad[5])
        {
            registers[Vx] = 5;
        }
        else if (keypad[6])
        {
            registers[Vx] = 6;
        }
        else if (keypad[7])
        {
            registers[Vx] = 7;
        }
        else if (keypad[8])
        {
            registers[Vx] = 8;
        }
        else if (keypad[9])
        {
            registers[Vx] = 9;
        }
        else if (keypad[10])
        {
            registers[Vx] = 10;
        }
        else if (keypad[11])
        {
            registers[Vx] = 11;
        }
        else if (keypad[12])
        {
            registers[Vx] = 12;
        }
        else if (keypad[13])
        {
            registers[Vx] = 13;
        }
        else if (keypad[14])
        {
            registers[Vx] = 14;
        }
        else if (keypad[15])
        {
            registers[Vx] = 15;
        }
        else
        {
            pc -= 2; // Simulates waiting (executes same instruction repeatedly)
        }
    }


    // LD DT, Vx ~~ Set delayTimer = Vx
    void OP_Fx15()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        delayTimer = registers[Vx];
    }


    // LD ST, Vx ~~ Set soundTimer = Vx
    void OP_Fx18()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        soundTimer += registers[Vx];
    }


    // ADD I, Vx ~~ Set I = I + Vx
    void OP_Fx1E()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        index += registers[Vx];
    }


    // LD F, Vx ~~ Set I = location of sprite for digit Vx
    void OP_Fx29()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t digit = registers[Vx];

        index = FONTSET_START_ADDRESS + (5 * digit); // since all characters are 5 byte each
    }


    // LD B, Vx ~~ Store BCD representation of digit Vx in memory locations I, I+1, and I+2
    void OP_Fx33()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t value = registers[Vx];

        // Ones-place
        memory[index + 2] = value % 10;
        value /= 10;


        // Tens-place
        memory[index + 1] = value % 10;
        value /= 10;

        // Hundreds-place
        memory[index] = value % 10;
    }


    // LD [I], Vx ~~ Store registers V0 through Vx in memory starting at location I
    void OP_Fx55()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        for (uint8_t i = 0; i <= Vx; ++i)
        {
            memory[index + i] = registers[i];
        }
    }


    // LD Vx, [I] ~~ Read registers V0 through Vx from memory starting at location I
    void OP_Fx65()
    {
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

        for (uint8_t i = 0; i <= Vx; ++i)
        {
            registers[i] = memory[index + i];
        }
    }





    // ------------------------ FUNCTION POINTER ---------------------------------


    // Array of pointers where the opcode is the index, because of scalability 
    // #TODO there is a better faster way to do this like a hashmap or else


    void Table0()
    {
        ((*this).*(table0[opcode & 0x000Fu]))();
    }

    void Table8()
    {
        ((*this).*(table8[opcode & 0x000Fu]))();
    }

    void TableE()
    {
        ((*this).*(tableE[opcode & 0x000Fu]))();
    }

    void TableF()
    {
        ((*this).*(tableF[opcode & 0x00FFu]))();
    }

    void OP_NULL()
    {
    }

    // ----------------- CYCLE -------------------



    void Cycle()
    {
        // Fetch the next instruction in the form of an opcode
        opcode = (memory[pc] << 8u) | memory[pc + 1];

        //Increment pc
        pc += 2;

        // Decode the instruction and execute
        ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

        // Decrement delay timer if it's been set
        if (delayTimer > 0)
        {
            --delayTimer;
        }

        // Decrement the sound timer if it's been set
        if (soundTimer > 0)
        {
            -soundTimer;
        }
    }





};


