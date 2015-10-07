import argparse
import socket
import time

def valid_sid(sid):
    return len(sid) == 5 and sid.isdigit()

def valid_port(p):
    return 1024 <= p <= 65535

def ecp_query(ip, port, query):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(5.0)

        received = False
        attempt  = 0
        max_no_of_attempts = 3

        while not received and attempt < max_no_of_attempts:
            try:
                sock.sendto(bytes(query + '\n', 'ascii'), (ip, port))
                data = sock.makefile().readline().strip().split()
                received = True
            except socket.timeout as e:
                print(str(e))

                attempt += 1
                if attempt > max_no_of_attempts:
                    return []

                time.sleep(1.0)

        if data == ['ERR'] or data == ['EOF']:
            print('An error has ocurred:', data[0])
            return []
        else:
            return data

def fetch_topics(ip, port):
    data = ecp_query(ip, port, 'TQR')
    if data and data[0] != 'AWT':
        return []
    else:
        return data[2:]

def mkrequest(ip, port, number):
    data = ecp_query(ip, port, 'TER ' + number)
    if data and data[0] != 'AWTES':
        return []
    else:
        return data[1:]

def fetch_pdf(ip, port, sid):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(5.0)

        sock.connect((ip, port))
        sock.sendall(bytes('RQT ' + sid + '\n', 'ascii'))
        reply, qid, deadline = sock.recv(3 + 1 + 24 + 1 + 18).split()

        if reply == 'ERR':
            raise ValueError('Bad query')
        elif reply != b'AQT':
            raise ValueError('Invalid answer from server: ' + reply)

        received = sock.recv(64).strip().split(b' ', 1)
        size     = int(received[0])

        with open(qid.decode('ascii') + '.pdf', 'ab+') as quiz:
            if len(received) > 1:
                quiz.write(received[1])
                size -= len(received[1])

            while size > 0:
                data = sock.recv(4096)
                quiz.write(data)
                size -= len(data)
        return qid

def send_answers(ip, port, sid, qid, answers):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(5.0)

        sock.connect((ip, port))
        sock.sendall(bytes('RQS ' + sid + ' ' + qid + ' ' + ' '.join(answers) + '\n', 'ascii'))
        reply = sock.makefile().readline().strip()

        print('RQS ' + sid + ' ' + qid + ' ' + ' '.join(answers) + '\n')

        if reply == '-1':
            return reply
        elif reply == '-2':
            raise ValueError('Wrong pair SID-QID')
        elif reply == 'ERR':
            raise ValueError('Bad query')
        elif reply[:3] != 'AQS' or len(reply.split()) < 3:
            print('Reply:', reply)
            raise ValueError('Invalid reply from server')

        score = reply.split()[2]
        return score

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
                    print(score)
            elif command == ['exit']:
                break
            else:
                print("Unknown command")
        except Exception as e:
            print('An error has occured:', str(e))

