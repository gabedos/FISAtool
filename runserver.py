"""Starts the server to process HTTP requests (same as pset3)"""

import sys
from argparse import ArgumentParser
from fisa import app

def main():
    """ Starts up HTTP server on specified port """

    parser = ArgumentParser(allow_abbrev=False,
        description="FISA GUI")
    parser.add_argument('port', type=str,
        help="the port on which the server is listening")
    args = parser.parse_args()

    try:
        port = int(args.port)
    except ValueError:
        print('Port must be an integer.', file=sys.stderr)
        sys.exit(1)

    try:
        assert 0 <= port <= 65536
    except AssertionError:
        print('Port must be between integer values of 0 and 65536.')
        sys.exit(1)

    try:
        app.run(host='localhost', port=port, debug=True)
    except Exception as ex:
        print(ex, file=sys.stderr)
        sys.exit(1)

# python3 runserver.py 17290
if __name__ == '__main__':
    main()