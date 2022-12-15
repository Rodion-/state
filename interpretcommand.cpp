#include "interpretcommand.h"

InterpretCommand::InterpretCommand()
{
}

InterpretCommand::~InterpretCommand()
{
}

int InterpretCommand :: get_args_length( void )
{
    return msg_len;
}

int InterpretCommand :: get_id_game( void )
{
    return cmd.id_game;
}

int InterpretCommand :: get_id_object( void )
{
    return cmd.id_object;
}

int InterpretCommand :: get_id_operation( void )
{
    return cmd.id_operation;
}

void InterpretCommand :: get_args( char *dst )
{
    memcpy( dst , cmd.args , msg_len );
}

void InterpretCommand :: cmd_encod(char *dst, int id_game, int id_object, int id_operation, char *args, int length)
{
    Cmd tempcmd;

    tempcmd.id_game = id_game;
    tempcmd.id_object = id_object;
    tempcmd.id_operation = id_operation;
    memcpy( &tempcmd.args , args , length );

    int pos = 0;

    dst[ pos++ ] = 'a';
    dst[ pos++ ] = 'a';
    memcpy( &dst[ pos ] , &tempcmd , ID_SIZE * 3 + length );

    pos += ID_SIZE * 3 + length;

    dst[ pos++ ] = 'c';
    dst[ pos++ ] = 'c';
    dst[ pos++ ] = 'd';
    dst[ pos++ ] = 'd';
}

void InterpretCommand :: cmd_decoder(char* src , int length )
{
    int pos = 0; // индекс для заполнения команды
    int rpos = 0;

    int cmd_len = 0;

    flag = 0;

    for( int i = 0; i < length; i++ )
    {
        if( ( src[ i + 1 ] != 'a' ) && ( src[ i ] != 'a' ) )
        {
            pos++;
            continue;
        }
        else
        {
            flag = 1;

            // copy id_game

            pos += HEADER_SIZE;

            memcpy( &cmd.id_game , &src[ pos ] , 4 );
            pos += ID_SIZE;

            // copy id_object

            memcpy( &cmd.id_object , &src[ pos ] , 4 );
            pos += ID_SIZE;

            // copy id_operation

            memcpy( &cmd.id_operation , &src[ pos ] , 4 );
            pos += ID_SIZE;

            // copy data
            rpos = 0;
            for( int k = 0; k <= ARGS_SIZE; k++ )
            {
                args[ rpos++ ] = src[ pos++ ];

                if( src[ pos - 1 ] == 'c' && src[ pos ] == 'c' && src[ pos + 1 ] == 'd' && src[ pos + 2 ] == 'd' )
                {
                    memcpy( &cmd.args , args , rpos );
                    break;
                }

                cmd_len++;

                if( ( k == ARGS_SIZE - 1 ) && ( src[ pos - 1 ] != 'c' || src[ pos ] != 'd' ) )
                {
                    flag = 0;
                    break;
                }
            }

            if( flag == 0 ) break;

            //copy crc

            // check CRC
            //...

            msg_len = cmd_len - 1;

            break;
        }
    }
}
