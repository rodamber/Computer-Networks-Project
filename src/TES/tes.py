#! /usr/bin/env python3

import argparse
import fcntl
import math
import os
import random
import socket
import sys
import time



months   = ["JAN", "FEV", "MAR", "ABR", "MAI", "JUN", "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"]
deadline = "09OUT2015_20:00:00"
error    = "ERR\n"


def valid_port(p):
    return 1024 <= p <= 65535


def valid_sid(sid):
    return sid.isdigit() and len(sid) == 5

def date():
    m = int(time.strftime("%m")) - 1 # Because lists are 0-indexed.
    clock = time.strftime("%d") + months[m] + time.strftime("%Y_%H:%M:%S")


def handle(request):

    def handle_rqt(request):
        print("Handling RQT request...")

        params = request.split()

        if len(params) != 2:
            print("Error: Bad request")
            return error

        print("Checking SID...")

        sid = params[1]
        if not valid_sid(sid):
            print("Bad request")
            return error

        print("Creating QID...")

        qid = sid + '_' + date()

        print("Choosing file...")

        size = 0
        while (size == 0):
            quiz = random.choice([f for f in os.listdir('.') if f.endswith(".pdf")])
            size = os.stat(quiz).st_size

        print("Saving transaction...")

        with open("transactions.txt", 'a') as f:
            fcntl.flock(f, fcntl.LOCK_EX)
            f.write(sid + ' ' + qid + ' ' + quiz + '\n')
            fcntl.flock(f, fcntl.LOCK_EX)

        print("Sending file...")

        with open(quiz, 'r', encoding = "latin-1") as q:
            return "AQT " + str(qid)  + ' ' \
                          + deadline  + ' ' \
                          + str(size) + ' ' \
                          + ''.join(list(q)) + '\n'

    def handle_rqs(request):

        def check_parameters_of(request):
            params = request.split()
            params_number = len(params)

            if params_number != 8:
                raise ValueError("Error: Number of parameters should be 8 not {}".format(params_number))

            sid     = params[1]
            qid     = params[2]
            answers = list(params[3:])

            return (sid, qid, answers)

        def check_pair(sid, qid):
            with open("transactions.txt", 'r') as f:
                for line in f.readlines():
                    trio = line[:-1].split() # Remove newline and then split.
                    # trio is a list composed of SID, QID and quiz name.
                    if sid == trio[0] and qid == trio[1]:
                        return (True, trio[2])
            return (False, "")

        def check_answers(answers, quiz, deadline):
            quizno = int(quiz[5:8])
            correct_answers = ""

            def predicate(f):
                return f.startswith('T')  and f.endswith('.txt') and int(f[5:8]) == quizno

            answers_file = list(filter(predicate, os.listdir('.')))

            with open(answers_file, 'r') as f:
                lines = f.readlines()
                number_of_questions = len(lines)
                correct_answers = list(map(str.strip, lines))

            points = 0
            for answer, correct in zip(answers, correct_answers):
                if answer == correct:
                    points += 1
                elif answer == 'N':
                    pass
                else:
                    points -= 0.5

            # FIXME: Score should be -1 if answers submited after deadline
            return math.ceil(100 * points / number_of_questions)

        print("Handling RQS request...")
        sid, qid, answers = check_parameters_of(request)

        print("Checking pair SID-QID...")
        checked, quiz = check_pair(sid, qid)

        if checked == False:
            print("Error: SID and QID do not match")
            return "-2\n"

        print("Checking answers...")
        score = str(check_answers(answers, deadline)) + '%'
        print("Scored " + score)

        #FIXME: Send things to ECP before returning

        return score

    if request[:3] == "RQT":
        return handle_rqt(request)
    elif request[:3] == "RQS":
        return handle_rqs(request)
    else:
        return error



if __name__ == '__main__':
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


    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
            server.setblocking(True)
            server.bind(('', TESport))
            server.listen(5)

            print("Bound server on port {}".format(TESport))

            while True:
                try:
                    conn, (ip, port) = server.accept() # FIXME: getting the wrong port!
                    conn.settimeout(5.0)
                    print("Accepted {}:{}".format(ip, port))
                except Exception as e:
                    print("Failed to accept connection: {}".format(str(e)))
                    continue

                try:
                    pid = os.fork()
                except Exception as e:
                    print("Failed to fork: {}".format(str(e)))
                    conn.close()
                else:
                    if pid == 0:
                        try:
                            request = conn.makefile().readline()
                            print(request)
                            conn.sendall(bytes(handle(request), "latin-1"))
                            conn.close()
                            sys.exit(0)
                        except Exception as e:
                            print("Connection error: {}:{} : {}".format(ip, port, str(e)))
                        finally:
                            conn.close()
                            sys.exit(0)

    except Exception as e:
        print("Failed to open server socket on port {}: {}".format(TESport, str(e)))
