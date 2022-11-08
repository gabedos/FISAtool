#!/usr/bin/env python3
"""Manages all of the html page generation"""

from flask import Flask, request, make_response, render_template
from theory import generate_theory

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

    # Fetch arguements
    surv = request.args.get('surv')
    comm = request.args.get('comm')
    target = request.args.get('target')
    targetUS = request.args.get('targetUS')
    recieve = request.args.get('recieve')
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
        radio = False
    else:
        wire = False
        radio = False

    surv = bool_process(surv)
    target = bool_process(target)
    targetUS = bool_process(targetUS)
    recieve = bool_process(recieve)
    sent = bool_process(sent)
    rep_le = bool_process(rep_le)
    consent = bool_process(consent)
    tres = bool_process(tres)

    # Create SPASS theory
    theory = generate_theory(surv, wire, radio, target, targetUS, recieve, sent, rep_le, consent, tres)
    print(theory)

    # Verify the theory
    result = spass_api(theory)

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


def spass_api(theory):
    return ""
