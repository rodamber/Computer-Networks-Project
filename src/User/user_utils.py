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
