import calendar
import datetime
import fcntl
import math
import random
import os
import socket
import time
import traceback

error = bytes('ERR\n', 'ascii')

def valid_sid(sid):
    return sid.isdigit() and len(sid) == 5

def date():
    m = int(time.strftime("%m"))
    months = {k: v for k,v in enumerate(calendar.month_abbr)}
    return time.strftime("%d") + months[m] + time.strftime("%Y_%H:%M:%S")

def check_pair(sid, qid):
    with open("transactions.txt", 'r') as f:
        for line in f.readlines():
            trio = line.strip().split()
            # trio is a list composed of SID, QID and quiz name.
            if sid == trio[0] and qid == trio[1]:
                return (True, trio[2])
    return (False, "")

def after_deadline(deadline):
    def year(d):
        return int(d[5:9])
    def month(d):
        days_of_the_month = {v: k for k,v in enumerate(calendar.month_abbr)}
        return days_of_the_month[d[2:5]]
    def day(d):
        return int(d[:2])
    def hour(d):
        return int(d[10:12])
    def minute(d):
        return int(d[13:15])
    def second(d):
        return int(d[16:18])

    now = date()
    now = datetime.datetime(year(now), month(now), day(now), \
                     hour(now), minute(now), second(now))
    deadline = datetime.datetime(year(deadline), month(deadline), \
                                 day(deadline), hour(deadline),   \
                                 minute(deadline), second(deadline))
    return now > deadline

def check_answers(answers, quiz, deadline):
    if after_deadline(deadline):
        return -1

    quizno = int(quiz[5:8])
    correct_answers = ""

    def predicate(f):
        return f.startswith('T') and f.endswith('A.txt') and int(f[5:8]) == quizno

    answers_file = list(filter(predicate, os.listdir('.')))[0]

    with open(answers_file, 'r') as f:
        lines = f.readlines()
        number_of_questions = len(lines)
        correct_answers = list(map(str.strip, lines))

    points = 0
    for answer, correct in zip(answers, correct_answers):
        if answer.lower() == correct.lower():
            points += 1
        elif answer.lower() == 'n':
            pass
        else:
            points -= 0.5

    if points < 0:
        return 0
    else:
        return math.ceil(100 * points / number_of_questions)

def send_score_to_ECP(sid, qid, topic_name, score, hostname, port):
    ip = socket.gethostbyname(hostname)

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
        client.settimeout(5.0)

        iqr = bytes('IQR ' + ' ' + sid + ' ' + qid + ' ' + topic_name + ' ' + score + '\n', 'ascii')
        tries = 3

        while tries > 0:
            try:
                tries -= 1

                client.sendto(iqr, (ip, port))
                reply = client.makefile().readline()

                print(reply)

                if len(reply) == 29 and reply[:4] == 'AWI ' and reply[4:28] == qid:
                    break
            except socket.timeout as e:
                traceback.print_exc()
                print("Failed communication with ECP:", str(e))

        if tries >= 0:
            return bytes('AQS ' + qid + ' ' + score + '\n', 'ascii')
        else:
            return error

def handle_rqt(request, deadline):
    print("Handling RQT request...")

    params = request.split()
    params_number = len(params)

    if  params_number != 2:
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
        quiz = random.choice([f for f in os.listdir('.') if f.startswith('T') and f.endswith(".pdf")])
        size = os.stat(quiz).st_size

    print("Saving transaction...")

    with open("transactions.txt", 'a') as f:
        fcntl.flock(f, fcntl.LOCK_EX)
        f.write(sid + ' ' + qid + ' ' + quiz + '\n')
        fcntl.flock(f, fcntl.LOCK_EX)

    print("File sent\n")

    with open(quiz, 'rb') as q:
        return bytes("AQT " + str(qid)  + ' '           \
                            + deadline  + ' '           \
                            + str(size) + ' ', 'ascii') \
             + bytes(q.read())                          \
             + bytes('\n', 'ascii')

def handle_rqs(request, deadline, topic_name, ecp_name, ecp_port):

    def check_parameters(request):
        params = request.split()
        params_number = len(params)

        if params_number != 8:
            return (False, "", "", [])

        sid     = params[1]
        qid     = params[2]
        answers = list(params[3:])

        return (True, sid, qid, answers)

    print("Handling RQS request...")
    checked, sid, qid, answers = check_parameters(request)

    if not checked:
        print("Error: Bad request")
        return error

    print("Checking pair SID-QID...")
    checked, quiz = check_pair(sid, qid)

    if not checked:
        print("Error: SID and QID do not match")
        return "-2"

    print("Checking answers...")
    score = str(check_answers(answers, quiz, deadline))
    print("Score: " + score + "%\n")

    return send_score_to_ECP(sid, qid, topic_name, score, ecp_name, ecp_port)

def handle(request, deadline, topic_name, ecp_name, ecp_port):
    if request[:3] == "RQT":
        return handle_rqt(request, deadline)
    elif request[:3] == "RQS":
        return handle_rqs(request, deadline, topic_name, ecp_name, ecp_port)
    else:
        return error
