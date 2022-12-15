#include "async_server.h"

void async_server ::connection :: set_server( async_server* s )
{
    server = s;
}

void async_server :: connection :: create_comand(char *data, int length)
{

}

void async_server :: connection :: read ( void )
{
    socket_.async_read_some(	boost::asio::buffer( data , MAX_SIZE ),
                      [ this ]( boost::system::error_code ec, std::size_t length )
                            {
                                if ( !ec )
                                {
                                    cmd.cmd_decoder( data , length );

                                    char argv[100];

                                    if( cmd.get_args_length() > 4 )
                                    {
                                        if( cmd.get_id_operation() == 1 )
                                        {
                                            //  char argv[50] = "11\0 55\0 33\0 22\0 555\0";
                                            //xy {1,5} velocity {3,2} obj_id 555
                                            cmd.get_args( argv );
                                            std::string x(argv);
                                            std::string y(&argv[x.size()+1]);
                                            std::string velx(&argv[y.size() + 1 + x.size()+1]);
                                            std::string vely(&argv[velx.size()+1 + y.size() + 1 + x.size()+1]);
                                            std::string obj_id(&argv[vely.size()+1 + velx.size()+1 + y.size() + 1 + x.size()+1]);

                                            std::string s[5];
                                            s[0]=x;
                                            s[1]=y;
                                            s[2]=velx;
                                            s[3]=vely;
                                            s[4]=obj_id;

                                            ioc.Resolve( "move" , s );
                                        }
                                    }

                                    char msg[ 100 ] = { "Your id : " };
                                    std::string s = std::to_string(id);
                                    char const *pchar = s.c_str();

                                    memcpy( &msg[ 11 ] , pchar , s.size() );
                                    memcpy( &msg[ 11 + s.size() ] , "\n" , 1 );

                                    int msg_len = 0;
                                    //   cmd.get_data( &msg[ 11 + s.size() + 1 ] , &msg_len );

//                                    argv[ 0 ] = "1";
//                                    argv[ 1 ] = "2";
//                                    argv[ 2 ] = "3";
//                                    argv[ 3 ] = "4";
//                                    argv[ 4 ] = "512";
                                    //            ICommand* movecmd = ioc.Resolve( "move" , argv );
                                    //            movecmd->execute();

                                    write_ack( msg , msg_len + 13 );
                                }
                                else
                                {
                                    if( ec.value() == 2 )
                                    {
                                        std::cout<<"client id : " << id << " disconnect "<<std::endl;

                                        server->erase( id );
                                    }
                                }
                            }
                        );
}

void async_server :: connection :: write_ack ( char* data_ , int len )
{
    memcpy( data , data_ , len );

    std::cout<<std::endl;

    boost::asio::async_write(	socket_,
                    boost::asio::buffer(data, len ),
                        [ this ]( boost::system::error_code ec , std::size_t /*length*/ )
                        {
                            std::cout<<"async_write2 ec : "<<ec.value()<<std::endl;

                            if (!ec)
                            {
                                std::cout<<std::endl;
                                read();
                        }
                        }
                    );
}

void async_server :: connection :: write_echo ( int len )
{

    std::cout<<std::endl;

    boost::asio::async_write(	socket_,
                    boost::asio::buffer(data, len ),
                        [ this ]( boost::system::error_code ec , std::size_t /*length*/ )
                        {
                            if (!ec)
                            {
                                std::cout<<std::endl;
                            read();
                        }
                        }
                    );
}

std::map<int,std::shared_ptr<UObject>>async_server::obj_pool;

async_server :: async_server( boost::asio::io_context& io_context, short port ) : acceptor( io_context , tcp::endpoint( tcp::v4(), port ) )
{
    counter = 0;

    // for example
    obj_pool.insert( {counter /*obj_id*/ , std::make_shared<UObject>() } );
    //

    do_accept();
}

void async_server :: do_accept()
{
    acceptor.async_accept(	[ this  ]( boost::system::error_code ec , tcp::socket socket )
                        {
                            if ( !ec )
                            {
                                std::lock_guard lock(mtx);
                                client_pool.insert( std::pair<int, std::unique_ptr< connection  > > ( counter ,  std::make_unique< connection >(std::move(socket) , counter ) ) );

                                client_pool.at( counter )->set_server( this );

                                client_pool.at( counter )->start();

                                counter++;
                        }
                        do_accept();
                    }
                );
}

void async_server :: erase ( int id )
{
    std::lock_guard lock(mtx);
    client_pool.erase( id );

    std::cout<<"erase id : " << id << " client_pool.size : " << client_pool.size() << std::endl;
}
