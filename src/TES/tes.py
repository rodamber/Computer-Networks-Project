#! /usr/bin/env python3

import argparse
import contextlib
import os
import socket
import sys

import handle



@contextlib.contextmanager
def accept(sock):
    client, address = sock.accept()
    yield client, address
    client.close()

def valid_port(p):
    return 1024 <= p <= 65535

def startup():
    with open('startup.txt', 'r') as f:
        topic_name, deadline = f.read().strip().split()
        return (topic_name, deadline)
    return ('', '')

def main():
    topic_name, deadline = startup()

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", help = """TESport is the well-known port where the
                                     TES server accepts user requests, in TCP.
                                     This is an optional argument. If omitted,
                                     it assumes the value 59000."""
                            , metavar = "TESport"
                            , type = int)
    parser.add_argument("-n", help = """ECPname is the name of the machine where
                                     the central evaluation contact point
                                     (ECP) runs. This is an optional
                                     argument. If this argument is omitted,
                                     the ECP should be running on the same
                                     machine."""
                            , metavar = "ECPname"
                            , type = str)

    parser.add_argument("-e", help = """ECPport is the well-known port where the
                                     ECP server accepts user requests, in UDP.
                                     This is an optional argument. If omitted,
                                     it assumes the value 58009."""
                            , metavar = "ECPport"
                            , type = int)

    TESport = 59000
    ECPname = "localhost"
    ECPport = 58009

    args = parser.parse_args()
    if args.p and valid_port(args.p):
        TESport = args.p
    if args.n != ECPname:
        ECPname = args.n
    if args.e and valid_port(args.e):
        ECPport = args.e

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setblocking(True)
        server.bind(('', TESport))
        server.listen(10)
        print("Server bound on port {}".format(TESport))
        while True:
            try:
                with accept(server) as (conn, (ip, port)):
                    conn.settimeout(5.0)
                    print("Accepted {}:{}".format(ip, port))
                    try:
                        pid = os.fork()
                        if pid == 0:
                            request = conn.makefile().readline()
                            print(request)
                            reply = handle(request, deadline, topic_name, ECPname, ECPport)
                            conn.sendall(reply)
                    except Exception as e:
                        print("Connection error: {}:{} : {}".format(ip, port, str(e)))
                        conn.sendall(error)
                    finally:
                        if pid == 0:
                            sys.exit(0)
            except Exception as e:
                print("Failed to accept connection", str(e))



if __name__ == '__main__':
    main()
