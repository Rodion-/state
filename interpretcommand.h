#ifndef INTERPRETCOMMAND_H
#define INTERPRETCOMMAND_H

// command struct
// header a a 2 char
// id game 4 char
// id object 4 char
// id operation 4 char
// args n char
// crc char 2
// pattern ccdd 4 char
// aa idididid idididid idididiid [ args ] CRC ccdd

#include <iostream>
#include <cstring>

class InterpretCommand
{
    #define ID_SIZE 4
    #define ARGS_SIZE 50
    #define HEADER_SIZE	2

    struct Cmd
    {
        int id_game;
        int id_object;
        int id_operation;
        char args[ ARGS_SIZE ];
    };

    Cmd cmd;

    char args[ ARGS_SIZE ];

    int msg_len = 0;

    int flag = 0;

    public:
    InterpretCommand();

    ~InterpretCommand();
    InterpretCommand( const InterpretCommand& ) = delete;
    InterpretCommand( const InterpretCommand&& ) = delete;
    InterpretCommand& operator= ( InterpretCommand& ) = delete;
    InterpretCommand& operator= ( InterpretCommand&& ) = delete;

    void cmd_encod( char* dst , int id_game , int id_object , int id_operation , char* args , int length );
    void cmd_decoder( char* src , int length );

    int get_id_game( void );
    int get_id_object( void );
    int get_id_operation( void );
    void get_args( char* dst );
    int get_args_length( void );
};

#endif // INTERPRETCOMMAND_H
