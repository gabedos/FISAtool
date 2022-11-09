"""Handles theory creation"""

def generate_theory(surv, wire, radio, target, targetUS, receive, sent, rep_le, consent, tres):
    """
    Generates the SPASS theory

    Args:
        surv: surveilance devices is being used?
        wire: wire communication is being surveiled?
        radio: radio communication is being surveiled?
        target: the person is the target (intentionally)?
        targetUS: the target is a us person?
        receive: the communication was received in the US?
        sent: the communication was sent from the US?
        rep_le: is there a reasonable expectation of privacy and warrant for law enforcement?
        consent: all parties have given consent to be surveiled?
        tres: the communication relates to computer trespassers
    """
    
    theory = SPASS_HEADER

    theory += SPASS_FIXED_THEORY

    theory += ""

    if surv:
        theory += "formula(Surv(Device)).\n"
    else:
        theory += "formula(not(Surv(Device))).\n"

    if wire:
        theory += "formula(Wire(Device)).\n"
    else:
        theory += "formula(not(Wire(Device))).\n"

    if radio:
        theory += "formula(Radio(Device)).\n"
    else:
        theory += "formula(not(Radio(Device))).\n"

    if target:
        theory += "formula(Target(Person)).\n"
    else:
        theory += "formula(not(Target(Person))).\n"

    if targetUS:
        theory += "formula(USP(Person)).\n"
    else:
        theory += "formula(not(USP(Person))).\n"

    if receive:
        theory += "formula(ReceiveUS(Contents)).\n"
    else:
        theory += "formula(not(ReceiveUS(Contents))).\n"

    if sent:
        theory += "formula(SentUS(Contents)).\n"
    else:
        theory += "formula(not(SentUS(Contents))).\n"

    if rep_le:
        theory += "formula(REP_LE(Person)).\n"
    else:
        theory += "formula(not(REP_LE(Person))).\n"

    if consent:
        theory += "formula(Consent).\n"
    else:
        theory += "formula(not(Consent)).\n"

    if tres:
        theory += "formula(Tres(Contents)).\n"
    else:
        theory += "formula(not(Tres(Contents))).\n"

    theory += SPASS_CONJECTURE

    return theory


SPASS_HEADER = """
begin_problem(fisa).

list_of_descriptions.
name({*Law Security Logic Project 2*}).
author({*Gabriel Dos Santos (gnd6), Alice Ao (ava26) and Ben M ()*}).
status(unsatisfiable).
description({*Verifying FISA*}).
end_of_list.

list_of_symbols.
functions[
(Person, 0),
(Contents, 0),
(Device, 0)
].

predicates[
(Surv, 1), % determine whether Device is a Surveillance Device
(USP, 1), % determine whether the the target is a US Person
(Target, 1), % determine the person is the target
(Wire, 1), % determine whether communication via wire
(Radio, 1), % determine whether communication via radio
(SentUS, 1), % determine whether communication sent from the US
(ReceiveUS, 1), % determine whether communication received in the US
(REP_LE, 1), % determine whether reasonable expectation of privacy for communication
(Consent, 0), % determine whether any party have given consent
(Tres, 1), % determine whether the communication is computer tresspassing
(ElectronicSurveilance, 0) % determine whether inputs lead to electronic surveilance
].
end_of_list.
"""

SPASS_FIXED_THEORY = """
list_of_formulae(axioms).

% FISA S 1801 f1
formula(
    implies(
        and(
            Surv(Device), USP(Person), Target(Person), REP_LE(Person),
            or(Wire(Contents), Radio(Contents)),
            or(SentUS(Contents), ReceiveUS(Contents))
        ),
        ElectronicSurveilance
    )
).

% FISA S 1801 f2
formula(
    implies(
        and(
            Surv(Device),
            Wire(Contents),
            or(SentUS(Contents), ReceiveUS(Contents)),
            not(Consent),
            % ACQ in US
            not(Tres(Contents))
        ),
        ElectronicSurveilance
    )
).

% FISA S 1801 f3
formula(
    implies(
        and(
            Surv(Device),
            Radio(Contents),
            REP_LE(Contents),
            and(SentUS(Contents), ReceiveUS(Contents))
        ),
        ElectronicSurveilance
    )
).

% FISA S 1801 f4
formula(
    implies(
        and(
            Surv(Device),
            % DEVICE IN THE US
            not(or(Radio(Contents), Wire(Contents))),
            REP_LE(Contents)
        ),
        ElectronicSurveilance
    )
).
"""

SPASS_CONJECTURE = """
end_of_list.

list_of_formulae(conjectures).
% formula(ElectronicSurveilance).
end_of_list.

end_problem.
"""
