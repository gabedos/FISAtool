#!/usr/bin/env python3
"""Manages all of the html page generation"""

from flask import Flask, request, make_response, render_template
from theory import generate_theory
import sys
import os

app = Flask(__name__, template_folder='.')

@app.route('/', methods=['GET'])
@app.route('/home', methods=['GET'])
def index():
    "Generates html for homepage"

    html = render_template('index.html')
    response = make_response(html)
    return response


@app.route('/results', methods=['GET'])
def results():

    # Fetch arguments
    intent = request.args.get('intent')
    surv = request.args.get('surv')
    survUS = request.args.get('survUS')
    comm = request.args.get('comm')
    target = request.args.get('target')
    targetUS = request.args.get('targetUS')
    receive = request.args.get('receive')
    sent = request.args.get('sent')
    rep_le = request.args.get('rep_le')
    consent = request.args.get('consent')
    tres = request.args.get('tres')

    # Process arguments
    if comm == "Wire":
        wire = True
        radio = False
    elif comm == "Radio":
        wire = False
        radio = True
    else:
        wire = False
        radio = False

    intent = bool_process(intent)
    surv = bool_process(surv)
    survUS = bool_process(survUS)
    target = bool_process(target)
    targetUS = bool_process(targetUS)
    receive = bool_process(receive)
    sent = bool_process(sent)
    rep_le = bool_process(rep_le)
    consent = bool_process(consent)
    tres = bool_process(tres)

    # Create SPASS theory
    theory = generate_theory(intent, surv, survUS, wire, radio, target, targetUS, receive, sent, rep_le, consent, tres)

    with open('spass.txt', 'w') as file1:
        file1.write(theory)

    # Verify the theory
    result = spass_api()

    # Dispaly results
    html = render_template('results.html',
        result=result
    )

    response = make_response(html)

    return response


def bool_process(val):

    if val == "T":
        return True
    elif val == "F":
        return False
    else:
        raise Exception("You messed up with input management")


def spass_api() -> str:
    """Runs spass on the theory and determines if conjecture is true"""

    # Executes SPASS theory
    os.system("spass39/SPASS spass.txt > result.txt")

    # Reads the result
    with open('result.txt', 'r') as file1:
        content = file1.read()
    print(content)

    # Return the result
    if("Proof found." in content):
        print("SUCCESS")
        return "True: The circumstances describe electornic surveillance under FISA"
    print("FAIL")
    return "False: The circumstances are not electornic surveillance under FISA"
