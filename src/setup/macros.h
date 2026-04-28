#pragma once

#define BTN_PIN 4
#define LED_PIN 5

#define SETBIT(ADDR,BIT)(ADDR |= (1<<BIT))
#define DIRBIT(ADDR, BIT)(ADDR |= (1<<BIT))
#define CLRBIT(ADDR, BIT)(ADDR &= ~(1<<BIT))
#define ZROBIT(ADDR)(ADDR = 0x00)
#define FILLBIT(ADDR)(ADDR |= 0xFF)

#define SCREEN_WIDTH 128
#define CHAR_WIDTH 8
