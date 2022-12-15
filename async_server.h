#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H

#include <iostream>

#include <ctime>
#include <chrono>

#include <boost/asio.hpp>

#include <thread>
#include <mutex>

#include <map>

#include "UObject.h"
#include "interpretcommand.h"
#include "ioc.h"

using boost::asio::ip::tcp;

using GameObject = std::shared_ptr<UObject>;

class async_server
{
    static std::map<int,std::shared_ptr<UObject>> obj_pool;

    tcp::acceptor acceptor;

    int counter;

    std::mutex mtx;

    class connection
    {
        enum DATA_BUFF
        {
            MAX_SIZE = 1000
        };

        char data[ MAX_SIZE ];

        tcp::socket socket_;

        async_server* server;

        int id;


        Ioc<ICommand> ioc;
        InterpretCommand cmd;

        public:

        connection(tcp::socket _socket , int _id ): socket_(std::move(_socket)) , id( _id )
        {
            std::string argv[ 10 ];

            ioc.Register( "move" , []( std::string argv[] )->ICommand*
            {
                position xy{ std::stoi( argv[ 0 ] ) , std::stoi( argv[ 1 ] ) };
                velocity vel{ std::stoi( argv[ 2 ] ) , std::stoi( argv[ 3 ] ) };

                int obj_id = std::stoi( argv[ 4 ] );

                std::shared_ptr<UObject> obj;
                if( obj_pool.find( obj_id ) != obj_pool.end() )
                {
                    obj = obj_pool.at( obj_id );
                }
                else
                {
                    // throw exception
                }

                return new macroCommandMove( obj , xy , vel );
            });
        }
        ~connection(){}

        connection( const connection& ) = delete;
        connection( const connection&& ) = delete;
        connection& operator= ( connection& ) = delete;
        connection& operator= ( connection&& ) = delete;



        void set_server( async_server* s );

        void start( void ) { read(); }

        void read ( void );

        void write_echo ( int len );

        void write_ack ( char* data_ , int len );

        void create_comand( char* data , int length );
    };

    std::map< int , std::unique_ptr < connection  > > client_pool;

    public:

    async_server( boost::asio::io_context& _io_context , short _port );
    ~async_server(){}
    async_server( const async_server& ) = delete;
    async_server( const async_server&& ) = delete;
    async_server& operator= ( const async_server& ) = delete;
    async_server& operator= ( const async_server&& ) = delete;

    std::shared_ptr<async_server> getptr();
    void erase ( int id );

    private:

    void do_accept();
};



#endif // ASYNC_SERVER_H
