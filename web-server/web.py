import argparse
from poller import Poller

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="CamHart Python Web Server", description="Web server for CS 360", add_help=True)
    parser.add_argument("-p", "--port", type=int, default=8080, help="port number to run the server on")
    parser.add_argument("-d", "--debug", action="store_true", help="Debug output", default=False)
    args = parser.parse_args()
    server = Poller(port=args.port, debug=args.debug)
    server.run()