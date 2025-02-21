#pragma once

#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <random>
#include <chrono>
#include <fstream>
#include <string.h>
#include <iostream>

class Chip8
{
public:
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
        randByte = std::uniform_int_distribution<int>(0, 255);

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
            table0[i] = &Chip8::OP_NULL;
            table8[i] = &Chip8::OP_NULL;
            tableE[i] = &Chip8::OP_NULL;
        }


        // Tables for repeating digits
        table0[0x0] = &Chip8::OP_00E0;
        table0[0xE] = &Chip8::OP_00EE;

        // Functions pointers that indexes correctly
        table8[0x0] = &Chip8::OP_8xy0;
        table8[0x1] = &Chip8::OP_8xy1;
        table8[0x2] = &Chip8::OP_8xy2;
        table8[0x3] = &Chip8::OP_8xy3;
        table8[0x4] = &Chip8::OP_8xy4;
        table8[0x5] = &Chip8::OP_8xy5;
        table8[0x6] = &Chip8::OP_8xy6;
        table8[0x7] = &Chip8::OP_8xy7;
        table8[0xE] = &Chip8::OP_8xyE;

        tableE[0x1] = &Chip8::OP_ExA1;
        tableE[0xE] = &Chip8::OP_Ex9E;


        for (size_t i = 0; i <= 0x65; i++)
        {
            tableF[i] = &Chip8::OP_NULL;
        }

        // Function pointers that indexes correctly
        tableF[0x07] = &Chip8::OP_Fx07;
        tableF[0x0A] = &Chip8::OP_Fx0A;
        tableF[0x15] = &Chip8::OP_Fx15;
        tableF[0x18] = &Chip8::OP_Fx18;
        tableF[0x1E] = &Chip8::OP_Fx1E;
        tableF[0x29] = &Chip8::OP_Fx29;
        tableF[0x33] = &Chip8::OP_Fx33;
        tableF[0x55] = &Chip8::OP_Fx55;
        tableF[0x65] = &Chip8::OP_Fx65;
    }
    void LoadROM(char const* filename);
    void Cycle();

    uint8_t registers[16]{};        // Storage V0 - VF (all CPU operations)
    uint8_t memory[4096]{};         // 4 bytes (interpreter, characters, intructions)
    uint16_t index{};               // Memory addresses
    uint16_t pc{};                  // Address of the next instruction to execute
    uint16_t stack[16]{};           // Tracks order of execution
    uint8_t sp{};                   // Tracks top of stack
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};           // 0 - F input keys
    uint32_t video[64 * 32]{};      // Pixels
    uint16_t opcode = 00E0;	        // Default opcode: clear display



    const unsigned int START_ADDRESS = 0x200;           // Start address for instructions in memory
    const unsigned int FONTSET_START_ADDRESS = 0x50;    // Start address for characters in memory

    const unsigned int VIDEO_WIDTH = 64;
    const unsigned int VIDEO_HEIGHT = 32;


private:
    std::default_random_engine randGen;
    std::uniform_int_distribution<int> randByte;

    typedef void (Chip8::* Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    const static unsigned int FONTSET_SIZE = 80;
    uint8_t fontset[FONTSET_SIZE];

    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();

    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
};

#endif // CHIP8_H