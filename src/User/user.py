import argparse
import socket

from user_utils import valid_sid, valid_port, fetch_topics, mkrequest, \
                       fetch_pdf, send_answers

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("SID", metavar = "SID",     type = str)
    parser.add_argument("-n",  metavar = "ECPname", type = str)
    parser.add_argument("-p",  metavar = "ECPport", type = int)

    ECPname = 'localhost'
    ECPport = 58009

    args = parser.parse_args()
    if args.SID and valid_sid(args.SID):
        sid = args.SID
    if args.n:
        ECPname = args.n
    if args.p and valid_port(args.p):
        ECPport = args.p

    ECPip = socket.gethostbyname(ECPname);

    # 'Undefined' variables
    TESip   = -1
    TESport = -1
    qid     = ''

    while True:
        try:
            command = input("Please enter a command: ").strip().split()
            if command == ['list']:
                topic_list = fetch_topics(ECPip, ECPport)
                for i, t in enumerate(topic_list):
                    print('{}. {}'.format(str(i+1), t))
            elif command[0] == 'request' and len(command) > 1:
                TESip, TESport = mkrequest(ECPip, ECPport, command[1])
                TESport        = int(TESport)
                qid            = fetch_pdf(TESip, TESport, sid).decode('ascii')
            elif command[0] == 'submit':
                if len(command[1:]) < 5:
                    print("Insuficient number of answers")
                elif TESip == -1 or TESport == -1 or qid == '':
                    print('Must first make a topic request')
                else:
                    score = send_answers(TESip, TESport, sid, qid, command[1:])
                    print('Score: ' + score + '%')
            elif command == ['exit']:
                break
            else:
                print("Unknown command")
        except Exception as e:
            print('An error has occured:', str(e))

